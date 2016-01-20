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
	
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3, 2); //we define the OpenGL version we want to use
	settings.width = 1280;
	settings.height = 720;
	ofCreateWindow(settings);


	ofRunApp(new ofApp());

}
