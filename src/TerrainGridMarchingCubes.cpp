#include "TerrainGridMarchingCubes.h"



TerrainGridMarchingCubes::TerrainGridMarchingCubes()
{
	theGrid = new of3dPrimitive();

	theShader = new ofShader();

	// Load shader files
	theShader->load("data/shaders/grid_marching_cubes.vert", "data/shaders/grid_marching_cubes.frag", "data/shaders/grid_marching_cubes.geom");
	//theShader->load("data/shaders/grid_marching_cubes.vert", "data/shaders/grid_marching_cubes.frag");
	theShader->setGeometryInputType(GL_POINT);

	// Create triangle table.
	triangleBuffer = new ofBufferObject();
	triangleBuffer->allocate(sizeof(triTable), triTable, GL_DYNAMIC_READ);
	triangleBuffer->setData(sizeof(triTable), triTable, GL_DYNAMIC_READ);
	triangleTable = new ofTexture();
	triangleTable->disableMipmap();
	triangleTable->allocateAsBufferTexture(*triangleBuffer, GL_DYNAMIC_READ);
	
	Rebuild();
}


TerrainGridMarchingCubes::~TerrainGridMarchingCubes()
{
}

void TerrainGridMarchingCubes::Update()
{
	theGrid->setPosition(OffsetPosition + ofVec3f(-PointScale * XDimension/2, -PointScale*YDimension/2, -PointScale*ZDimension/2));

}

void TerrainGridMarchingCubes::Draw()
{
	// The drawVertices function draws the vertices in order, and I'm rendering them as GL_POINT type.
	theGrid->getMeshPtr()->setMode(OF_PRIMITIVE_POINTS);
	
	// Bind buffer texture
	triangleTable->bind();

	// Draw using shader.
	theShader->begin();
		theShader->setUniform1f("gridscale", PointScale);
		theShader->setUniform3f("gridoffset", OffsetPosition);
		theShader->setUniform1f("isolevel", 0);
		
		theGrid->draw(OF_MESH_POINTS);
		

	theShader->end();
	
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
