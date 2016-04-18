#include "TerrainGridMarchingCubes.h"



TerrainGridMarchingCubes::TerrainGridMarchingCubes()
{
	theGrid = new of3dPrimitive();
	theGrid->setUseVbo(true);

	theShader = new ofShader();

	physOffset = ofVec3f(0, 0, 0);

	// Load shader files
	theShader->setGeometryInputType(GL_POINTS);
	theShader->setGeometryOutputCount(16);
	theShader->setGeometryOutputType(GL_TRIANGLE_STRIP);

	// Manual Shader Setup
	theShader->setupShaderFromFile(GL_VERTEX_SHADER, "data/shaders/grid_marching_cubes.vert");
	theShader->setupShaderFromFile(GL_GEOMETRY_SHADER, "data/shaders/grid_marching_cubes.geom");
	theShader->setupShaderFromFile(GL_FRAGMENT_SHADER, "data/shaders/grid_marching_cubes.frag");
	
	// Set Feedback Parameters
	const GLchar* feedbackVaryings[] = { "vertexPosition" };
	glTransformFeedbackVaryings(theShader->getProgram(), 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	theShader->linkProgram();

	// Assign feedback buffer
	outputBuffer = new ofBufferObject();
	outputBuffer->allocate();
	outputBuffer->setData(sizeof(float) * 15 * 3 * XDimension*YDimension*ZDimension, NULL, GL_DYNAMIC_DRAW);

	// Assign output buffer to feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, outputBuffer->getId());

	// Create CSG operations buffer
	csgOperations.clear();
	CSGAddSphere(ofVec3f(0, 0, 0), 10);
	

	csgBuffer = new ofBufferObject();
	csgBuffer->allocate();
	csgBuffer->bind(GL_TEXTURE_BUFFER);
	csgBuffer->setData(csgOperations, GL_STREAM_DRAW);

	csgTable = new ofTexture();
	csgTable->allocateAsBufferTexture(*csgBuffer, GL_R32F);
	csgTable->setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	// Create triangle table.
	triangleBuffer = new ofBufferObject();
	triangleBuffer->allocate();
	triangleBuffer->bind(GL_TEXTURE_BUFFER);
	triangleBuffer->setData(triTableV, GL_STREAM_DRAW);
	
	triangleTable = new ofTexture();
	triangleTable->allocateAsBufferTexture(*triangleBuffer, GL_R32F);
	triangleTable->setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	
	
	theShader->begin();
	theShader->setUniformTexture("tritabletex", *triangleTable, 0);
	theShader->setUniformTexture("csgtex", *csgTable, 1);
	theShader->end();

	
	
	glGenQueries(1, &feedbackQuery);

	Rebuild();
}


TerrainGridMarchingCubes::~TerrainGridMarchingCubes()
{
}

void TerrainGridMarchingCubes::Update()
{
	theGrid->setPosition(OffsetPosition + ofVec3f(-PointScale * XDimension/2, -PointScale*YDimension/2, -PointScale*ZDimension/2));
	time += (float)ofGetLastFrameTime();
}

void TerrainGridMarchingCubes::Draw()
{
	// The drawVertices function draws the vertices in order, and I'm rendering them as GL_POINT type.
	theGrid->getMeshPtr()->setMode(OF_PRIMITIVE_POINTS);

	// Update csg operations table
	csgBuffer->setData(csgOperations, GL_STREAM_DRAW);
	
	// Draw using shader.
	theShader->begin();
		theShader->setUniform1f("gridscale", PointScale);
		theShader->setUniform3f("gridoffset", (theGrid->getPosition()));
		theShader->setUniform1f("isolevel", 0.1f);
		theShader->setUniform1f("expensiveNormals", expensiveNormals);
		theShader->setUniform1f("time", time);
		theShader->setUniform1f("numberOfCSG", csgOperations.size() / 8);
		theShader->setUniformTexture("csgtex", *csgTable, 1);

		if (updatePhysicsMesh)
		{
			glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, feedbackQuery); // <- this line instructs openGL to record how many triangles come back from the geometry shader.
			glBeginTransformFeedback(GL_TRIANGLES);
			
			theGrid->draw();
		
			
			glEndTransformFeedback();
			glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		}
		else
		{
			
			theGrid->draw();
		}

		
		

	theShader->end();

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, outputBuffer->getId());

	// Check to see if we need to update the current mesh.
	if (updatePhysicsMesh)
	{
		// For this operation, we need to fetch the data back from the GPU.

		// We need to know how many vertices to store.
		GLuint numTriangles;
		glGetQueryObjectuiv(feedbackQuery, GL_QUERY_RESULT, &numTriangles);
		
		if (numTriangles < 1)
		{
			return;
		}

		float* feedback = new float[numTriangles * 3 * 3];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(float) * numTriangles * 3 * 3, feedback);

		// Now, we create an ofMesh for the physics terrain to make use of.
		ofMesh newPhysicsMesh;
		for (int i = 0; i < (numTriangles * 3 * 3) - 2; i += 3)
		{
			// sanity-check on mesh; we don't want to generate vertices for blank spaces.

				newPhysicsMesh.addVertex(ofVec3f(feedback[i], feedback[i + 1], feedback[i + 2]));
			
			
			
		}
		for (int i = 0; i < (numTriangles * 3); i++)
		{
			// sanity-check on mesh
		
				newPhysicsMesh.addIndex(i);
			
		}
		
		

		UpdatePhysicsMesh(thePhysicsWorld, &newPhysicsMesh);
		
		

		// Cleanup.
		delete[] feedback;
		feedback = 0;
	}

}

