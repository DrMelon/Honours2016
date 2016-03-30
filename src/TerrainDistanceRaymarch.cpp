#include "TerrainDistanceRaymarch.h"

//Filename: TerrainDistanceRaymarch.cpp
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 30/03/2016
//
//Purpose: This is the implementation for a Distance-Field Raymarched Terrain.
//
// This will do most of its work in the pixel/fragment shader on the graphics card: this class will simply draw a 2D plane in front of the camera, at a chosen resolution.
// The rendering will be saved to a frame buffer and then rendered at full scale over the whole screen.
// 

TerrainDistanceRaymarch::TerrainDistanceRaymarch()
{
	// Set up framebuffer
	RaymarchFramebuffer = new ofFbo();
	
	
	// Set up shader
	RaymarchShader = new ofShader();

	RaymarchShader->load("data/shaders/raymarch.vert", "data/shaders/raymarch.frag");

	CurrentCamera = 0;

	Rebuild(320, 240);
}

void TerrainDistanceRaymarch::Rebuild(int newX, int newY)
{
	RaymarchResX = newX;
	RaymarchResY = newY;

	RaymarchFramebuffer->allocate(RaymarchResX, RaymarchResY, GL_RGBA, 0);
	

	


}

void TerrainDistanceRaymarch::Draw()
{
	// First, enable framebuffer.
	RaymarchFramebuffer->begin();
	ofClear(ofColor::black);

	// Enable shader
	RaymarchShader->begin();

	// Update camera information.
	if (CurrentCamera != 0)
	{
		RaymarchShader->setUniform2f("screenResolution", ofVec2f(RaymarchResX, RaymarchResY));
		RaymarchShader->setUniform1i("numIterations", numIterations);
		RaymarchShader->setUniform1f("maximumDepth", maximumDepth);
		RaymarchShader->setUniform3f("cameraPosition", CurrentCamera->getPosition());
		RaymarchShader->setUniform3f("cameraUpVector", CurrentCamera->getUpDir());
		RaymarchShader->setUniform3f("cameraLookTarget", CurrentCamera->getPosition() + (CurrentCamera->getLookAtDir() * 5.0f));

	}

	// Draw rectangle
	ofDrawRectangle(0, 0, RaymarchResX, RaymarchResY);
	//ofRectangle(0, 0, RaymarchResX, RaymarchResY);
	
	RaymarchShader->end();
	RaymarchFramebuffer->end();

	// Now draw the terrain.
	ofDisableDepthTest();
	RaymarchFramebuffer->draw(ofPoint(0, 0), ofGetWindowWidth(), ofGetWindowHeight());
	ofEnableDepthTest();

}


TerrainDistanceRaymarch::~TerrainDistanceRaymarch()
{
}
