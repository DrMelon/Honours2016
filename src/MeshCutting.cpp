#include "MeshCutting.h"

//Filename: MeshCutting.cpp
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 20/02/2016
//
//Purpose: This is a set of helpful functions for cutting/fracturing/shattering meshes.

// This function lets us cut an arbitrary mesh using a normal vector-defined plane. 
// It returns a list of resultant meshes.
std::vector<ofMesh*> CutMeshWithPlane(MeshCutting::Plane thePlane, ofMesh meshToCut)
{
	return CutMeshWithPlane(thePlane.PlanePoint, thePlane.PlaneNormal, meshToCut);
}
std::vector<ofMesh*> CutMeshWithPlane(ofVec3f planePoint, ofVec3f planeNormalVector, ofMesh meshToCut)
{
	// This function goes roughly as follows:
	// 1. Find all points of intersection with the plane.
	// 2. Using those points of intersection as new vertices, create new meshes. 
	//	  2.1 To identify where one mesh ends and another begins, we can test to see if the new shape's vertices lie on the positive or negative side of the plane.
	//    2.2 Meshes may have areas that are not necessarily triangles, and so these must be triangulated/tesselated appropriately.
	// 3. Return a list of these resultant meshes.  

	std::vector<ofMesh*> meshList;

	// Create a key-value list of vertices to store which side of the plane the meshes belong on.
	std::multimap<int, ofVec3f> splitVertices;

	// Create an index list to store the new indices, for each half.
	std::vector<int> outsideIndices;
	std::vector<int> insideIndices;

	// Track the last known index / vertex numbers.
	// Useful when we're adding extras.
	int newIndexNumber = meshToCut.getNumIndices();
	int newVertexNumber = meshToCut.getNumVertices();

	// We need to keep a master-list of all the vertices, that we can add more to.
	std::vector<ofPoint> masterListVertices = meshToCut.getVertices();

	// These are used to store the outgoing meshes
	ofMesh* insideMesh;
	ofMesh* outsideMesh;



	// Loop through the faces of the mesh, finding which vertices are on the inside and outside of the mesh.
	// Also, add the vertices where the faces intersect with the plane to both lists.
	int VerticesInside = 0;

	// --------------------------------v try get num vertices?
	for (int i = 0; i < meshToCut.getUniqueFaces().size(); i++)
	{
		ofMeshFace currentFace = meshToCut.getFace(i);

		//TODO: Try mesh.vertex(i) or sth
		ofVec3f vert0 = currentFace.getVertex(0);
		ofVec3f vert1 = currentFace.getVertex(1);
		ofVec3f vert2 = currentFace.getVertex(2);

		// Discover which case we're dealing with - no verts inside, all verts inside, two verts "inside" the plane, or only one inside
		bool Vert0Inside = PointPlaneSide(planePoint, planeNormalVector, vert0) >= 0;
		bool Vert1Inside = PointPlaneSide(planePoint, planeNormalVector, vert1) >= 0;
		bool Vert2Inside = PointPlaneSide(planePoint, planeNormalVector, vert2) >= 0;

		VerticesInside = 0;

		if (Vert0Inside)
		{
			VerticesInside++;
		}
		if (Vert1Inside)
		{
			VerticesInside++;
		}
		if (Vert2Inside)
		{
			VerticesInside++;
		}

		// Indices tracking; we need to know the current index of any given vertex.
		int index0, index1, index2;
		index0 = meshToCut.getIndex((i * 3) + 0);
		index1 = meshToCut.getIndex((i * 3) + 1);
		index2 = meshToCut.getIndex((i * 3) + 2);

		if (VerticesInside == 0)
		{
			// No vertices inside! Add the face's indices to the outside list only.
			outsideIndices.push_back(index0);
			outsideIndices.push_back(index1);
			outsideIndices.push_back(index2);

			

		}
		if (VerticesInside == 3)
		{
			// All vertices inside! Add the face's indices to the inside list only.
			insideIndices.push_back(index0);
			insideIndices.push_back(index1);
			insideIndices.push_back(index2);
		}

		// Now, the only remaining cases are:
		// 1. There's two vertices outside, one inside
		// 2. There's two vertices inside, one outside

		// Only one vertex inside
		if (VerticesInside == 1)
		{
			// If it's the first one in the triangle...
			// So, that's 0. 1 and 2 are outside.
			if (Vert0Inside)
			{
				// Get intersecting points
				ofVec3f intersectVert1, intersectVert2;
				intersectVert1 = LerpVec3(vert0, vert1, PlaneIntersectLine(planePoint, planeNormalVector, vert0, vert1));
				intersectVert2 = LerpVec3(vert0, vert2, PlaneIntersectLine(planePoint, planeNormalVector, vert0, vert2));



				// Add these vertices to the masterlist.
				masterListVertices.push_back(intersectVert1);
				masterListVertices.push_back(intersectVert2);

				int newIndex1, newIndex2;
				newIndex1 = newVertexNumber++;
				newIndex2 = newVertexNumber++;

				// Then, add the indices in the right places
				insideIndices.push_back(index0);
				insideIndices.push_back(newIndex1);
				insideIndices.push_back(newIndex2);

				outsideIndices.push_back(newIndex1);
				outsideIndices.push_back(index1);
				outsideIndices.push_back(index2);

				outsideIndices.push_back(newIndex1);
				outsideIndices.push_back(index2);
				outsideIndices.push_back(newIndex2);
			}
			// Repeat for index 1, and index 2...
			if (Vert1Inside)
			{
				// Get intersecting points
				ofVec3f intersectVert1, intersectVert2;
				intersectVert1 = LerpVec3(vert1, vert0, PlaneIntersectLine(planePoint, planeNormalVector, vert1, vert0));
				intersectVert2 = LerpVec3(vert1, vert2, PlaneIntersectLine(planePoint, planeNormalVector, vert1, vert2));

				// Add these vertices to the masterlist.
				masterListVertices.push_back(intersectVert1);
				masterListVertices.push_back(intersectVert2);

				int newIndex1, newIndex2;
				newIndex1 = newVertexNumber++;
				newIndex2 = newVertexNumber++;

				// Then, add the indices in the right places
				insideIndices.push_back(index1);
				insideIndices.push_back(newIndex2);
				insideIndices.push_back(newIndex1);

				outsideIndices.push_back(newIndex1);
				outsideIndices.push_back(newIndex2);
				outsideIndices.push_back(index2);

				outsideIndices.push_back(newIndex1);
				outsideIndices.push_back(index2);
				outsideIndices.push_back(index0);
			}
			if (Vert2Inside)
			{
				// Get intersecting points
				ofVec3f intersectVert1, intersectVert2;
				intersectVert1 = LerpVec3(vert2, vert0, PlaneIntersectLine(planePoint, planeNormalVector, vert2, vert0));
				intersectVert2 = LerpVec3(vert2, vert1, PlaneIntersectLine(planePoint, planeNormalVector, vert2, vert1));

				// Add these vertices to the masterlist.
				masterListVertices.push_back(intersectVert1);
				masterListVertices.push_back(intersectVert2);

				int newIndex1, newIndex2;
				newIndex1 = newVertexNumber++;
				newIndex2 = newVertexNumber++;

				// Then, add the indices in the right places
				insideIndices.push_back(newIndex1);
				insideIndices.push_back(newIndex2);
				insideIndices.push_back(index2);

				outsideIndices.push_back(index1);
				outsideIndices.push_back(newIndex2);
				outsideIndices.push_back(newIndex1);

				outsideIndices.push_back(index1);
				outsideIndices.push_back(newIndex1);
				outsideIndices.push_back(index0);
			}
		}

		// Two vertices inside -- basically identical to the block above,
		// but with the order of outside/inside swapped!
		if (VerticesInside == 2)
		{
			// If it's the first one in the triangle that's outside...
			// So, that's 0. 1 and 2 are inside.
			if (!Vert0Inside)
			{
				// Get intersecting points
				ofVec3f intersectVert1, intersectVert2;
				intersectVert1 = LerpVec3(vert0, vert1, PlaneIntersectLine(planePoint, planeNormalVector, vert0, vert1));
				intersectVert2 = LerpVec3(vert0, vert2, PlaneIntersectLine(planePoint, planeNormalVector, vert0, vert2));

				// Add these vertices to the masterlist.
				masterListVertices.push_back(intersectVert1);
				masterListVertices.push_back(intersectVert2);

				int newIndex1, newIndex2;
				newIndex1 = newVertexNumber++;
				newIndex2 = newVertexNumber++;

				// Then, add the indices in the right places
				outsideIndices.push_back(index0);
				outsideIndices.push_back(newIndex1);
				outsideIndices.push_back(newIndex2);

				insideIndices.push_back(newIndex1);
				insideIndices.push_back(index1);
				insideIndices.push_back(index2);

				insideIndices.push_back(newIndex1);
				insideIndices.push_back(index2);
				insideIndices.push_back(newIndex2);
			}
			// Repeat for index 1, and index 2...
			if (!Vert1Inside)
			{
				// Get intersecting points
				ofVec3f intersectVert1, intersectVert2;
				intersectVert1 = LerpVec3(vert1, vert0, PlaneIntersectLine(planePoint, planeNormalVector, vert1, vert0));
				intersectVert2 = LerpVec3(vert1, vert2, PlaneIntersectLine(planePoint, planeNormalVector, vert1, vert2));

				// Add these vertices to the masterlist.
				masterListVertices.push_back(intersectVert1);
				masterListVertices.push_back(intersectVert2);

				int newIndex1, newIndex2;
				newIndex1 = newVertexNumber++;
				newIndex2 = newVertexNumber++;

				// Then, add the indices in the right places
				outsideIndices.push_back(index1);
				outsideIndices.push_back(newIndex2);
				outsideIndices.push_back(newIndex1);

				insideIndices.push_back(newIndex1);
				insideIndices.push_back(newIndex2);
				insideIndices.push_back(index2);

				insideIndices.push_back(newIndex1);
				insideIndices.push_back(index2);
				insideIndices.push_back(index0);
			}
			if (!Vert2Inside)
			{
				// Get intersecting points
				ofVec3f intersectVert1, intersectVert2;
				intersectVert1 = LerpVec3(vert2, vert0, PlaneIntersectLine(planePoint, planeNormalVector, vert2, vert0));
				intersectVert2 = LerpVec3(vert2, vert1, PlaneIntersectLine(planePoint, planeNormalVector, vert2, vert1));

				// Add these vertices to the masterlist.
				masterListVertices.push_back(intersectVert1);
				masterListVertices.push_back(intersectVert2);

				int newIndex1, newIndex2;
				newIndex1 = newVertexNumber++;
				newIndex2 = newVertexNumber++;

				// Then, add the indices in the right places
				outsideIndices.push_back(newIndex1);
				outsideIndices.push_back(newIndex2);
				outsideIndices.push_back(index2);

				insideIndices.push_back(index1);
				insideIndices.push_back(newIndex2);
				insideIndices.push_back(newIndex1);

				insideIndices.push_back(index1);
				insideIndices.push_back(newIndex1);
				insideIndices.push_back(index0);
			}
		}


	}

	// Now we need to cap the meshes; that is, we need to look at the vertices that were generated along the intersection points,
	// find an average/center point of them all, and connect that point to all the interpolated points on both meshes to fill the hole left behind from slicing.

	ofVec3f averagePosition(0, 0, 0);

	int firstInterpVert = meshToCut.getNumVertices();
	int lastInterpVert = masterListVertices.size() - 1;
	int numNewVertices = lastInterpVert - firstInterpVert;

	// First we must check that we have enough new verts to fill a hole with.
	if (numNewVertices < 3)
	{
		// Not enough...
	}
	else if (numNewVertices == 3)
	{
		// Just one new triangle on both meshes
		outsideIndices.push_back(newVertexNumber - 2);
		outsideIndices.push_back(newVertexNumber - 1);
		outsideIndices.push_back(newVertexNumber - 0);


		insideIndices.push_back(newVertexNumber - 2);
		insideIndices.push_back(newVertexNumber - 1);
		insideIndices.push_back(newVertexNumber - 0);


	}
	else
	{
		// More triangles - need to create center vertex.


		for (int i = firstInterpVert; i < lastInterpVert; i++)
		{
			// Starting at the first intersected vertex, we build an average.
			averagePosition += masterListVertices.at(i);

		}

		// Average out the position.
		averagePosition /= numNewVertices;

		// Create a vertex at this position.
		masterListVertices.push_back(averagePosition);

		// Keep shoving vertices on if not a power of 3 [hacky]
		while (masterListVertices.size() % 3 != 0)
		{
			masterListVertices.push_back(averagePosition);
		}

		// Get the starting indices
		int startInsideIndex = insideIndices.size();
		int startOutsideIndex = outsideIndices.size();

		// For each of the interpolated vertices, we need to add indices to link this last one to them.

		for (int i = firstInterpVert; i < lastInterpVert; i++)
		{
			insideIndices.push_back(newVertexNumber);
			insideIndices.push_back(i);

			outsideIndices.push_back(newVertexNumber);
			outsideIndices.push_back(i);

			// last verts of triangles have to check to make sure they aren't running over memory
			if (i == lastInterpVert - 1)
			{
				insideIndices.push_back(firstInterpVert);
				outsideIndices.push_back(firstInterpVert);
			}
			else
			{
				insideIndices.push_back(i + 1);
				outsideIndices.push_back(i + 1);
			}



		}
	}




	// Now that the lists of indices and vertices have been finalized, we can build the new meshes.

	insideMesh = new ofMesh();
	insideMesh->setMode(OF_PRIMITIVE_TRIANGLES);
	//insideMesh->disableIndices();
	//insideMesh->setupIndicesAuto();

	outsideMesh = new ofMesh();
	outsideMesh->setMode(OF_PRIMITIVE_TRIANGLES);
	//outsideMesh->disableIndices();
	//outsideMesh->setupIndicesAuto();

	if (insideIndices.size() < 1)
	{
		cout << "Hey! No inner indices?" << endl;
	}
	if (outsideIndices.size() < 1)
	{
		cout << "Hey! No outer indices?" << endl;
	}

	// For all the indices in the new mesh...
	for (int i = 0; i < masterListVertices.size(); i++)
	{
		insideMesh->addVertex(masterListVertices.at(i));
		insideMesh->addColor(ofColor::magenta);
	}
	for (int i = 0; i < insideIndices.size(); i++)
	{
		insideMesh->addIndex(insideIndices.at(i));

	}
	for (int i = 0; i < masterListVertices.size(); i++)
	{
		outsideMesh->addVertex(masterListVertices.at(i));
		outsideMesh->addColor(ofColor::cyan);
	}
	for (int i = 0; i < outsideIndices.size(); i++)
	{
		outsideMesh->addIndex(outsideIndices.at(i));

	}


	// If there are at least some vertices in each side, send them out of this function. Otherwise, delete them.

		meshList.push_back(insideMesh);
		meshList.push_back(outsideMesh);




	return meshList;
}

