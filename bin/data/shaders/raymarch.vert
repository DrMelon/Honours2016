#version 330

// Pass-through shader

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform float usingTexture;
uniform float usingColors;
uniform vec4 globalColor;



in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoords;


out vec2 texCoord;

void main()
{
	texCoord = texcoords;
	gl_Position = modelViewProjectionMatrix * position;
}