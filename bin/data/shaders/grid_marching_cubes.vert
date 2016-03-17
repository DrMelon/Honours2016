#version 330

// Grid-Based Marching Cubes Implementation, Vertex Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: A simple pass-through shader, for grid-based marching cubes. Passes not only the position of the vertices after a screen transform, but also their original world-space positions before camera transforms.
// Additionally, vertices sent to the geometry shader know the scale of the grid, which is encoded in the R channel of the vertex colour.
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform float usingTexture;
uniform float usingColors;
uniform vec4 globalColor;

in vec4 position;
in vec4 color;
in vec4 normal;
in vec4 texcoord; 

uniform float gridscale;
uniform vec3 gridoffset;

out vec4 worldspaceposition;
out float worldspacescale;
out vec3 gridoffset_g;
out vec3 lightdir;
out mat3 norm_mat;

void main()
{
	
	// Lighting
	mat3 norm_mat = transpose(inverse(mat3(modelViewMatrix)));

	mat4 modelMatrix = modelViewMatrix * inverse(viewMatrix);

	lightdir = vec3(normalize((viewMatrix) * vec4(1.0, 1.0, 0.0, 0.0)));


	// Terrain scaling


	worldspaceposition = (position + vec4(gridoffset,1.0));
	worldspacescale = gridscale;

	gridoffset_g = gridoffset;
	gl_Position = position;

}