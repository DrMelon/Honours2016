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
	// Instrumentation
	updateStopwatch.filename = "main_update.log";
	drawStopwatch.filename = "draw_update.log";
	updateStopwatch.StartTiming();
	
	// Set maximum framerate.
	ofSetFrameRate(60);
	ofSetBackgroundColor(ofColor(182, 227, 242));
	ofSetVerticalSync(true);
	ofDisableArbTex();

	HelpToggle = true;

	// Set up z-buffer.
	ofEnableDepthTest();

	// Set up GUI
	theGUI = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	theGUI->setTheme(new ofxDatGuiThemeCharcoal());
	
	// Disable the GUI's own automatic rendering, as we're doing shader passes and things.
	theGUI->setAutoDraw(false);

	// Create the camera, using OpenFrameworks' First Person Camera class. This gives us a simple control system.
	theCamera = new ofxFirstPersonCamera();
	theCamera->setNearClip(0.01f);
	theCamera->setFarClip(1500.f);
	theCamera->setFov(59);

	// Create help image
	helpImage = new ofImage("help.png");
	

	// Create lighting shader
	lightShader = new ofShader();
	lightShader->load("data/shaders/directional_light.vert", "data/shaders/directional_light.frag");
	
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

	// Create "dummy" element in the csgOperations buffer. This is necessary for it to work properly as a texture buffer for the terrains.
	CSGAddSphere(ofVec3f(0,0,0), 0);

	// Set up analytics
	gnpUpdatePerformance.Column1Name = "Frame No.";
	gnpUpdatePerformance.Column2Name = "Frame-Time (ms)";
	gnpUpdatePerformance.XAxisName = "Frame No.";
	gnpUpdatePerformance.YAxisName = "Frame-Time (ms)";
	gnpUpdatePerformance.DataColumns = 2;
	gnpUpdatePerformance.HexColour = "aa0000";
	gnpUpdatePerformance.LineThickness = 1;
	gnpUpdatePerformance.DotSize = 1;
	gnpUpdatePerformance.DotType = 7;
	gnpUpdatePerformance.GraphStyle = 1;

	gnpDrawPerformance.Column1Name = "Frame No.";
	gnpDrawPerformance.Column2Name = "Frame-Time (ms)";
	gnpDrawPerformance.XAxisName = "Frame No.";
	gnpDrawPerformance.YAxisName = "Frame-Time (ms)";
	gnpDrawPerformance.DataColumns = 2;
	gnpDrawPerformance.HexColour = "0000aa";
	gnpDrawPerformance.LineThickness = 1;
	gnpDrawPerformance.DotSize = 1;
	gnpDrawPerformance.DotType = 7;
	gnpDrawPerformance.GraphStyle = 1;

	gnpLastFrameTime.Column1Name = "Frame No.";
	gnpLastFrameTime.Column2Name = "Frame-Time (ms)";
	gnpLastFrameTime.XAxisName = "Frame No.";
	gnpLastFrameTime.YAxisName = "Frame-Time (ms)";
	gnpLastFrameTime.DataColumns = 2;
	gnpLastFrameTime.HexColour = "00aa00";
	gnpLastFrameTime.LineThickness = 1;
	gnpLastFrameTime.DotSize = 1;
	gnpLastFrameTime.DotType = 7;
	gnpLastFrameTime.GraphStyle = 1;

	// Create mesh template for physics boxes.	
	testBoxMesh = new ofBoxPrimitive(10, 10, 10, 1, 1, 1);

	// Create a blank mesh for the physics terrain
	ofMesh singleTriangle;
	singleTriangle.addVertex(ofVec3f(0, 0, 0));
	singleTriangle.addVertex(ofVec3f(1, 0, 0));
	singleTriangle.addVertex(ofVec3f(1, 1, 0));
	singleTriangle.addIndex(0);
	singleTriangle.addIndex(1);
	singleTriangle.addIndex(2);
	thePhysicsMesh = CreatePhysicsMesh(thePhysicsWorld, &singleTriangle);

	((TerrainGridMarchingCubes*)theTerrain)->updatePhysicsMesh = true;
	
	// Assign CSG operations buffer
	theTerrain->csgOperations = csgOperations;

	// Add elements to GUI.
	buildGUI();

	// Stop timing
	updateStopwatch.StopTiming("Startup Complete.");
	
}

