#pragma once
#include "ofMain.h"
#include <vector>
#include <map>


//Filename: MeshCutting.h
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 20/02/2016
//
//Purpose: This is a set of helpful functions for cutting/fracturing/shattering meshes.


// This function lets us cut an arbitrary mesh using a normal vector-defined plane. 
// It returns a list of resultant meshes.
std::vector<ofMesh*> CutMeshWithPlane(ofVec3f planePoint, ofVec3f planeNormalVector, ofMesh meshToCut);

// This function tells us which side of a plane a point is on.
float PointPlaneSide(ofVec3f planePoint, ofVec3f planeNormalVector, ofVec3f testPoint);

// This function tells us how far along a line a plane has intersected it. This will be used to get the intersection points of the mesh.
float PlaneIntersectLine(ofVec3f planePoint, ofVec3f planeNormalVector, ofVec3f lineStart, ofVec3f lineEnd);