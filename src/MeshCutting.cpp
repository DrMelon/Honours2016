#include "MeshCutting.h"

//Filename: MeshCutting.cpp
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 20/02/2016
//
//Purpose: This is a set of helpful functions for cutting/fracturing/shattering meshes.

// This function lets us cut an arbitrary mesh using a normal vector-defined plane. 
// It returns a list of resultant meshes.
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
	for (int i = 0; i < meshToCut.getNumIndices() / 3; i++)
	{
		ofMeshFace currentFace = meshToCut.getFace(i);
		
		// Discover which case we're dealing with - no verts inside, all verts inside, two verts "inside" the plane, or only one inside
		bool Vert0Inside = PointPlaneSide(planePoint, planeNormalVector, currentFace.getVertex(0)) >= 0;
		bool Vert1Inside = PointPlaneSide(planePoint, planeNormalVector, currentFace.getVertex(1)) >= 0;
		bool Vert2Inside = PointPlaneSide(planePoint, planeNormalVector, currentFace.getVertex(2)) >= 0;
		
		int VerticesInside = 0;

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
				intersectVert1 = LerpVec3(currentFace.getVertex(0), currentFace.getVertex(1), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(0), currentFace.getVertex(1)));
				intersectVert2 = LerpVec3(currentFace.getVertex(0), currentFace.getVertex(2), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(0), currentFace.getVertex(2)));

				

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
				intersectVert1 = LerpVec3(currentFace.getVertex(1), currentFace.getVertex(2), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(1), currentFace.getVertex(2)));
				intersectVert2 = LerpVec3(currentFace.getVertex(1), currentFace.getVertex(0), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(1), currentFace.getVertex(0)));

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
				intersectVert1 = LerpVec3(currentFace.getVertex(2), currentFace.getVertex(0), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(2), currentFace.getVertex(0)));
				intersectVert2 = LerpVec3(currentFace.getVertex(2), currentFace.getVertex(1), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(2), currentFace.getVertex(1)));

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
				intersectVert1 = LerpVec3(currentFace.getVertex(0), currentFace.getVertex(1), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(0), currentFace.getVertex(1)));
				intersectVert2 = LerpVec3(currentFace.getVertex(0), currentFace.getVertex(2), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(0), currentFace.getVertex(2)));

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
				intersectVert1 = LerpVec3(currentFace.getVertex(1), currentFace.getVertex(2), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(1), currentFace.getVertex(2)));
				intersectVert2 = LerpVec3(currentFace.getVertex(1), currentFace.getVertex(0), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(1), currentFace.getVertex(0)));

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
				intersectVert1 = LerpVec3(currentFace.getVertex(2), currentFace.getVertex(0), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(2), currentFace.getVertex(0)));
				intersectVert2 = LerpVec3(currentFace.getVertex(2), currentFace.getVertex(1), PlaneIntersectLine(planePoint, planeNormalVector, currentFace.getVertex(2), currentFace.getVertex(1)));

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

	
	// Now that the lists of indices and vertices have been finalized, we can build the new meshes.

	insideMesh = new ofMesh();
	insideMesh->setMode(OF_PRIMITIVE_TRIANGLES);
	//insideMesh->disableIndices();
	//insideMesh->setupIndicesAuto();
	
	outsideMesh = new ofMesh();
	outsideMesh->setMode(OF_PRIMITIVE_TRIANGLES);
	//outsideMesh->disableIndices();
	//outsideMesh->setupIndicesAuto();


	// For all the indices in the new mesh...
	for (int i = 0; i < insideIndices.size(); i++)
	{
		insideMesh->addVertex(masterListVertices.at(insideIndices.at(i)));
		insideMesh->addIndex(i);
		insideMesh->addColor(ofColor::magenta);
	}
	for (int i = 0; i < outsideIndices.size(); i++)
	{
		outsideMesh->addVertex(masterListVertices.at(outsideIndices.at(i)));
		outsideMesh->addIndex(i);
		outsideMesh->addColor(ofColor::cyan);
	}


	// If there are at least some vertices in each side, send them out of this function. Otherwise, delete them.
	if (insideMesh->getNumVertices() > 0)
	{
		meshList.push_back(insideMesh);
	}
	else
	{
		delete insideMesh;
	}

	if (outsideMesh->getNumVertices() > 0)
	{
		meshList.push_back(outsideMesh);
	}
	else
	{
		delete outsideMesh;
	}

	

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
std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> SlicePhysicsObject(ofxBulletCustomShape* physicsObject, ofMesh* physicsObjectMesh, ofVec3f planePoint, ofVec3f planeNormalVector, ofxBulletWorldRigid* theWorld, bool deleteOriginal)
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
		
		newShape->addMesh(*(cutMeshes.at(i)), ofVec3f(1, 1, 1), false);
		ofVec3f meshPosition = cutMeshes.at(i)->getCentroid();
		float tmp = meshPosition.x;
		meshPosition.x = meshPosition.z;
		meshPosition.z = tmp;
		ofVec3f newOffset = physObjTranslation + (meshPosition - physObjTranslation);
		// Creating the object requires that we offset by the relative distance between the centroids.
		newShape->create(theWorld->world, newOffset, physObjRotation, 1.0f);
		newShape->add();
		newShape->getRigidBody()->setLinearVelocity(shapeVelocity);

		// Populate List
		outputList.push_back(std::make_pair(cutMeshes.at(i), newShape));

	}



	

	return outputList;
}