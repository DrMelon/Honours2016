#version 150

// Grid-Based Marching Cubes Implementation, Vertex Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: A simple pass-through shader, for grid-based marching cubes. Passes not only the position of the vertices after a screen transform, but also their original world-space positions before camera transforms.

uniform mat4 modelViewProjectionMatrix;
in vec4 position;

out vertexData
{
	vec3 position;
} worldspaceVertex;




void main()
{
	worldspaceVertex.position = position.xyz;
	gl_Position = modelViewProjectionMatrix * position;
}