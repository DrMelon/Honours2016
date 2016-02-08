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
	ofSetBackgroundColor(ofColor::white);
	ofSetVerticalSync(true);

	// Set up z-buffer.
	ofEnableDepthTest();

	// Set up GUI
	theGUI = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	theGUI->setTheme(new ofxDatGuiThemeWireframe());
	theGUI->setAutoDraw(false);

	// Add elements to GUI.
	buildGUI();

	// Create the camera, using OpenFrameworks' ofEasyCam class. This gives us a simple control system.
	theCamera = new ofxFirstPersonCamera();
	GridExpensiveNormals = 0.0f;
	
	// Make the terrain, starting off with using the GridMarchingCubes implementation.
	theTerrain = new TerrainGridMarchingCubes();
	((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
	currentTerrainType = TERRAIN_TYPE::TERRAIN_GRID_MC;
}

//--------------------------------------------------------------
void ofApp::update()
{
	// Update gui
	theGUI->update();

	// Update camera offset for terrain.
	theTerrain->SetOffset(theCamera->getPosition());
	if (currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		((TerrainGridMarchingCubes*)theTerrain)->expensiveNormals = GridExpensiveNormals;
	}
	
	theTerrain->Update();
	
}

//--------------------------------------------------------------
void ofApp::draw()
{
	theCamera->begin(); // Begin drawing with this camera.

		// Draw the terrain.
		theTerrain->Draw();


	theCamera->end(); // Cease drawing with the camera.


	// Draw GUI
	ofDisableDepthTest();
	theGUI->draw();
	ofEnableDepthTest();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	//
	// Marching Cube Grid Terrain Controls.
	//
	if (currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		if (key == 'o')
		{
			GridTerrainResolution *= 2;
			GridTerrainSize /= 2;
			((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
		}
		if (key == 'i')
		{
			GridTerrainResolution /= 2;
			GridTerrainSize *= 2;
			((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
		}
		if (key == 'u')
		{
			//GridTerrainResolution /= 2;
			GridTerrainSize /= 2;
			((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
		}
		if (key == 'y')
		{
			//GridTerrainResolution /= 2;
			GridTerrainSize *= 2;
			((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
		}
		if (key == 't')
		{
			GridTerrainResolution *= 2;
			//GridTerrainSize /= 2;
			((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
		}
		if (key == 'r')
		{
			GridTerrainResolution /= 2;
			//GridTerrainSize *= 2;
			((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
		}
		if (key == 'p')
		{
			if (GridExpensiveNormals == 0.0f)
			{
				GridExpensiveNormals = 1.0f;
			}
			else
			{
				GridExpensiveNormals = 0.0f;
			}
		}
	}


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
	if (button == 2)
	{
		theCamera->toggleControl();
	}
	
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

// Build GUI
void ofApp::buildGUI()
{
	theGUI->addLabel("Real-Time Physics Based Destruction With\nDensity-Field Terrains");
	theGUI->addLabel("J. Brown (1201717)");
	theGUI->addBreak()->setHeight(2.0f);
	ofxDatGuiFolder* diagnosticsFolder = theGUI->addFolder("Diagnostics", ofColor::white);
	diagnosticsFolder->addFRM();
	theGUI->addBreak()->setHeight(2.0f);
	theGUI->addLabel("Terrain Type: ");
	vector<string> terrainOptions = { "Grid-Based Naive Marching Cubes", "Grid-Based Optimised Marching Cubes" };
	theGUI->addDropdown("Grid-Based Naive Marching Cubes",terrainOptions);
	theGUI->addBreak()->setHeight(2.0f);
	ofxDatGuiFolder* terrainFolder = theGUI->addFolder("Terrain Controls", ofColor::darkCyan);
	

}
