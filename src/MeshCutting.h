#pragma once
#include "ofMain.h"
#include "ofxBullet.h"
#include "ofxVoro.h"
#include <vector>
#include <map>


//Filename: MeshCutting.h
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 20/02/2016
//
//Purpose: This is a set of helpful functions & classes for cutting/fracturing/shattering meshes.

// This class represents a plane in 3d space; by taking the normal direction of the plane and a point colinear with that plane,
// we can easily cut meshes apart via plane-line intersections.
namespace MeshCutting
{
	class Plane
	{
		public:
			ofVec3f PlanePoint;
			ofVec3f PlaneNormal;
			Plane();
		
	};

	inline Plane::Plane() { PlanePoint = ofVec3f(0, 0, 0); PlaneNormal = ofVec3f(0, 0, 0); };

};


// This function lets us cut an arbitrary mesh using a normal vector-defined plane. 
// It returns a list of resultant meshes.
std::vector<ofMesh*> CutMeshWithPlane(MeshCutting::Plane thePlane, ofMesh meshToCut);
std::vector<ofMesh*> CutMeshWithPlane(ofVec3f planePoint, ofVec3f planeNormalVector, ofMesh meshToCut);

// This function tells us which side of a plane a point is on.
float PointPlaneSide(ofVec3f planePoint, ofVec3f planeNormalVector, ofVec3f testPoint);

// This function tells us how far along a line a plane has intersected it. This will be used to get the intersection points of the mesh.
float PlaneIntersectLine(ofVec3f planePoint, ofVec3f planeNormalVector, ofVec3f lineStart, ofVec3f lineEnd);

// This function allows us to linearly interpolate between two ofVec3f points.
ofVec3f LerpVec3(ofVec3f start, ofVec3f end, float amount);

// This function slices a physics object into two new physics objects & meshes.
std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> SlicePhysicsObject(ofxBulletCustomShape* physicsObject, ofMesh* physicsObjectMesh, ofVec3f planePoint, ofVec3f planeNormalVector, ofxBulletWorldRigid* theWorld, bool deleteOriginal, bool addToWorld);

// This function returns a list of planes from a Voro++ container. Each plane is given an ID that represents which cell it belongs to.
std::vector<std::pair<int, MeshCutting::Plane>> VoronoiPlanesFromContainer(voro::container &theContainer, ofVec3f offset);

// This function fractures a physics object using a 3D Voronoi Diagram.
std::vector<std::pair<ofMesh*, ofxBulletCustomShape*>> VoronoiFracture(ofxBulletCustomShape* physicsObject, ofMesh* physicsObjectMesh, ofxBulletWorldRigid* theWorld, int numCells, ofVec3f* impactPoint);



