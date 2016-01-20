#version 330

// Grid-Based Marching Cubes Implementation, Geometry Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: The geometry shader is where the marching cubes algorithm will take place; the density function is defined here, but CSG operands that modify the density function must be passed in from the main application.
// The implementation is based on Paul Bourke's "Polygonising a Scalar Field" with appropriate adjustments made for this specific implementation.

layout (points) in;
layout (points, max_vertices=8) out;


// We need to include the modelView and projection matrices so that we can transform the terrain's vertices based on the camera, and send them to the fragment shader.
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

in gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];

}gl_in[];

in vec4 worldspaceposition[];
in float worldspacescale[];

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

out vec3 worldspaceposition_g;


// Because the Marching Cube Algorithm requires we look at the 8 vertices of a cube, upon which the density functions are tested, we need to extrapolate 8 vertices from the single point.
// This implemention will do this by sampling the position near the worldspace of the input vertex, at a distance defined by the scale of the grid.

// The ExtrapolateVertex function simply takes an integer input, and returns a position based on which vertex is being referred to.
//
// Cube vertices in the Marching Cubes Algorithm are ordered in a clockwise fashion, starting from the far-left-bottom vertex, and then from the far-left-top vertex.
//
//
//

vec4 ExtrapolateVertex(int index, vec4 invertexpos, float invertexscale)
{
	vec4 outputPosition;

	// Note: Conditionals should ideally be avoided when writing shaders, but occasionally become necessary. This particular, simple, implementation is one such case.
	// A more advanced approach is included in texture_marching_cubes.*
	if(index == 0)
	{
		outputPosition.x = invertexpos.x - (0.5f * invertexscale);
		outputPosition.y = invertexpos.y - (0.5f * invertexscale);
		outputPosition.z = invertexpos.z + (0.5f * invertexscale);
	}
	if(index == 1)
	{
		outputPosition.x = invertexpos.x + (0.5f * invertexscale);
		outputPosition.y = invertexpos.y - (0.5f * invertexscale);
		outputPosition.z = invertexpos.z + (0.5f * invertexscale);
	}
	if(index == 2)
	{
		outputPosition.x = invertexpos.x + (0.5f * invertexscale);
		outputPosition.y = invertexpos.y - (0.5f * invertexscale);
		outputPosition.z = invertexpos.z - (0.5f * invertexscale);
	}
	if(index == 3)
	{
		outputPosition.x = invertexpos.x - (0.5f * invertexscale);
		outputPosition.y = invertexpos.y - (0.5f * invertexscale);
		outputPosition.z = invertexpos.z - (0.5f * invertexscale);
	}
	if(index == 4)
	{
		outputPosition.x = invertexpos.x - (0.5f * invertexscale);
		outputPosition.y = invertexpos.y + (0.5f * invertexscale);
		outputPosition.z = invertexpos.z + (0.5f * invertexscale);
	}
	if(index == 5)
	{
		outputPosition.x = invertexpos.x + (0.5f * invertexscale);
		outputPosition.y = invertexpos.y + (0.5f * invertexscale);
		outputPosition.z = invertexpos.z + (0.5f * invertexscale);
	}
	if(index == 6)
	{
		outputPosition.x = invertexpos.x + (0.5f * invertexscale);
		outputPosition.y = invertexpos.y + (0.5f * invertexscale);
		outputPosition.z = invertexpos.z - (0.5f * invertexscale);
	}
	if(index == 7)
	{
		outputPosition.x = invertexpos.x - (0.5f * invertexscale);
		outputPosition.y = invertexpos.y + (0.5f * invertexscale);
		outputPosition.z = invertexpos.z - (0.5f * invertexscale);
	}

	outputPosition.w = 1.0f;

	return outputPosition;

}

void main()
{
	int i;
	for(i = 0; i < gl_in.length(); i++)
	{
	
		// Create the 8 cube edge vertices.
		for(int j = 0; j < 8; j++)
		{
			vec4 extrapolatedVertex = ExtrapolateVertex(j, worldspaceposition[i], worldspacescale[i]);
			worldspaceposition_g =  worldspaceposition[i].xyz;
			gl_Position =  modelViewProjectionMatrix * extrapolatedVertex;
			EmitVertex();
			EndPrimitive();
		}
	}

	
}