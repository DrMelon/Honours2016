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

	
	// We should loop through all the vertices
	// and for each vertex, mark it as being positive or negative of the plane
	// then take all positive vertices & make contiguous shapes of them
	// and same for all negative
	// by filling the hole that appears?

	// extremely naive. alternative is to use Paul Bourke's method.
	// first use naive method.

	// Create a key-value list of vertices to store which side of the plane the meshes belong on.
	std::multimap<bool, ofVec3f> splitVertices;

	for (int i = 0; i < meshToCut.getNumVertices(); i++)
	{
		// Pick the vertex
		ofVec3f currentVertex = meshToCut.getVertex(i);
		
		// Store it in the map depending on if it's "inside" or "outside" of the plane.
		// Because it's possible for points to be exactly on the plane, we'll store those on the "inside" side.
		bool isInside = (PointPlaneSide(planeNormalVector, currentVertex) >= 0);

		splitVertices.insert(std::make_pair(isInside, currentVertex));
	}

	// Now that we know which side the vertices are on, we can generate the resultant meshes.
	ofMesh* insideMesh;
	ofMesh* outsideMesh;

	insideMesh = new ofMesh();
	insideMesh->setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	//insideMesh->disableIndices();
	insideMesh->setupIndicesAuto();
	
	outsideMesh = new ofMesh();
	outsideMesh->setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	//outsideMesh->disableIndices();
	outsideMesh->setupIndicesAuto();
	// For all meshes inside...
	for (std::multimap<bool, ofVec3f>::iterator iter = splitVertices.find(true); iter != splitVertices.end(); iter++)
	{
		insideMesh->addVertex(iter->second);
		insideMesh->addColor(ofColor::red);
	}

	// And for all meshes outside...
	for (std::multimap<bool, ofVec3f>::iterator iter = splitVertices.find(false); iter != splitVertices.end(); iter++)
	{
		outsideMesh->addVertex(iter->second);
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
float PointPlaneSide(ofVec3f planeNormalVector, ofVec3f testPoint)
{
	return planeNormalVector.dot(testPoint);
}