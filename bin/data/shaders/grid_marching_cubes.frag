#version 330

// Grid-Based Marching Cubes Implementation, Fragment Shader
// Author: J. Brown (1201717)
// Date: 19/01/2016
// Purpose: After the marching cubes algorithm has concluded in the geometry shader, this shader will colour and shade the terrain appropriately.

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

out vec4 outcolour;



in vec3 normalOfVertex;

void main()
{
	
	outcolour = vec4(normalOfVertex.x, normalOfVertex.y, normalOfVertex.z, 1.0);

	//float fogAmt = ((gl_FragCoord.z / gl_FragCoord.w) / 128.0f);

	//outcolour = mix(outcolour, vec4(1.0, 1.0,1.0,1.0), fogAmt);
	//outcolour.a = 1.0f;
}
