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
	ofSetBackgroundColor(ofColor(182, 227, 242));
	ofSetVerticalSync(true);
	ofDisableArbTex();

	// Set up z-buffer.
	ofEnableDepthTest();

	// Set up GUI
	theGUI = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	theGUI->setTheme(new ofxDatGuiThemeCharcoal());
	
	// Disable the GUI's own automatic rendering, as we're doing shader passes and things.
	theGUI->setAutoDraw(false);



	// Create the camera, using OpenFrameworks' ofEasyCam class. This gives us a simple control system.
	theCamera = new ofxFirstPersonCamera();
	theCamera->setNearClip(0.01f);
	theCamera->setFarClip(1500.f);

	// Create lighting shader
	lightShader = new ofShader();
	lightShader->load("data/shaders/directional_light.vert", "data/shaders/directional_light.frag");
	
	// Scalar value for shader to determine if we should use smoothed normals or not on the grid terrain.
	GridExpensiveNormals = 0.0f;
	
	// Make the terrain, starting off with using the GridMarchingCubes implementation.
	theTerrain = new TerrainGridMarchingCubes();
	((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
	//theTerrain = new TerrainDistanceRaymarch();
	//((TerrainDistanceRaymarch*)theTerrain)->Rebuild(1280, 720);
	//((TerrainDistanceRaymarch*)theTerrain)->CurrentCamera = theCamera;
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
	
	testBoxMesh = new ofBoxPrimitive(10, 10, 10, 1, 1, 1);

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

	// Add elements to GUI.
	buildGUI();
	
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
	else if (currentTerrainType == TERRAIN_TYPE::TERRAIN_RAY_DIST)
	{
		((TerrainDistanceRaymarch*)theTerrain)->numIterations = RayTerrainIterations;
		((TerrainDistanceRaymarch*)theTerrain)->maximumDepth = RayTerrainDrawDistance;
		((TerrainDistanceRaymarch*)theTerrain)->RaymarchResX = RayTerrainResolutionX;
		((TerrainDistanceRaymarch*)theTerrain)->RaymarchResY = RayTerrainResolutionY;
	}

	
	// Update terrain
	theTerrain->Update();

	// Update physics
	if (PhysicsEnabled)
	{
		thePhysicsWorld->update(deltaTime * (PhysicsTimescale*2), 0);

		// every half-second check for resting bodies
		//if (ofGetElapsedTimeMillis() % 30 == 0)
		//{
			CheckBodiesAtRest();
		//}
	}

	
	
}

//--------------------------------------------------------------
void ofApp::draw()
{


	theCamera->begin(); // Begin drawing with this camera.

		// Draw the terrain without using the camera, if it's raymarched.
		if(currentTerrainType == TERRAIN_TYPE::TERRAIN_RAY_DIST)
		{
			theCamera->end();

			theTerrain->Draw();
			
			theCamera->begin();
		}
		else
		{
			theTerrain->Draw();
		}
		

		// Debug: draw the physics mesh over the top
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


		// Enable light shader
		//lightShader->begin();

		// Draw physics objects.
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
		
		// Draw sliced up objects
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

		// stop using lights
		//lightShader->end();

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
	if (button == 2) // Right Mouse
	{
		theCamera->toggleControl();
	}

	if (button == 1) // Middle Mouse
	{
		// Trace a point on the terrain, add a csg sphere
		if (currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
		{
			ofVec3f removePos = (theCamera->getLookAtDir() * 5.0f) + (theCamera->getPosition() + (theCamera->getPosition() - ((TerrainGridMarchingCubes*)theTerrain)->theGrid->getPosition()));
			
			((TerrainGridMarchingCubes*)theTerrain)->CSGRemoveSphere(removePos, 25);
			std::cout << "Removed CSG Sphere, at " << removePos << "." << std::endl;
		}

		if (currentTerrainType == TERRAIN_TYPE::TERRAIN_RAY_DIST)
		{
			ofVec3f removePos = (theCamera->getLookAtDir() * 5.0f) + (theCamera->getPosition());

			((TerrainDistanceRaymarch*)theTerrain)->CSGRemoveSphere(removePos, 25);
			std::cout << "Removed CSG Sphere, at " << removePos << "." << std::endl;
		}
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

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
	auto selectedItem = e.target->getSelected();
	if (selectedItem->getName() == "Grid-Based Naive Marching Cubes" && currentTerrainType != TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		// delete terrain, remake as grid
		delete theTerrain;
		theTerrain = new TerrainGridMarchingCubes();
		((TerrainGridMarchingCubes*)theTerrain)->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);

		currentTerrainType = TERRAIN_TYPE::TERRAIN_GRID_MC;

		buildGUI();
	}
	if (selectedItem->getName() == "Raymarched Distance Field" && currentTerrainType != TERRAIN_TYPE::TERRAIN_RAY_DIST)
	{
		theTerrain = new TerrainDistanceRaymarch();
		((TerrainDistanceRaymarch*)theTerrain)->Rebuild(320, 240);
		((TerrainDistanceRaymarch*)theTerrain)->CurrentCamera = theCamera;

		currentTerrainType = TERRAIN_TYPE::TERRAIN_RAY_DIST;

		buildGUI();
	}
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
	if (e.target->getName() == "Rebuild Terrain" && currentTerrainType == TERRAIN_TYPE::TERRAIN_RAY_DIST)
	{
		((TerrainDistanceRaymarch*)theTerrain)->Rebuild(RayTerrainResolutionX, RayTerrainResolutionY);
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

		//Slice objects with voronoi fracturing, add to list of active sliced objects.
		std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> newObjects = VoronoiFracture(testBox, testBoxMesh->getMeshPtr(), thePhysicsWorld, 16, NULL);
		cutPhysicsObjects.insert(cutPhysicsObjects.end(), newObjects.begin(), newObjects.end());
		e.target->setName("Slice Done");


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
		theGUI->onDropdownEvent(this, &ofApp::onDropdownEvent);
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

	vector<string> terrainOptions = { "Grid-Based Naive Marching Cubes", "Raymarched Distance Field" };
	theGUI->addDropdown("Select Terrain Type",terrainOptions);
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
	else if (currentTerrainType == TERRAIN_TYPE::TERRAIN_RAY_DIST)
	{
		auto terrainResSizeX = terrainFolder->addSlider("Render Resolution X", 32, 1280, 1280);
		terrainResSizeX->setPrecision(0);
		terrainResSizeX->bind(RayTerrainResolutionX);

		auto terrainResSizeY = terrainFolder->addSlider("Render Resolution Y", 24, 720, 720);
		terrainResSizeY->setPrecision(0);
		terrainResSizeY->bind(RayTerrainResolutionY);

		auto terrainStepAmt = terrainFolder->addSlider("Max Steps", 16, 1024, 256);
		terrainStepAmt->setPrecision(0);
		terrainStepAmt->bind(RayTerrainIterations);

		auto terrainDistance = terrainFolder->addSlider("Max Distance", 64, 16000, 1500);
		terrainDistance->setPrecision(2);
		terrainDistance->bind(RayTerrainDrawDistance);

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
	newShape->activate();


	return newShape;
}

void ofApp::CheckBodiesAtRest()
{
	// Loop through list and find physics objects that are considered "at rest".
	for (auto iter = cutPhysicsObjects.begin(); iter != cutPhysicsObjects.end(); ++iter)
	{
		if (iter->second->getActivationState() != OFX_BT_ACTIVATION_STATE_ACTIVE)
		{
			// Object is asleep; convert it to a density object and remove it from the simulation.
			ConvertMeshToDensity(iter->first, ofVec3f(iter->second->getPosition().x, iter->second->getPosition().y, iter->second->getPosition().z));

			// Remove from simulation
			iter->second->remove();

			// Kill mesh
			//delete iter->first;

			// Erase from list.
			iter = cutPhysicsObjects.erase(iter);
			if (iter == cutPhysicsObjects.end())
			{
				break;
			}
		}
	}

	

}

void ofApp::ConvertMeshToDensity(ofMesh* theMesh, ofVec3f position)
{
	// NOTE: Due to time constraints, this function does not use a scan algorithm to recreate the mesh in density-field form properly.
	// Ordinarily this function would render the mesh in its own projection space and use a raytrace-type shader to fill a 3d texture with
	// density information. This would give us a texture that we could pass into the terrain renderers to recreate the shape exactly.
	// However, the project has run into time constraint problems, and so instead the function simply adds a sphere to the terrain
	// where the mesh is located, at a radius consistent with the maximal size of the object.

	// Get Max, Min vertices for sizing
	ofVec3f minVert, maxVert;
	minVert.set(0, 0, 0);
	maxVert.set(0, 0, 0);
	for (auto iter = theMesh->getVertices().begin(); iter != theMesh->getVertices().end(); ++iter)
	{
		if (iter->x < minVert.x && iter->y < minVert.y && iter->z < minVert.z)
		{
			minVert.set(iter->x, iter->y, iter->z);
		}
		if (iter->x > maxVert.x && iter->y > maxVert.y && iter->z > maxVert.z)
		{
			maxVert.set(iter->x, iter->y, iter->z);
		}
	}

	float radius = (maxVert - minVert).length();

	// Create sphere of that radius, at the provided position.
	if (currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		((TerrainGridMarchingCubes*)theTerrain)->CSGAddSphere(position + (position - ((TerrainGridMarchingCubes*)theTerrain)->theGrid->getPosition()), radius);
	}
	else if(currentTerrainType == TERRAIN_TYPE::TERRAIN_RAY_DIST)
	{
		((TerrainDistanceRaymarch*)theTerrain)->CSGAddSphere(position, radius);
	}

}
