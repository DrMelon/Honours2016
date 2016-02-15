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

	// Make the physics world.
	thePhysicsWorld = new ofxBulletWorldRigid();
	thePhysicsWorld->setup();
	thePhysicsWorld->setCamera(theCamera);

	// Create a blank mesh
	ofMesh singleTriangle;
	singleTriangle.addVertex(ofVec3f(0, 0, 0));
	singleTriangle.addVertex(ofVec3f(1, 0, 0));
	singleTriangle.addVertex(ofVec3f(1, 1, 0));
	singleTriangle.addIndex(0);
	singleTriangle.addIndex(1);
	singleTriangle.addIndex(2);
	thePhysicsMesh = CreatePhysicsMesh(thePhysicsWorld, &singleTriangle);

	((TerrainGridMarchingCubes*)theTerrain)->updatePhysicsMesh = true;

}

//--------------------------------------------------------------
void ofApp::update()
{
	// Update gui
	theGUI->update();

	// Update camera offset for terrain.
	theTerrain->SetOffset(theCamera->getPosition());

	// Set terrain variables.
	if (currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		((TerrainGridMarchingCubes*)theTerrain)->expensiveNormals = GridExpensiveNormals;
		//((TerrainGridMarchingCubes*)theTerrain)->updatePhysicsMesh = physicsNeedsRebuilding;
		((TerrainGridMarchingCubes*)theTerrain)->thePhysicsWorld = thePhysicsWorld;
		((TerrainGridMarchingCubes*)theTerrain)->thePhysicsMesh = thePhysicsMesh;
	}

	
	
	theTerrain->Update();


	
}

//--------------------------------------------------------------
void ofApp::draw()
{
	theCamera->begin(); // Begin drawing with this camera.

		// Draw the terrain.
		theTerrain->Draw();

		// Debug: draw the physics mesh
		
		thePhysicsWorld->drawDebug();


	theCamera->end(); // Cease drawing with the camera.


	// Draw GUI
	ofDisableDepthTest();
	theGUI->draw();
	ofEnableDepthTest();

	// Check to see if terrain rebuilt our physics during its draw phase.
	if (physicsNeedsRebuilding && currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		if (((TerrainGridMarchingCubes*)theTerrain)->updatePhysicsMesh == false)
		{
			physicsNeedsRebuilding = false;
		}

	}

	


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	//
	// Marching Cube Grid Terrain Controls.
	//
	if (currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
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

void ofApp::onSliderChanged(ofxDatGuiSliderEvent e)
{
	
	
}

void ofApp::onButtonChanged(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "Rebuild Terrain" && currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
	}
	if (e.target->getName() == "Smooth Normals" && currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		GridExpensiveNormals = e.enabled;
	}

}

// Build GUI
void ofApp::buildGUI()
{
	if (theGUI != 0)
	{
		delete theGUI;
		theGUI = 0;
		theGUI = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
		theGUI->setTheme(new ofxDatGuiThemeWireframe());
		theGUI->setAutoDraw(false);
		theGUI->onButtonEvent(this, &ofApp::onButtonChanged);
		theGUI->onSliderEvent(this, &ofApp::onSliderChanged);
	}

	theGUI->addLabel("Real-Time Physics Based Destruction With\nDensity-Field Terrains");
	theGUI->addLabel("J. Brown (1201717)");
	theGUI->addBreak()->setHeight(2.0f);

	ofxDatGuiFolder* diagnosticsFolder = theGUI->addFolder("Diagnostics", ofColor::white);
	diagnosticsFolder->addFRM();
	theGUI->addBreak()->setHeight(2.0f);

	theGUI->addLabel("Terrain Type: ");
	vector<string> terrainOptions = { "Grid-Based Naive Marching Cubes", "Grid-Based Optimised Marching Cubes", "Raymarched Distance Field" };
	theGUI->addDropdown("Grid-Based Naive Marching Cubes",terrainOptions);
	theGUI->addBreak()->setHeight(2.0f);

	ofxDatGuiFolder* terrainFolder = theGUI->addFolder("Terrain Controls", ofColor::darkCyan);
	if (currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		auto gridResolutionSlider = terrainFolder->addSlider("Grid Resolution", 3, 128, 64);
		gridResolutionSlider->setPrecision(0);
		gridResolutionSlider->bind(GridTerrainResolution);

		auto gridScaleSlider = terrainFolder->addSlider("Grid Zoom", 1, 20, 5);
		gridScaleSlider->setPrecision(1);
		gridScaleSlider->bind(GridTerrainSize);

		auto gridNormalsToggle = terrainFolder->addToggle("Smooth Normals", false);
		

		terrainFolder->addButton("Rebuild Terrain");
	}
	
	

}

ofxBulletTriMeshShape* ofApp::CreatePhysicsMesh(ofxBulletWorldRigid* world, ofMesh* theMesh)
{
	ofxBulletTriMeshShape* newShape = new ofxBulletTriMeshShape();
	newShape->create(world->world, *theMesh, ofVec3f(0, 0, 0), 1.0f);
	newShape->add();
	newShape->enableKinematic();
	newShape->setActivationState(DISABLE_DEACTIVATION);


	return newShape;
}
