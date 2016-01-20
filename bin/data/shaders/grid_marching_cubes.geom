#version 330

// Using extension to allow multidimensional c-style arrays.
#extension GL_ARB_arrays_of_arrays : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_gpu_shader4 : enable


// Grid-Based Marching Cubes Implementation, Geometry Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: The geometry shader is where the marching cubes algorithm will take place; the density function is defined here, but CSG operands that modify the density function must be passed in from the main application.
// The implementation is based on Paul Bourke's "Polygonising a Scalar Field" with appropriate adjustments made for this specific implementation.

layout (points) in;
layout (triangle_strip, max_vertices=16) out;


// We need to include the modelView and projection matrices so that we can transform the terrain's vertices based on the camera, and send them to the fragment shader.
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform isampler2D src_tex_unit0;

uniform float isolevel;

in gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];

}gl_in[];

in vec4 worldspaceposition[];
in float worldspacescale[];
in vec3 gridoffset_g[];

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

// This is the density function that represents our entire terrain. 
// It is through this value that the terrain can be explored.
//
float DensityFunction(vec3 worldspaceposition)
{
	float density;

	density = -worldspaceposition.y;

	return density;
}

// Marching Cubes Tables
// These are from Paul Bourke's 1994 Paper, Polygonising A Scalar Field, and are used here because their generation is nontrivial.
uniform int edgeTable[256]={
0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };




// This function allows the triangles in the marching cubes test cases to smoothly map to the surface of the terrain, by interpolating the position of the vertices towards
// the intersection point.
// This code is adapted from Paul Bourke's 1994 paper, Polygonizing a Scalar Field.


vec3 InterpolateVertex(vec3 point1, vec3 point2, float density1, float density2)
{
	float mu;
	vec3 outputPoint;
	
	if(abs(isolevel - density1) < 0.00001)
	{
		return point1;
	}
	if(abs(isolevel - density2) < 0.00001)
	{
		return point2;
	}
	if(abs(density1 - density2) < 0.00001)
	{
		return point1;
	}

	mu = (isolevel - density1) / (density2 - density1);
	outputPoint.x = point1.x + mu * (point2.x - point1.x);
	outputPoint.y = point1.y + mu * (point2.y - point1.y);
	outputPoint.z = point1.z + mu * (point2.z - point1.z);

	return outputPoint;

}

int triTable(int cube, int index)
{
	return texelFetch2D(src_tex_unit0, ivec2(index, cube), 0).a;
}


