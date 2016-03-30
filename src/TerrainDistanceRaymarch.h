#pragma once
#include "Terrain.h"

//Filename: TerrainDistanceRaymarch.h
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 30/03/2016
//
//Purpose: This is the header file for a Distance-Field Raymarched Terrain.
//
// This will do most of its work in the pixel/fragment shader on the graphics card: this class will simply draw a 2D plane in front of the camera, at a chosen resolution.
// The rendering will be saved to a frame buffer and then rendered at full scale over the whole screen.
// 



class TerrainDistanceRaymarch : public Terrain
{
	public:

		// Resolution of texture
		int RaymarchResX = 320;
		int RaymarchResY = 240;

		// Framebuffer to store texture
		ofFbo* RaymarchFramebuffer;

		// Shader 
		ofShader* RaymarchShader;

		ofCamera* CurrentCamera;


		TerrainDistanceRaymarch();
		~TerrainDistanceRaymarch();
		void Rebuild(int newX, int newY);
		void Draw();


};

