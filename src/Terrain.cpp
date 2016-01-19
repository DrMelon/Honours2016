#include "Terrain.h"

//Filename: Terrain.cpp
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 19/01/2016
//
//Purpose: This is the code file for the Terrain parent class; this class is simply a skeleton for more specific implementations to plug into, and will be used for creating, modifying, and displaying 
// the terrain. This will allow for a modular approach to representing the terrains.

Terrain::Terrain()
{

}

Terrain::~Terrain()
{

}



void Terrain::Rebuild()
{

}

void Terrain::Update()
{

}


void Terrain::Draw()
{

}

void Terrain::SetOffset(ofVec3f newOffset)
{
	OffsetPosition = newOffset;
}
