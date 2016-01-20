#version 150

// Grid-Based Marching Cubes Implementation, Fragment Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: After the marching cubes algorithm has concluded in the geometry shader, this shader will colour and shade the terrain appropriately.

out vec4 outcolour;

void main()
{
	outcolour = vec4(1.0, 1.0, 1.0, 1.0);
}
