#version 150

// Grid-Based Marching Cubes Implementation, Geometry Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: The geometry shader is where the marching cubes algorithm will take place; the density function is defined here, but CSG operands that modify the density function must be passed in from the main application.
// 

layout (points) in;
layout (points) out;

in vertexData
{
	vec3 position;
} vertices[];



void main()
{
	int i;
	for(i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}