#version 410

// Grid-Based Marching Cubes Implementation, Vertex Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: A simple pass-through shader, for grid-based marching cubes. Passes not only the position of the vertices after a screen transform, but also their original world-space positions before camera transforms.
// Additionally, vertices sent to the geometry shader know the scale of the grid, which is encoded in the R channel of the vertex colour.
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec4 color;
in vec4 normal;
in vec4 texcoord; 

uniform float gridscale;
uniform vec3 gridoffset;

out vec3 worldspaceposition;
out float worldspacescale;

void main()
{
	worldspaceposition = position.xyz + gridoffset;
	worldspacescale = gridscale;
	gl_Position = modelViewProjectionMatrix * position;

}