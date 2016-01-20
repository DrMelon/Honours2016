#version 150

// Pass-through shader

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform float usingTexture;
uniform float usingColors;
uniform vec4 globalColor;
uniform sampler2D src_tex_unit0;


in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;

out vec2 texcoords;

void main()
{
	gl_Position = modelViewProjectionMatrix * position;
}