//--------------------------------------------------------------
void ofApp::update()
{
	updateStopwatch.StartTiming();
	float deltaTime = ofGetLastFrameTime();
	
	// Update GUI
	if (GuiNeedsRebuilt)
	{
		buildGUI();
	}

	auto frametimeGUI = theGUI->getTextInput("Frame-Time", "Diagnostics");
	frametimeGUI->setText(std::to_string(deltaTime) + " s");
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
		thePhysicsWorld->update(0.016f * (PhysicsTimescale*2), 0);
		// every half-second check for resting bodies
		if (ofGetElapsedTimeMillis() % 60 == 0)
		{
			CheckBodiesAtRest();
		}
	}

	gnpUpdatePerformance.Column1.push_back(gnpUpdatePerformance.Column1.size());
	gnpUpdatePerformance.Column2.push_back(updateStopwatch.StopTiming("Update Completed."));

	gnpLastFrameTime.Column1.push_back(gnpLastFrameTime.Column1.size());
	gnpLastFrameTime.Column2.push_back(ofGetLastFrameTime() * 1000.0f);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	drawStopwatch.StartTiming();

	theCamera->begin(); // Begin drawing with this camera.

		// Draw the terrain without using the camera, if it's raymarched.
		if(currentTerrainType == TERRAIN_TYPE::TERRAIN_RAY_DIST)
		{
			theCamera->end();
			ofSetColor(ofColor::white);
			theTerrain->Draw();
			
			theCamera->begin();
		}
		else
		{
			if (currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
			{
				if (((TerrainGridMarchingCubes*)(theTerrain))->updatePhysicsMesh)
				{
					// Raise GNUPlot event
					GNUPlotEvent newEvent;
					newEvent.xPosition = gnpUpdatePerformance.Column1.size() + 1;
					newEvent.xRange = 3;
					newEvent.boxColour = "ffcccc";
					newEvent.labelName = "Phys";
					gnpUpdatePerformance.Events.push_back(newEvent);
					gnpDrawPerformance.Events.push_back(newEvent);
					gnpLastFrameTime.Events.push_back(newEvent);
				}
			}
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
		lightShader->begin();

		// Draw physics objects.

		
		// Draw sliced up objects
		for (int i = 0; i < cutPhysicsObjects.size(); i++)
		{
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
		lightShader->end();

		// Draw a transparent sphere where the user will carve the terrain, toggled with the Alt key
		if (PreviewToggle)
		{
			ofSetColor(ofColor(255, 60, 60, 40));
			glEnable(GL_BLEND);
			ofDrawSphere((theCamera->getPosition() + (theCamera->getLookAtDir() * CarveDistance)), 25.0f);
			glDisable(GL_BLEND);
		}

	theCamera->end(); // Cease drawing with the camera.


	// Draw GUI
	ofDisableDepthTest();
	theGUI->draw();

	if (HelpToggle)
	{
		ofSetColor(ofColor(255, 255, 255, 220));
		helpImage->draw(25, 720 - 200);
	}
	
	ofEnableDepthTest();

	// Check to see if terrain rebuilt our physics during its draw phase.
	if (physicsNeedsRebuilding && currentTerrainType == TERRAIN_TYPE::TERRAIN_GRID_MC)
	{
		if (((TerrainGridMarchingCubes*)theTerrain)->updatePhysicsMesh == false)
		{
			physicsNeedsRebuilding = false;
		}

	}

	

	if (theCamera->getPosition() != camDelta && (theCamera->getPosition() - camDelta).length() > 40.0f)
	{
		camDelta = theCamera->getPosition();

		if ((currentTerrainType == TERRAIN_TYPE::TERRAIN_RAY_DIST))
		{
			// Quickly create and render a grid terrain.
			TerrainGridMarchingCubes* newTerrain = new TerrainGridMarchingCubes();
			newTerrain->updatePhysicsMesh = true;
			newTerrain->PhysicsOnly = true;
			newTerrain->Rebuild(GridTerrainResolution, GridTerrainResolution, GridTerrainResolution, GridTerrainSize);
			newTerrain->thePhysicsWorld = thePhysicsWorld;
			newTerrain->thePhysicsMesh = thePhysicsMesh;
			newTerrain->expensiveNormals = GridExpensiveNormals;
			newTerrain->SetOffset(theCamera->getPosition());
			newTerrain->Update();
			newTerrain->csgOperations = csgOperations;
			newTerrain->Draw();

			delete newTerrain;

			physicsNeedsRebuilding = false;

			// Raise GNUPlot event
			GNUPlotEvent newEvent;
			newEvent.xPosition = gnpUpdatePerformance.Column1.size() + 1;
			newEvent.xRange = 3;
			newEvent.boxColour = "ffcccc";
			newEvent.labelName = "Phys";
			gnpUpdatePerformance.Events.push_back(newEvent);
			gnpDrawPerformance.Events.push_back(newEvent);
			gnpLastFrameTime.Events.push_back(newEvent);

		}
	}

	// Update analytics
	gnpDrawPerformance.Column1.push_back(gnpDrawPerformance.Column1.size());
	gnpDrawPerformance.Column2.push_back(drawStopwatch.StopTiming("Draw Completed."));

}

//--------------------------------------------------------------
void ofApp::keyPressed(ofKeyEventArgs &keyargs)
{
	int key = keyargs.key;
	int keycode = keyargs.keycode;
	if (key == OF_KEY_BACKSPACE)
	{
		//undo csg
		CSGUndo();
	}

	if (key == OF_KEY_SHIFT)
	{
		ShiftHeld = true;
	}

	if (key == OF_KEY_CONTROL)
	{
		CtrlHeld = true;
	}

	if (key == OF_KEY_ALT)
	{
		PreviewToggle = !PreviewToggle;
	}

	if (keycode == GLFW_KEY_H)
	{
		HelpToggle = !HelpToggle;
	}
	

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	if (key == OF_KEY_SHIFT)
	{
		ShiftHeld = false;
	}

	if (key == OF_KEY_CONTROL)
	{
		CtrlHeld = false;
	}


}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

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
		// Carve a segment of terrain out of the world.
		if (!ShiftHeld && !CtrlHeld)
		{
			ofVec3f removePos = (theCamera->getLookAtDir() * CarveDistance) + (theCamera->getPosition());

			CSGRemoveSphere(removePos, 25);
			std::cout << "Removed CSG Sphere, at " << removePos << "." << std::endl;

			physicsNeedsRebuilding = true;

			// Raise GNUPlot event
			GNUPlotEvent newEvent;
			newEvent.xPosition = gnpUpdatePerformance.Column1.size();
			newEvent.xRange = 10;
			newEvent.boxColour = "ffffcc";
			newEvent.labelName = "Carve (No Object)";
			gnpUpdatePerformance.Events.push_back(newEvent);
			gnpDrawPerformance.Events.push_back(newEvent);
			gnpLastFrameTime.Events.push_back(newEvent);
		}

		// Carve a segment of terrain out of the world, with residual sphere slice.
		if (ShiftHeld)
		{
			ofVec3f removePos = (theCamera->getLookAtDir() * CarveDistance) + (theCamera->getPosition());

			physicsNeedsRebuilding = true;

			// Create Physics Sphere Object
			ofxBulletCustomShape newSphereShape;
			newSphereShape.create(thePhysicsWorld->getWorld(), removePos, 1.0f);
			

			ofSpherePrimitive newSphere;
			newSphere.setRadius(12.5f);

			newSphereShape.addMesh(newSphere.getMesh(), ofVec3f(1, 1, 1), true);
			newSphereShape.add();
			

			// Slice up that object
			std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> newObjects = VoronoiFracture(&newSphereShape, newSphere.getMeshPtr(), thePhysicsWorld, 16, NULL);
			cutPhysicsObjects.insert(cutPhysicsObjects.end(), newObjects.begin(), newObjects.end());

			CSGRemoveSphere(removePos, 25);
			std::cout << "Removed CSG Sphere w/ Object, at " << removePos << "." << std::endl;

			

			// Raise GNUPlot event
			GNUPlotEvent newEvent;
			newEvent.xPosition = gnpUpdatePerformance.Column1.size();
			newEvent.xRange = 10;
			newEvent.boxColour = "ffeecc";
			newEvent.labelName = "Carve (Object)";
			gnpUpdatePerformance.Events.push_back(newEvent);
			gnpDrawPerformance.Events.push_back(newEvent);
			gnpLastFrameTime.Events.push_back(newEvent);
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

void ofApp::mouseScrolled(int x, int y, float scrollx, float scrolly)
{
	CarveDistance += scrolly*2;
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

		theTerrain->csgOperations = csgOperations;

		GuiNeedsRebuilt = true;

		// Raise GNUPlot event
		GNUPlotEvent newEvent;
		newEvent.xPosition = gnpUpdatePerformance.Column1.size();
		newEvent.xRange = 10;
		newEvent.boxColour = "ffccff";
		newEvent.labelName = "Switched to Grid";
		gnpUpdatePerformance.Events.push_back(newEvent);
		gnpDrawPerformance.Events.push_back(newEvent);
		gnpLastFrameTime.Events.push_back(newEvent);

	}
	if (selectedItem->getName() == "Raymarched Distance Field" && currentTerrainType != TERRAIN_TYPE::TERRAIN_RAY_DIST)
	{
		theTerrain = new TerrainDistanceRaymarch();
		((TerrainDistanceRaymarch*)theTerrain)->Rebuild(RayTerrainResolutionX, RayTerrainResolutionY);
		((TerrainDistanceRaymarch*)theTerrain)->CurrentCamera = theCamera;

		currentTerrainType = TERRAIN_TYPE::TERRAIN_RAY_DIST;

		theTerrain->csgOperations = csgOperations;

		GuiNeedsRebuilt = true;

		// Raise GNUPlot event
		GNUPlotEvent newEvent;
		newEvent.xPosition = gnpUpdatePerformance.Column1.size();
		newEvent.xRange = 10;
		newEvent.boxColour = "ccffff";
		newEvent.labelName = "Switched to Raycast";
		gnpUpdatePerformance.Events.push_back(newEvent);
		gnpDrawPerformance.Events.push_back(newEvent);
		gnpLastFrameTime.Events.push_back(newEvent);

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
	if (e.target->getName() == "Clear Logs")
	{
		updateStopwatch.ClearLogs();
		drawStopwatch.ClearLogs();
		

	}
	if (e.target->getName() == "Output Logs")
	{
		GNUPlotDataManager plotMan;
		plotMan.WriteGraphDataFile(gnpUpdatePerformance, "update_performance.dat");
		plotMan.WriteGraphDataFile(gnpDrawPerformance, "draw_performance.dat");
		plotMan.WriteGraphDataFile(gnpLastFrameTime, "lastft.dat");
	}

}

// Build GUI
void ofApp::buildGUI()
{
	Stopwatch newWatch("guitime.log");
	newWatch.StartTiming();
	GuiNeedsRebuilt = false;
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
	
	auto clearButton = theGUI->addButton("Clear Logs");

	auto logButton = theGUI->addButton("Output Logs");
	

	auto footerGUI = theGUI->addFooter();
	footerGUI->setLabelWhenCollapsed(":: SHOW TOOLS ::");
	footerGUI->setLabelWhenExpanded(":: HIDE TOOLS ::");

	newWatch.StopTiming("GUI Updated.");
}

ofxBulletTriMeshShape* ofApp::CreatePhysicsMesh(ofxBulletWorldRigid* world, ofMesh* theMesh)
{
	Stopwatch newWatch("physicsmesh.log");
	newWatch.StartTiming();
	ofxBulletTriMeshShape* newShape = new ofxBulletTriMeshShape();
	newShape->create(world->world, *theMesh, ofVec3f(0, 0, 0), 1.0f);
	newShape->add();
	newShape->enableKinematic();
	newShape->activate();

	newWatch.StopTiming("CreatePhysicsMesh Called.");
	return newShape;
}

void ofApp::CheckBodiesAtRest()
{
	// Loop through list and find physics objects that are considered "at rest".
	for (auto iter = cutPhysicsObjects.begin(); iter != cutPhysicsObjects.end(); ++iter)
	{



		if (iter->second->getRigidBody()->getLinearVelocity().length2() < 0.8 && iter->second->getRigidBody()->getAngularVelocity().length2() < 1.0)
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

	float radius = (maxVert - minVert).length() / 2.0f;

	CSGAddSphere(position, radius);

	// Raise GNUPlot event
	GNUPlotEvent newEvent;
	newEvent.xPosition = gnpUpdatePerformance.Column1.size();
	newEvent.xRange = 5;
	newEvent.boxColour = "ccffcc";
	newEvent.labelName = "Mesh->Density";
	gnpUpdatePerformance.Events.push_back(newEvent);
	gnpDrawPerformance.Events.push_back(newEvent);
	gnpLastFrameTime.Events.push_back(newEvent);
}

// CSG Operations work by filling a texture buffer.
// The texture is 8 elements wide: 
// First element: Add/Subtract operation, 0 or 1
// Second: Shape to be defined. 0: Sphere, 1: Box,
// For spheres, the next 4 elements define the position & radius of the sphere.
// The remaining two are left blank

void ofApp::CSGAddSphere(ofVec3f Position, float Radius)
{
	csgOperations.push_back(0);
	csgOperations.push_back(0);
	csgOperations.push_back(Position.x);
	csgOperations.push_back(Position.y);
	csgOperations.push_back(Position.z);
	csgOperations.push_back(Radius);
	csgOperations.push_back(0);
	csgOperations.push_back(0);


	// keep terrain parity
	if (theTerrain)
	{
		theTerrain->csgOperations = csgOperations;
	}
}

void ofApp::CSGRemoveSphere(ofVec3f Position, float Radius)
{
	csgOperations.push_back(1);
	csgOperations.push_back(0);
	csgOperations.push_back(Position.x);
	csgOperations.push_back(Position.y);
	csgOperations.push_back(Position.z);
	csgOperations.push_back(Radius);
	csgOperations.push_back(0);
	csgOperations.push_back(0);

	// keep terrain parity
	if (theTerrain)
	{
		theTerrain->csgOperations = csgOperations;
	}
}

void ofApp::CSGUndo()
{
	// We want the first "dummy" element to remain, always.
	if (csgOperations.size() > 1)
	{
		csgOperations.pop_back();
	}

	// keep terrain parity
	if (theTerrain)
	{
		theTerrain->csgOperations = csgOperations;
	}

}

void ofApp::exit()
{
	// Last-minute logging write-outs.
	GNUPlotDataManager plotMan;
	plotMan.WriteGraphDataFile(gnpUpdatePerformance, "update_performance.dat");
	plotMan.WriteGraphDataFile(gnpDrawPerformance, "draw_performance.dat");
	plotMan.WriteGraphDataFile(gnpLastFrameTime, "lastft.dat");
	

}