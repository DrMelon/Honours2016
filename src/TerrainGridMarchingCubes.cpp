#include "TerrainGridMarchingCubes.h"



TerrainGridMarchingCubes::TerrainGridMarchingCubes()
{
	theGrid = new of3dPrimitive();
	theShader = new ofShader();

	// Load shader files
	theShader->load("data/shaders/grid_marching_cubes.vert", "data/shaders/grid_marching_cubes.frag", "data/shaders/grid_marching_cubes.geom");
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
	ofColor(1.0f, 0.0f, 0.0f);
	theGrid->getMeshPtr()->setMode(OF_PRIMITIVE_POINTS);
	theGrid->drawVertices();
	
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
