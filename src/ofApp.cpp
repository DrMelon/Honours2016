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
	theGUI->setTheme(new ofxDatGuiThemeCharcoal());
	
	// Disable the GUI's own automatic rendering, as we're doing shader passes and things.
	theGUI->setAutoDraw(false);

	// Add elements to GUI.
	buildGUI();

	// Create the camera, using OpenFrameworks' ofEasyCam class. This gives us a simple control system.
	theCamera = new ofxFirstPersonCamera();
	
	// Scalar value for shader to determine if we should use smoothed normals or not on the grid terrain.
	GridExpensiveNormals = 0.0f;
	
	// Make the terrain, starting off with using the GridMarchingCubes implementation.
	theTerrain = new TerrainGridMarchingCubes();
	((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
	currentTerrainType = TERRAIN_TYPE::TERRAIN_GRID_MC;

	// Make the physics world.
	thePhysicsWorld = new ofxBulletWorldRigid();
	thePhysicsWorld->setup();

	// Set up gravity
	thePhysicsWorld->setGravity(ofVec3f(0, -9.81f, 0));
	thePhysicsWorld->enableGrabbing();
	thePhysicsWorld->setCamera(theCamera);

	// Create the physics sphere
	testSphere = new ofxBulletSphere();
	testSphere->create(thePhysicsWorld->world, theCamera->getPosition() + theCamera->upvector * 20, 1.0, 2.0);
	
	testBoxMesh = new ofBoxPrimitive(5, 5, 5, 2, 2, 2);

	testBox = new ofxBulletCustomShape();
	testBox->addMesh(testBoxMesh->getMesh(), ofVec3f(1, 1, 1), false);
	testBox->create(thePhysicsWorld->world, theCamera->getPosition() + theCamera->upvector * 35, 1.0f);
	
	testSphere->add();
	testBox->add();

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
	planeNormal = ofVec3f(ofRandomf(), ofRandomf(), ofRandomf());
	planeNormal.normalize();
	planePoint = ofVec3f(ofRandomf(), ofRandomf(), ofRandomf());
	//planeNormal = ofVec3f(0.5, 0.5, 0);
	//planePoint = ofVec3f(0, 1, 2);

	//cutMeshes = CutMeshWithPlane(planePoint, planeNormal, testBox->getMesh());

	
	
}

//--------------------------------------------------------------
void ofApp::update()
{
	float deltaTime = ofGetLastFrameTime();
	
	// Update GUI
	auto frametimeGUI = theGUI->getTextInput("Frame-Time", "Diagnostics");
	frametimeGUI->setText(std::to_string(deltaTime) + " ms");
	auto frametimePlot = theGUI->getValuePlotter("FT", "Diagnostics");
	frametimePlot->setValue(deltaTime);
	//frametimePlot->setSpeed(0.1f);

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

	
	// Update terrain
	theTerrain->Update();

	// Update physics
	if (PhysicsEnabled)
	{
		thePhysicsWorld->update(deltaTime * (PhysicsTimescale*2), 0);
	}


	
}

//--------------------------------------------------------------
void ofApp::draw()
{
	theCamera->begin(); // Begin drawing with this camera.

		// Draw the terrain.
		theTerrain->Draw();

		// Debug: draw the physics mesh
		ofDisableDepthTest();
		if (thePhysicsWorld->world->getDebugDrawer() != 0)
		{
			thePhysicsWorld->world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		}
		
		if (PhysicsWireframe)
		{
			thePhysicsWorld->drawDebug();
		}
		
		
		ofEnableDepthTest();
		// Draw physics sphere

		testSphere->draw();

		// Draw physics box
		if (testBox->getCollisionShape() != NULL)
		{
			
			testBox->transformGL();
			if (!PhysicsWireframe)
			{
				testBoxMesh->draw();
			}
			else
			{
				testBoxMesh->drawWireframe();
			}
			
			testBox->restoreTransformGL();
		}
		
		// Draw voronoi
		for (int i = 0; i < voronoiMeshes.size(); i++)
		{
			voronoiMeshes.at(i).drawWireframe();
		}



		for (int i = 0; i < cutPhysicsObjects.size(); i++)
		{
			//cutPhysicsObjects.at(i).second->draw();
			cutPhysicsObjects.at(i).second->transformGL();
			
			if (PhysicsWireframe)
			{
				cutPhysicsObjects.at(i).first->drawWireframe();
			}
			else
			{
				cutPhysicsObjects.at(i).first->draw();
			}
			
			cutPhysicsObjects.at(i).second->restoreTransformGL();
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
	if (e.target->getName() == "Physics Enabled")
	{
		PhysicsEnabled = e.enabled;
	}
	if (e.target->getName() == "Wireframe")
	{
		PhysicsWireframe = e.enabled;
	}
	if (e.target->getName() == "Slice")
	{


		//cutMeshes = CutMeshWithPlane(planePoint, planeNormal, sliceMesh);
		//cutPhysicsObjects = SlicePhysicsObject(testBox, testBoxMesh->getMeshPtr(), planePoint + testBox->getPosition(), planeNormal, thePhysicsWorld, true);

		// Do voronoi test

		cutPhysicsObjects = VoronoiFracture(testBox, testBoxMesh->getMeshPtr(), thePhysicsWorld, 4, NULL);

		// Move meshes to new locations
		/*
		for (int mesh = 0; mesh < voronoiMeshes.size(); mesh++)
		{
			for (int i = 0; i < voronoiMeshes.at(mesh).getVertices().size(); i++)
			{
				voronoiMeshes.at(mesh).getVertices().at(i) += origin;
			}
		}*/
		
		// Cut object by planes



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
		theGUI->setWidth(500);
	}

	auto titleLabel = theGUI->addLabel("Real-Time Physics Based Destruction With Density-Field Terrains");
	
	
	theGUI->addLabel("J. Brown (1201717)");
	theGUI->addBreak()->setHeight(2.0f);

	ofxDatGuiFolder* diagnosticsFolder = theGUI->addFolder("Diagnostics", ofColor::white);
	diagnosticsFolder->addFRM();
	diagnosticsFolder->addTextInput("Frame-Time", "0ms");
	auto diagPlot = diagnosticsFolder->addValuePlotter("FT", 0.00f, 0.1f);
	diagPlot->setDrawMode(ofxDatGuiGraph::FILLED);
	diagPlot->setSpeed(2.0f);
	
	
	


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
	physicsFolder->addToggle("Wireframe", false);

	auto physicsSlider = physicsFolder->addSlider("Timescale", 0.01f, 1.0f, 1.0f);
	physicsSlider->setPrecision(2);
	physicsSlider->bind(PhysicsTimescale);
	
	auto sliceButton = physicsFolder->addButton("Slice");


	

	auto footerGUI = theGUI->addFooter();
	footerGUI->setLabelWhenCollapsed(":: SHOW TOOLS ::");
	footerGUI->setLabelWhenExpanded(":: HIDE TOOLS ::");

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
