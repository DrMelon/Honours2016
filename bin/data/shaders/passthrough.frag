#version 150

uniform sampler2DRect theTexture; // Using default pass-through texture
uniform vec2 screenResolution;

in vec2 texcoords;
out vec4 finalColor;

void main()
{
	// Post-FX Passthrough - just draw the texture. 
	// Because openframeworks uses ARB textures (and sampler2DRect types by extension) the texture coordinates map from 0 - textureWidth rather than 0.0 - 1.0. 
	// As such, since this is a Post-FX filter we're drawing over the whole screen, we can just use the screen's coordinates.
	vec2 uv = gl_FragCoord.xy;

	uv.y = screenResolution.y - uv.y; // Flip Y coordinate for GLSL

	finalColor = vec4(1.0, 1.0, 1.0, 1.0);
	finalColor = texture(theTexture, uv);
}