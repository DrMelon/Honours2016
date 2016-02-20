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
	thePhysicsWorld->setGravity(ofVec3f(0, -98.1f, 0));
	thePhysicsWorld->enableGrabbing();
	thePhysicsWorld->setCamera(theCamera);

	// Create the physics sphere
	testSphere = new ofxBulletSphere();
	testSphere->create(thePhysicsWorld->world, theCamera->getPosition() + theCamera->upvector * 20, 1.0, 2.0);
	testSphere->setActivationState(DISABLE_DEACTIVATION);
	
	testSphere->add();

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

	// Test mesh cutting
	testBox = new ofBoxPrimitive(10, 10, 10, 4, 4, 4);
	planeNormal = ofVec3f(ofRandomf(), ofRandomf(), ofRandomf());
	planeNormal.normalize();
	planePoint = ofVec3f(ofRandomf(), ofRandomf(), ofRandomf());

	cutMeshes = CutMeshWithPlane(planePoint, planeNormal, testBox->getMesh());
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

	if (PhysicsEnabled)
	{
		
		thePhysicsWorld->update(0.016f * PhysicsTimescale);
	}
	
	
}

//--------------------------------------------------------------
void ofApp::draw()
{
	theCamera->begin(); // Begin drawing with this camera.

		// Draw the terrain.
		theTerrain->Draw();

		// Debug: draw the physics mesh
		
		//thePhysicsWorld->drawDebug();

		// Draw physics sphere
		testSphere->draw();

		// Draw cut meshes
		for (int i = 0; i < cutMeshes.size(); i++)
		{
			cutMeshes.at(i)->drawWireframe();
		}


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
	if (e.target->getName() == "Physics Enabled" && currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		PhysicsEnabled = e.enabled;
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

	ofxDatGuiFolder* physicsFolder = theGUI->addFolder("Physics", ofColor::red);

	physicsFolder->addToggle("Physics Enabled", false);

	auto physicsSlider = physicsFolder->addSlider("Timescale", 0.01f, 1.0f, 1.0f);
	physicsSlider->setPrecision(2);
	physicsSlider->bind(PhysicsTimescale);
	
	

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
