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
#include "TerrainDistanceRaymarch.h"
#include "ofxBullet.h"
#include "ofxDatGui.h"
#include "ofxVoro.h"
#include "Stopwatch.h"


class ofApp : public ofBaseApp{

	public:

		enum TERRAIN_TYPE{ TERRAIN_GRID_MC, TERRAIN_RAY_DIST };

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
		void onDropdownEvent(ofxDatGuiDropdownEvent e);
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

		// Lighting shader
		ofShader* lightShader;

		

		// Physics Stuff - Built from Vertices from the GPU.
		ofxBulletTriMeshShape* CreatePhysicsMesh(ofxBulletWorldRigid* world, ofMesh* theMesh);

		void CheckBodiesAtRest();
		void ConvertMeshToDensity(ofMesh * theMesh, ofVec3f position);
		
		
		// Physics objects
		ofxBulletWorldRigid* thePhysicsWorld;
		ofxBulletTriMeshShape* thePhysicsMesh;
		ofxBulletSphere* testSphere;
		ofxBulletCustomShape* testBox;
	

		// GUI stuff
		ofxDatGui* theGUI;

		// Terrain stuff

		int GridTerrainResolution = 16;
		float GridTerrainSize = 5;
		float GridExpensiveNormals = 0;

		float RayTerrainResolutionX = 320;
		float RayTerrainResolutionY = 240;
		float RayTerrainDrawDistance = 1500.0f;
		int RayTerrainIterations = 256;

		// Physics stuff

		bool PhysicsEnabled = false;
		float PhysicsTimescale = 1.0f;
		bool PhysicsWireframe = false;

		// Terrain modification buffer
		std::vector<GLfloat> csgOperations;
		void CSGAddSphere(ofVec3f Position, float Radius);
		void CSGRemoveSphere(ofVec3f Position, float Radius);
		void CSGUndo();


		// Mesh Cutting Test
		ofBoxPrimitive*  testBoxMesh;
		ofVec3f planeNormal;
		ofVec3f planePoint;

		std::vector<ofMesh*> cutMeshes;
		std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> cutPhysicsObjects;

		// Voronoi Diagram-based Mesh Cutting
		voro::container* voronoiContainer;
		std::vector<ofPoint> voronoiCentres;
		std::vector<ofVboMesh> voronoiMeshes;


		// Analysis/Instrumentation
		Stopwatch theStopwatch;
};
