#pragma once
#include "Terrain.h"



//Filename: TerrainGridMarchingCubes.h
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 19/01/2016
//
//Purpose: This is the header file for a grid-based Marching Cubes Terrain. 
//
// This will do most of its work in the geometry shader on the graphics card: this class will simply draw a 3D grid of vertices in front of the camera, moving with it. This means that
// the world-space position of each of the points in the grid will change as the camera is moved around, and those points will become the cell centres that the marching cubes algorithm
// will sample density around, in the geometry shader.
//
// This will be fairly lightweight on the CPU side; it's likely, in fact, that it won't even store very much in memory, holding only one single mesh.


class TerrainGridMarchingCubes : public Terrain
{
	private:
		of3dPrimitive* theGrid;
		ofVec3f OffsetPosition;

	public:
		// Fields
		int XDimension = 16;
		int YDimension = 16;
		int ZDimension = 16;
		float PointScale = 1.0f;


		// Methods
		TerrainGridMarchingCubes();
		~TerrainGridMarchingCubes();
		virtual void Update();
		virtual void Draw();
		virtual void Rebuild(int newX = 16, int newY = 16, int newZ = 16, float newScale = 10.0f);
		virtual void SetOffset(ofVec3f newOffset);
};