void TerrainGridMarchingCubes::Rebuild(int newX, int newY, int newZ, float newScale)
{
	XDimension = newX;
	YDimension = newY;
	ZDimension = newZ;
	PointScale = newScale;


	// Build an X*Y*Z grid of vertices.
	theGrid->getMeshPtr()->clear();
	for (int i = 0; i < XDimension; i++)
	{
		for (int j = 0; j < YDimension; j++)
		{
			for (int k = 0; k < ZDimension; k++)
			{
				ofVec3f newVertex = ofVec3f(((float)i * PointScale), ((float)j * PointScale), ((float)k * PointScale));
				theGrid->getMeshPtr()->addVertex(newVertex);
				theGrid->getMeshPtr()->addColor(ofColor::red);
			}
		}
	}
	
	updatePhysicsMesh = true;

	outputBuffer->setData(sizeof(float) * 15 * 3 * XDimension*YDimension*ZDimension, NULL, GL_DYNAMIC_DRAW);

}

void TerrainGridMarchingCubes::SetOffset(ofVec3f newOffset)
{
	// Only update physics terrain if new position is a significant distance from the old one
	if (OffsetPosition != newOffset && (physOffset - newOffset).length() > 10.0f)
	{
		updatePhysicsMesh = true;
		physOffset = newOffset;
	}
	OffsetPosition = newOffset;
}

// CSG Operations on this kind of terrain work by filling a texture buffer.
// The texture is 8 elements wide: 
// First element: Add/Subtract operation, 0 or 1
// Second: Shape to be defined. 0: Sphere, 1: Box,
// For spheres, the next 4 elements define the position & radius of the sphere.
// The remaining two are left blank

void TerrainGridMarchingCubes::CSGAddSphere(ofVec3f Position, float Radius)
{
	csgOperations.push_back(0);
	csgOperations.push_back(0);
	csgOperations.push_back(Position.x);
	csgOperations.push_back(Position.y);
	csgOperations.push_back(Position.z);
	csgOperations.push_back(Radius);
	csgOperations.push_back(0);
	csgOperations.push_back(0);
}

void TerrainGridMarchingCubes::CSGRemoveSphere(ofVec3f Position, float Radius)
{
	csgOperations.push_back(1);
	csgOperations.push_back(0);
	csgOperations.push_back(Position.x);
	csgOperations.push_back(Position.y);
	csgOperations.push_back(Position.z);
	csgOperations.push_back(Radius);
	csgOperations.push_back(0);
	csgOperations.push_back(0);
}



void TerrainGridMarchingCubes::UpdatePhysicsMesh(ofxBulletWorldRigid* world, ofMesh* theMesh)
{
	if (thePhysicsMesh != 0)
	{
		thePhysicsMesh->remove();
		//delete thePhysicsMesh;
		//thePhysicsMesh = 0;
	}
	
	if (thePhysicsMesh == 0)
	{
		thePhysicsMesh = new ofxBulletTriMeshShape();
	}
	
	thePhysicsMesh->create(world->world, *theMesh, theGrid->getPosition() - (OffsetPosition), 10000.0f, ofVec3f(-10000, -10000, -10000), ofVec3f(10000, 10000, 10000));
	// Create the physics mesh as a static object. This saves processing time & allows objects to "sleep" on the terrain.
	thePhysicsMesh->getRigidBody()->setCollisionFlags(thePhysicsMesh->getRigidBody()->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	//thePhysicsMesh->activate();
	thePhysicsMesh->add();
	

	

	//thePhysicsMesh->updateMesh(world->world, *theMesh);

	updatePhysicsMesh = false;
}