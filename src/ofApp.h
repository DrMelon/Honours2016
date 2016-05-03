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
#include "GNUPlotData.h"


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
		void mouseScrolled(int x, int y, float scrollx, float scrolly);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void exit();
		
		void gotMessage(ofMessage msg);
		void buildGUI();

		// Gui event handlers
		void onDropdownEvent(ofxDatGuiDropdownEvent e);
		void onButtonChanged(ofxDatGuiButtonEvent e);
		void onSliderChanged(ofxDatGuiSliderEvent e);

		// Flag to tell physics engine that something has changed
		bool physicsNeedsRebuilding = true;




		// Fields/Members
		ofxFirstPersonCamera* theCamera;
		ofVec3f camDelta;
		Terrain* theTerrain;
		TERRAIN_TYPE currentTerrainType;

		// Lighting shader
		ofShader* lightShader;

		// Preview Toggle
		bool PreviewToggle;
		float CarveDistance = 50.0f;

		// Physics Stuff - Built from Vertices from the GPU.
		ofxBulletTriMeshShape* CreatePhysicsMesh(ofxBulletWorldRigid* world, ofMesh* theMesh);

		void CheckBodiesAtRest();
		void ConvertMeshToDensity(ofMesh * theMesh, ofVec3f position);
		
		
		// Physics objects
		ofxBulletWorldRigid* thePhysicsWorld;
		ofxBulletTriMeshShape* thePhysicsMesh;

		// These lists are for user-interaction; when carving out terrain, one could produce a physics object sphere & shatter it if the shift key is held.
		std::vector<ofxBulletSphere*> createdTerrainSpheres;

		// If ctrl is held instead, boxes will be flung at the terrain, which will shatter on impact.
		std::vector<ofxBulletCustomShape*> createdShatterBoxes;
	

		// GUI stuff
		ofxDatGui* theGUI;
		bool ShiftHeld;
		bool CtrlHeld;
		bool GuiNeedsRebuilt;

		// Terrain stuff

		int GridTerrainResolution = 32;
		float GridTerrainSize = 5;
		float GridExpensiveNormals = 0;

		float RayTerrainResolutionX = 1280;
		float RayTerrainResolutionY = 720;
		float RayTerrainDrawDistance = 1500.0f;
		int RayTerrainIterations = 256;

		// Physics stuff

		bool PhysicsEnabled = false;
		float PhysicsTimescale = 1.0f;
		bool PhysicsWireframe = false;

		// Terrain modification buffer
		// Operations to change terrain via Constructive Solid Geometry (adding/removing regions of terrain via primitives)
		// Buffer will have a line of 8 floats: type, x, y, z - then remaining 4 are optionals - bounding, radius etc
		
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
		Stopwatch updateStopwatch;
		Stopwatch drawStopwatch;
		 
		// Graphing Data Storage
		GNUPlotData<int> gnpDrawPerformance;
		GNUPlotData<int> gnpUpdatePerformance;
		GNUPlotData<float> gnpLastFrameTime;
};
