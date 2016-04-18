#pragma once
#include "ofMain.h"
//Filename: Terrain.h
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 19/01/2016
//
//Purpose: This is the header file for the Terrain parent class; this class is simply a skeleton for more specific implementations to plug into, and will be used for creating, modifying, and displaying 
// the terrain. This will allow for a modular approach to representing the terrains.

class Terrain  
{
	private:
		ofVec3f OffsetPosition;
	public:
		// Construction & Destruction
		Terrain();
		~Terrain();

		// Store CSG operations as a texture
		std::vector<GLfloat> csgOperations;
		ofBufferObject* csgBuffer;
		ofTexture* csgTable;
		GLuint csgTableTex;

		// Overrideable Techniques
		virtual void Rebuild();
		virtual void Update();
		virtual void Draw();

		virtual void SetOffset(ofVec3f newOffset);
		virtual ofVec3f GetOffset();
		
};