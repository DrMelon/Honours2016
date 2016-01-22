#pragma once

//Filename: ofApp.h
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 19/01/2016
//
//Purpose: This is the header file for an openFrameworks application. The overarching program logic will be here.

#include "ofMain.h"
#include "ofxFirstPersonCamera.h"

#include "Terrain.h"
#include "TerrainGridMarchingCubes.h"


class ofApp : public ofBaseApp{

	public:

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

		// Fields/Members
		ofxFirstPersonCamera* theCamera;
		Terrain* theTerrain;

		int GridTerrainResolution = 64;
		float GridTerrainSize = 5;
		float GridExpensiveNormals = 0;

		
};
