#pragma once

//Filename: ofApp.h
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 19/01/2016
//
//Purpose: This is the header file for an openFrameworks application. The overarching program logic will be here.

#include "ofMain.h"
#include "ofxFirstPersonCamera.h"
#include "MeshCutting.h"

#include "Terrain.h"
#include "TerrainGridMarchingCubes.h"
#include "ofxBullet.h"
#include "ofxDatGui.h"


class ofApp : public ofBaseApp{

	public:

		enum TERRAIN_TYPE{ TERRAIN_GRID_MC, TERRAIN_PS_DIST };

		// openFrameworks Template Stuff
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void buildGUI();

		// Gui event handlers
		void onButtonChanged(ofxDatGuiButtonEvent e);
		void onSliderChanged(ofxDatGuiSliderEvent e);

		// Flag to tell physics engine that something has changed
		bool physicsNeedsRebuilding = true;

		// Operations to change terrain via Constructive Solid Geometry (adding/removing regions of terrain via primitives)
		// std::vector<CSGOperation*> csgOperations;
		// CSGOperation will include type, parameters, serialization for buffer.
		// Buffer will have a line of 8 floats: type, x, y, z - then remaining 4 are optionals - bounding, radius etc
		// will need separate list for n-sided polygonals, which will evaluate their density function once and pass in as number; only calculated once


		// Fields/Members
		ofxFirstPersonCamera* theCamera;
		Terrain* theTerrain;
		TERRAIN_TYPE currentTerrainType;

		// Physics Stuff - Built from Vertices from the GPU.
		ofxBulletWorldRigid* thePhysicsWorld;
		ofxBulletTriMeshShape* thePhysicsMesh;
		ofxBulletTriMeshShape* CreatePhysicsMesh(ofxBulletWorldRigid* world, ofMesh* theMesh);
		ofxBulletSphere* testSphere;

		// GUI stuff
		ofxDatGui* theGUI;

		// Terrain stuff

		int GridTerrainResolution = 16;
		float GridTerrainSize = 5;
		float GridExpensiveNormals = 0;

		// Physics stuff

		bool PhysicsEnabled = false;
		float PhysicsTimescale = 1.0f;


		// Mesh Cutting Test
		ofBoxPrimitive*  testBox;
		ofVec3f planeNormal;
		ofVec3f planePoint;

		std::vector<ofMesh*> cutMeshes;
};