void main()
{
	int i;
	for(i = 0; i < gl_in.length(); i++)
	{
		// Get the 8 cube edge vertices.
		vec4 cubeVertex0 = ExtrapolateVertex(0, worldspaceposition[i], worldspacescale[i]);
		vec4 cubeVertex1 = ExtrapolateVertex(1, worldspaceposition[i], worldspacescale[i]);
		vec4 cubeVertex2 = ExtrapolateVertex(2, worldspaceposition[i], worldspacescale[i]);
		vec4 cubeVertex3 = ExtrapolateVertex(3, worldspaceposition[i], worldspacescale[i]);
		vec4 cubeVertex4 = ExtrapolateVertex(4, worldspaceposition[i], worldspacescale[i]);
		vec4 cubeVertex5 = ExtrapolateVertex(5, worldspaceposition[i], worldspacescale[i]);
		vec4 cubeVertex6 = ExtrapolateVertex(6, worldspaceposition[i], worldspacescale[i]);
		vec4 cubeVertex7 = ExtrapolateVertex(7, worldspaceposition[i], worldspacescale[i]);

		// This is where we store which one of the 256 possible marching cube cases is used for this sample point.
		int cubeIndex = 0;
		// Using what is known about each point being inside or outside of the terrain's surface (the isosurface, given by the isolevel here), we can build
		// a 8-bit integer bitwise, and this will be used to select the specific case.
		if(DensityFunction(cubeVertex0.xyz) < isolevel) cubeIndex |= 1;
		if(DensityFunction(cubeVertex1.xyz) < isolevel) cubeIndex |= 2;
		if(DensityFunction(cubeVertex2.xyz) < isolevel) cubeIndex |= 4;
		if(DensityFunction(cubeVertex3.xyz) < isolevel) cubeIndex |= 8;
		if(DensityFunction(cubeVertex4.xyz) < isolevel) cubeIndex |= 16;
		if(DensityFunction(cubeVertex5.xyz) < isolevel) cubeIndex |= 32;
		if(DensityFunction(cubeVertex6.xyz) < isolevel) cubeIndex |= 64;
		if(DensityFunction(cubeVertex7.xyz) < isolevel) cubeIndex |= 128;

		
		


		// Now that we have the cube index, we can create a triangle from the verts listed in the triangle table.
		// This list will keep track of which vertices of the cube will be used when creating new triangles.
		vec3 vertList[120];

		if(!(edgeTable[cubeIndex] == 0)) // point is not fully inside or outside the surface
		{

				vertList[0] = InterpolateVertex(cubeVertex0.xyz, cubeVertex1.xyz, DensityFunction(cubeVertex0.xyz), DensityFunction(cubeVertex1.xyz));
				vertList[1] = InterpolateVertex(cubeVertex1.xyz, cubeVertex2.xyz, DensityFunction(cubeVertex1.xyz), DensityFunction(cubeVertex2.xyz));
				vertList[2] = InterpolateVertex(cubeVertex2.xyz, cubeVertex3.xyz, DensityFunction(cubeVertex2.xyz), DensityFunction(cubeVertex3.xyz));
				vertList[3] = InterpolateVertex(cubeVertex3.xyz, cubeVertex0.xyz, DensityFunction(cubeVertex3.xyz), DensityFunction(cubeVertex0.xyz));
				vertList[4] = InterpolateVertex(cubeVertex4.xyz, cubeVertex5.xyz, DensityFunction(cubeVertex4.xyz), DensityFunction(cubeVertex5.xyz));
				vertList[5] = InterpolateVertex(cubeVertex5.xyz, cubeVertex6.xyz, DensityFunction(cubeVertex5.xyz), DensityFunction(cubeVertex6.xyz));
				vertList[6] = InterpolateVertex(cubeVertex6.xyz, cubeVertex7.xyz, DensityFunction(cubeVertex6.xyz), DensityFunction(cubeVertex7.xyz));
				vertList[7] = InterpolateVertex(cubeVertex7.xyz, cubeVertex4.xyz, DensityFunction(cubeVertex7.xyz), DensityFunction(cubeVertex4.xyz));
				vertList[8] = InterpolateVertex(cubeVertex0.xyz, cubeVertex4.xyz, DensityFunction(cubeVertex0.xyz), DensityFunction(cubeVertex4.xyz));
				vertList[9] = InterpolateVertex(cubeVertex1.xyz, cubeVertex5.xyz, DensityFunction(cubeVertex1.xyz), DensityFunction(cubeVertex5.xyz));
				vertList[10] = InterpolateVertex(cubeVertex2.xyz, cubeVertex6.xyz, DensityFunction(cubeVertex2.xyz), DensityFunction(cubeVertex6.xyz));
				vertList[11] = InterpolateVertex(cubeVertex3.xyz, cubeVertex7.xyz, DensityFunction(cubeVertex3.xyz), DensityFunction(cubeVertex7.xyz));
			
			
		

	
			// Create the triangles
			for(int j = 0; triTable(cubeIndex,j) != -1; j += 3)
			{
				gl_Position = modelViewProjectionMatrix * vec4(vertList[triTable(cubeIndex,j)], 1.0);
				EmitVertex();
				gl_Position = modelViewProjectionMatrix * vec4(vertList[triTable(cubeIndex,j+1)], 1.0);
				EmitVertex();
				gl_Position = modelViewProjectionMatrix * vec4(vertList[triTable(cubeIndex,j+2)], 1.0);
				EmitVertex();
				EndPrimitive();
			}

		}
	}

	
}