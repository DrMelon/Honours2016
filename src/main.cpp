#include "ofMain.h"
#include "ofApp.h"

//Filename: main.cpp
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 19/01/2016
//
//Purpose: This is the program entry point.
//
// This is an OpenFrameworks Project, so this just creates an OpenGL context and launches the OpenFrameworks framework.


int main( ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// Set up OpenGL Context


	ofRunApp(new ofApp());

}
