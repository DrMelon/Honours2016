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
		int RaymarchResX = 1280;
		int RaymarchResY = 720;
		float maximumDepth = 1500.0f;
		int numIterations = 256;

		float accum;

		// Framebuffer to store texture
		ofFbo* RaymarchFramebuffer;

		// Shader 
		ofShader* RaymarchShader;

		// Noise Texture
		ofImage* noiseTex;

		// Store CSG operations as a texture
		std::vector<GLfloat> csgOperations;
		ofBufferObject* csgBuffer;
		ofTexture* csgTable;
		GLuint csgTableTex;

		// Camera Ref
		ofCamera* CurrentCamera;




		TerrainDistanceRaymarch();
		~TerrainDistanceRaymarch();
		void Rebuild(int newX, int newY);
		void Draw();

		void CSGAddSphere(ofVec3f Position, float Radius);
		void CSGRemoveSphere(ofVec3f Position, float Radius);


};

