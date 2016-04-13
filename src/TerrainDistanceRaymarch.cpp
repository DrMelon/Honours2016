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

	// Load noise texture
	ofDisableArbTex();
	ofSetTextureWrap(GL_REPEAT, GL_REPEAT);
	noiseTex = new ofImage("data/shaders/noise.png");
	noiseTex->getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
	noiseTex->getTexture().enableMipmap();
	noiseTex->getTexture().generateMipmap();

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

	RaymarchShader->begin();
		//RaymarchShader->setUniformTexture("noisetex", noiseTex->getTexture(), 0);
		RaymarchShader->setUniformTexture("csgtex", *csgTable, 1);
	RaymarchShader->end();

	CurrentCamera = 0;

	accum = 0;

	Rebuild(320, 240);
}

void TerrainDistanceRaymarch::Rebuild(int newX, int newY)
{
	RaymarchResX = newX;
	RaymarchResY = newY;

	RaymarchFramebuffer->allocate(RaymarchResX, RaymarchResY, GL_RGBA, 0);
	
	RaymarchFramebuffer->getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	

}

void TerrainDistanceRaymarch::Draw()
{
	// First, enable framebuffer.
	ofDisableArbTex();
	ofSetTextureWrap(GL_REPEAT, GL_REPEAT);
	RaymarchFramebuffer->begin();
	
	ofClear(ofColor::black);
	
	// Update csg operations table
	csgBuffer->setData(csgOperations, GL_STREAM_DRAW);
	
	// Enable shader
	RaymarchShader->begin();

	accum += 1;


	// Update camera information.
	if (CurrentCamera != 0)
	{
		RaymarchShader->setUniform2f("screenResolution", ofVec2f(RaymarchResX, RaymarchResY));
		RaymarchShader->setUniform1i("numIterations", numIterations);
		RaymarchShader->setUniform1f("maximumDepth", maximumDepth);
		RaymarchShader->setUniform3f("cameraPosition", CurrentCamera->getPosition());
		RaymarchShader->setUniform3f("cameraUpVector", CurrentCamera->getUpDir());
		RaymarchShader->setUniform3f("cameraLookTarget", CurrentCamera->getPosition() + (CurrentCamera->getLookAtDir() * 5.0f));
		RaymarchShader->setUniform1f("numberOfCSG", csgOperations.size() / 8);
		RaymarchShader->setUniform1f("time", accum);
		RaymarchShader->setUniformTexture("csgtex", *csgTable, 1);

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
	ofEnableArbTex();
}

// CSG Operations on this kind of terrain work by filling a texture buffer.
// The texture is 8 elements wide: 
// First element: Add/Subtract operation, 0 or 1
// Second: Shape to be defined. 0: Sphere, 1: Box,
// For spheres, the next 4 elements define the position & radius of the sphere.
// The remaining two are left blank

void TerrainDistanceRaymarch::CSGAddSphere(ofVec3f Position, float Radius)
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

void TerrainDistanceRaymarch::CSGRemoveSphere(ofVec3f Position, float Radius)
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


TerrainDistanceRaymarch::~TerrainDistanceRaymarch()
{
}
