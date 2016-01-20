#include "ofApp.h"

//Filename: ofApp.cpp
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 19/01/2016
//
//Purpose: This is the code file for an openFrameworks application; the overarching program logic will be here.

//--------------------------------------------------------------
void ofApp::setup()
{
	// Set maximum framerate.
	ofSetFrameRate(60);
	ofSetBackgroundColor(ofColor::black);
	ofSetVerticalSync(true);

	// Set up z-buffer.
	ofEnableDepthTest();

	// Create the camera, using OpenFrameworks' ofEasyCam class. This gives us a simple control system.
	theCamera = new ofxFirstPersonCamera();
	
	// Make the terrain, starting off with using the GridMarchingCubes implementation.
	theTerrain = new TerrainGridMarchingCubes();
	
}

//--------------------------------------------------------------
void ofApp::update()
{
	// Update camera offset for terrain.
	theTerrain->SetOffset(theCamera->getPosition());
	theTerrain->Update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	theCamera->begin(); // Begin drawing with this camera.

		// Draw the terrain.
		theTerrain->Draw();


	theCamera->end(); // Cease drawing with the camera.
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	theCamera->toggleControl();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