// This function tells us which side of a plane a point is on.
float PointPlaneSide(ofVec3f planePoint, ofVec3f planeNormalVector, ofVec3f testPoint)
{
	return planeNormalVector.dot(testPoint - planePoint);
}

float PlaneIntersectLine(ofVec3f planePoint, ofVec3f planeNormalVector, ofVec3f lineStart, ofVec3f lineEnd)
{
	ofVec3f difference = (planePoint - lineStart);// .getNormalized();
	ofVec3f lineDirection = (lineEnd - lineStart);// .getNormalized();
	float result = ((planeNormalVector.dot(difference)) / (planeNormalVector.dot(lineDirection)));
	return result;
}

ofVec3f LerpVec3(ofVec3f start, ofVec3f end, float amount)
{
	ofVec3f output;

	output = start + amount * (end - start);

	return output;
}

// This function slices a physics object into two new physics objects & meshes.
std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> SlicePhysicsObject(ofxBulletCustomShape* physicsObject, ofMesh* physicsObjectMesh, ofVec3f planePoint, ofVec3f planeNormalVector, ofxBulletWorldRigid* theWorld, bool deleteOriginal, bool addToWorld)
{
	// Create our output
	std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> outputList;


	// Prepare to slice mesh - transform it to the location of the physics object.
	ofMesh sliceMesh;
	sliceMesh = *physicsObjectMesh;
	for (int v = 0; v < sliceMesh.getNumVertices(); v++)
	{
		ofVec3f currentVertex = sliceMesh.getVertex(v);

		//Transform each vertex by the physics object's transformation matrix		
		ofMatrix4x4 newTransform = physicsObject->getTransformationMatrix();

		currentVertex = newTransform.transform3x3(currentVertex, newTransform);
		currentVertex += physicsObject->getPosition();

		sliceMesh.setVertex(v, currentVertex);
	}

	// Slice the mesh.
	std::vector<ofMesh*> cutMeshes = CutMeshWithPlane(planePoint, planeNormalVector, sliceMesh);

	// Store translation & rotation of physics object
	ofVec3f physObjTranslation = physicsObject->getPosition();
	ofQuaternion physObjRotation = physicsObject->getRotationQuat();


	btVector3 shapeVelocity = physicsObject->getRigidBody()->getLinearVelocity();

	// Delete original object, if desired
	if (deleteOriginal)
	{
		physicsObject->remove();
		//delete physicsObject;
		//physicsObject = 0;
	}

	// Create physics objects from sliced meshes.
	for (int i = 0; i < cutMeshes.size(); i++)
	{
		ofxBulletCustomShape* newShape = new ofxBulletCustomShape();
		if (addToWorld)
		{
			// Don't try to add empty meshes
			if (cutMeshes.at(i)->getVertices().size() <= 0)
			{
				continue;
			}
			if (cutMeshes.at(i)->getIndices().size() <= 0)
			{
				//cutMeshes.at(i)->setupIndicesAuto();
				continue;
			}
			if (!std::isfinite(cutMeshes.at(i)->getVertex(0).x))
			{
				continue;
			}
			if (!std::isnormal(cutMeshes.at(i)->getVertex(0).x))
			{
				continue;
			}

			// Offset the position of the new physics meshes so that they are separated along the cutting plane.
			newShape->addMesh(*(cutMeshes.at(i)), ofVec3f(1, 1, 1), false);
			ofVec3f meshPosition = cutMeshes.at(i)->getCentroid();
			float tmp = meshPosition.x;
			meshPosition.x = meshPosition.z;
			meshPosition.z = tmp;
			ofVec3f newOffset = physObjTranslation + (meshPosition - physObjTranslation);
			tmp = newOffset.x;
			newOffset.x = newOffset.z;
			newOffset.z = tmp;
			// Creating the object requires that we offset by the relative distance between the centroids.
			newShape->create(theWorld->world, newOffset, 1.0f);

			newShape->add();
			newShape->getRigidBody()->setLinearVelocity(shapeVelocity);
		}


		// Populate List
		outputList.push_back(std::make_pair(cutMeshes.at(i), newShape));

	}





	return outputList;
}


