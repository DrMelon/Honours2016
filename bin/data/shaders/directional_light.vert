#version 150

// Pass-through shader

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
in vec2 texcoord;

out vec3 posnorm;
out vec4 vertcol;
out vec4 eye;
out vec3 lightdir;

void main()
{
	mat3 norm_mat = transpose(inverse(mat3(modelViewMatrix)));

	mat4 modelMatrix = modelViewMatrix * inverse(viewMatrix);

	lightdir = vec3(normalize((viewMatrix) * vec4(1.0, 1.0, 0.0, 0.0)));
	posnorm = normalize(norm_mat * vec3(normal));
	vertcol = color;
	eye = -(modelViewMatrix * position);




	gl_Position = modelViewProjectionMatrix * position;
}