#version 330

// Grid-Based Marching Cubes Implementation, Fragment Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: After the marching cubes algorithm has concluded in the geometry shader, this shader will colour and shade the terrain appropriately.

out vec4 outcolour;

in vec3 worldspaceposition_g;

void main()
{
	outcolour = vec4(1.0, worldspaceposition_g.x / 100, 0.0, 1.0);
}
