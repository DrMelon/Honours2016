#include "TerrainGridMarchingCubes.h"



TerrainGridMarchingCubes::TerrainGridMarchingCubes()
{
	theGrid = new of3dPrimitive();
	theGrid->setUseVbo(true);

	theShader = new ofShader();

	// Load shader files
	theShader->setGeometryInputType(GL_POINTS);
	theShader->setGeometryOutputCount(16);
	theShader->setGeometryOutputType(GL_TRIANGLE_STRIP);
	//theShader->load("data/shaders/grid_marching_cubes.vert", "data/shaders/grid_marching_cubes.frag", "data/shaders/grid_marching_cubes.geom");
	//theShader->load("data/shaders/grid_marching_cubes.vert", "data/shaders/grid_marching_cubes.frag");
	// Manual Shader Setup
	theShader->setupShaderFromFile(GL_VERTEX_SHADER, "data/shaders/grid_marching_cubes.vert");
	theShader->setupShaderFromFile(GL_GEOMETRY_SHADER, "data/shaders/grid_marching_cubes.geom");
	theShader->setupShaderFromFile(GL_FRAGMENT_SHADER, "data/shaders/grid_marching_cubes.frag");
	
	// Set Feedback Parameters
	const GLchar* feedbackVaryings[] = { "normalOfVertex" };
	glTransformFeedbackVaryings(theShader->getProgram(), 1, feedbackVaryings, GL_SEPARATE_ATTRIBS);
	theShader->linkProgram();

	// Assign feedback buffer
	outputBuffer = new ofBufferObject();
	outputBuffer->allocate();
	outputBuffer->setData(sizeof(float) * 3 * XDimension*YDimension*ZDimension, NULL, GL_DYNAMIC_DRAW);

	// Assign output buffer to feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, outputBuffer->getId());


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
	theShader->end();
	

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

	// Draw using shader.
	theShader->begin();
		theShader->setUniform1f("gridscale", PointScale);
		theShader->setUniform3f("gridoffset", OffsetPosition);
		theShader->setUniform1f("isolevel", 0);
		theShader->setUniform1f("expensiveNormals", expensiveNormals);
		theShader->setUniform1f("time", time);

		glBeginTransformFeedback(GL_TRIANGLES);
		theGrid->draw();
		glEndTransformFeedback();
		

	theShader->end();

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, outputBuffer->getId());
	float* feedback = new float[XDimension*YDimension*ZDimension * 3 * 15];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

	cout << feedback[0] << endl;
	delete feedback;
	feedback = 0;
	
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
				theGrid->getMeshPtr()->addVertex(ofVec3f(((float)i * PointScale), ((float)j * PointScale), ((float)k * PointScale)));
				theGrid->getMeshPtr()->addColor(ofColor::red);
			}
		}
	}

	

}

void TerrainGridMarchingCubes::SetOffset(ofVec3f newOffset)
{
	OffsetPosition = newOffset;
}