// This function returns a list of planes from a voro++ container.  Each plane is given an ID that represents which cell it belongs to.
std::vector<std::pair<int, MeshCutting::Plane>> VoronoiPlanesFromContainer(voro::container &theContainer, ofVec3f offset)
{
	std::vector<std::pair<int, MeshCutting::Plane>> outPlanes;

	// Create a voro++ iterator.
	voro::c_loop_all VoroIterator(theContainer);
	
	// Track the current cell
	int currentCell = 0;

	// For each cell in the iterator, we extract the faces, giving them an index that correlates to the cell that created them
	if (VoroIterator.start())
	{
		do
		{
			voro::voronoicell theCell;

			// Abort if we can't compute the current cell.
			if (!theContainer.compute_cell(theCell, VoroIterator))
			{
				cout << "Voronoi cell " << currentCell << " failed to compute." << endl;
				break;
			}
			else
			{
				// Get number of faces in this cell.
				int NumFaces = theCell.number_of_faces();

				// Store face orders, normals, indices & vertices
				std::vector<int> faceOrders;
				std::vector<int> faceIndices;
				std::vector<double> faceNormals;
				std::vector<double> cellVertices;
				theCell.face_orders(faceOrders);
				theCell.face_vertices(faceIndices);
				theCell.vertices(cellVertices);
				theCell.normals(faceNormals);

				// For each face/plane in this cell, get that face's centroid & normal, pack into a MeshCutting::Plane, and add to the list.
				for (int face = 0; face < NumFaces; face++)
				{
					// Get current face order - this is the number of vertices in this face.
					int CurrentFaceOrder = faceOrders.at(face);

					// Store planepoint of face
					ofVec3f FaceCentroid;

					// Any point belonging to the face will be coplanar with its plane, so we just grab the first one.
					int faceIndex = faceIndices.at(face);
					faceIndex *= 3;
					double VertexX, VertexY, VertexZ;
					VertexX = cellVertices.at(faceIndex);
					VertexY = cellVertices.at(faceIndex + 1);
					VertexZ = cellVertices.at(faceIndex + 2);

					double CenterX, CenterY, CenterZ;
					theCell.centroid(CenterX, CenterY, CenterZ);

					FaceCentroid = ofVec3f(VertexX, VertexY, VertexZ);


					// Get the normal of this face.
					double NormalX, NormalY, NormalZ;
					NormalX = faceNormals.at((face * 3) + 0);
					NormalY = faceNormals.at((face * 3) + 1);
					NormalZ = faceNormals.at((face * 3) + 2);

					// Make sure normals face away from center
					if (NormalX > 0) // pointing right, so the plane's edge must be to the right of the center of the shape.
					{
						if (VertexX < CenterX)
						{
							// Normal X inverted.
							NormalX *= -1;
						}
					}
					if (NormalX < 0) // pointing left, so the plane's edge must be to the left of the center of the shape.
					{
						if (VertexX > CenterX)
						{
							// Normal X inverted.
							NormalX *= -1;
						}
					}

					if (NormalY > 0)
					{
						if (VertexY < CenterY)
						{
							// Normal Y inverted.
							NormalY *= -1;
						}
					}
					if (NormalY < 0) 
					{
						if (VertexY > CenterY)
						{
							// Normal Y inverted.
							NormalY *= -1;
						}
					}

					if (NormalZ > 0) 
					{
						if (VertexZ < CenterZ)
						{
							// Normal Z inverted.
							NormalZ *= -1;
						}
					}
					if (NormalZ < 0) 
					{
						if (VertexZ > CenterZ)
						{
							// Normal Z inverted.
							NormalZ *= -1;
						}
					}

					ofVec3f FaceNormal = ofVec3f(-NormalX, -NormalY, -NormalZ);

					// Store in plane, push back
					
					MeshCutting::Plane thePlane;
					thePlane.PlaneNormal = FaceNormal;
					thePlane.PlanePoint = FaceCentroid;

					outPlanes.push_back(std::make_pair(currentCell, thePlane));
						

				}

			}


			currentCell++;
		} while (VoroIterator.inc());
	}



	return outPlanes;
}


// This function fractures a physics object using a 3D Voronoi Diagram.
std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> VoronoiFracture(ofxBulletCustomShape* physicsObject, ofMesh* physicsObjectMesh, ofxBulletWorldRigid* theWorld, int numCells, ofVec3f* impactPoint = NULL)
{

	// First creating the container for the voronoi diagram.
	// This is essentially the same as the physics object's bounding box.
	btVector3 boundsMin, boundsMax;
	btTransform objectTransform;
	objectTransform.setIdentity();

	// First storing transform
	//physicsObject->getRigidBody()->getMotionState()->getWorldTransform(objectTransform);
	// Then fetching AABB from collision mesh.
	physicsObject->getCollisionShape()->getAabb(objectTransform, boundsMin, boundsMax);

	// Expand the boundaries a little
	boundsMin.setValue(boundsMin.getX() - 1, boundsMin.getY() - 1, boundsMin.getZ() - 1);
	boundsMax.setValue(boundsMax.getX() + 1, boundsMax.getY() + 1, boundsMax.getZ() + 1);

	// Now, create the container.
	// The container is not smoothed; we want simple plane divisions between the fragments.
	voro::container voroContainer = voro::container(boundsMin.getX(), boundsMax.getX(), boundsMin.getY(), boundsMax.getY(), boundsMin.getZ(), boundsMax.getZ(), 1, 1, 1, false, false, false, 8);

	// We must next seed the container with points; this is done either randomly, or via inverse-square based on an impact point.
	if (impactPoint != NULL)
	{
		// Impact-point seeding
	}
	else
	{
		// Randomized seeding
		for (int cell = 0; cell < numCells; cell++)
		{
			addCellSeed(voroContainer, new ofPoint(ofRandom(boundsMin.getX(), boundsMax.getX()), ofRandom(boundsMin.getY(), boundsMax.getY()), ofRandom(boundsMin.getZ(), boundsMax.getZ())), cell, true);
		}
	}

	// TODO:
	// Check for too many cell faces or for liminal cuts from cells, massive amounts of verts should not happen


	// Now that the diagram is seeded, we can fetch all the cells' meshes.
	std::vector<ofVboMesh> cellMeshes;
	getCellsFromContainer(voroContainer, cellMeshes, false);

	std::vector<std::pair<int, MeshCutting::Plane>> ContainerPlanes = VoronoiPlanesFromContainer(voroContainer, physicsObject->getPosition());

	// Note: we can't just use these meshes as the fracture result - this is because we won't always be dealing with a cubic mesh aligned perfectly with the AABB.
	// Instead, we loop through the faces of each cell-mesh and cut the physics object mesh repeatedly by each face; each result cut by the cell-mesh will then be stored for output.
	// We're basically going to iterate over the mesh and keep dumping the outside mesh.

	// Set up output container.
	std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> outputShapes;

	// For each voronoi cell, we'll be creating an output mesh.
	for (int thisCell = 0; thisCell < numCells; thisCell++)
	{
		// Create the output mesh required, based on original physics mesh
		ofMesh* cellOutputMesh = new ofMesh(*physicsObjectMesh);

		// Get the planes for this cell
		std::vector<MeshCutting::Plane> CellPlanes;
		for (int currentPlane = 0; currentPlane < ContainerPlanes.size(); currentPlane++)
		{
			if (ContainerPlanes.at(currentPlane).first == thisCell)
			{
				CellPlanes.push_back(ContainerPlanes.at(currentPlane).second);
			}
		}

		// For each face (plane) in this cell, we'll slice off another part of the output mesh.
		for (int face = 0; face < CellPlanes.size(); face++)
		{

	


			// Store the results of the slice.
			std::vector<ofMesh*> sliced;

			// Slice the mesh with the current plane.
			//cout << "Cutting face number " << face << " of cell " << thisCell << ". \nPos: " << CellPlanes.at(face).PlanePoint << "\nNorm: " << CellPlanes.at(face).PlaneNormal << endl;
			sliced = CutMeshWithPlane(CellPlanes.at(face), *cellOutputMesh);

			// Store only the "inside" mesh of this slice, discarding the other.
			if (sliced.size() > 0)
			{
				if (sliced.size() > 1)
				{
					delete sliced.at(1);
				}

				// Iterating over this mesh, so we store only the "inside" mesh
				cellOutputMesh = sliced.at(0);
				//cellOutputMesh->mergeDuplicateVertices(); // simplify mesh


			}

		}



		// Now we add the output mesh to the list of output meshes
		// We also need to create a physics mesh for it too.
		ofxBulletCustomShape* newShape = new ofxBulletCustomShape();

		// When we generate the physics mesh, we use convex hull (delauney triangulation) built into bullet.
		// This prevents physics meshes with large numbers of vertices from being created by the repeated slicing.
		//cellOutputMesh->mergeDuplicateVertices(); // simplify mesh
		
		newShape->addMesh(*cellOutputMesh, ofVec3f(1, 1, 1), true);
		ofVec3f meshPosition = cellOutputMesh->getCentroid();
		ofVec3f distancer = (meshPosition);
		ofVec3f newOffset = physicsObject->getPosition() + distancer;
		newShape->create(theWorld->world, newOffset, 1.0f);
		newShape->add();

		
		

		outputShapes.push_back(std::make_pair(cellOutputMesh, newShape));
		cout << "Cell Verts: " << cellOutputMesh->getNumVertices() << endl;

	}

	// We can now remove the original object, if desired.




	return outputShapes;
}