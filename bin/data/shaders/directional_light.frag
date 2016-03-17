#version 150

in vec3 posnorm;
in vec4 vertcol;
in vec4 eye;
in vec3 lightdir;

out vec4 finalColor;

void main()
{

	// Directional light shader.
	float lightIntensity = max(dot(normalize(posnorm), normalize(lightdir)), 0.0);

	finalColor = max(lightIntensity * vec4(1.0, 1.0, 1.0, 1.0), vec4(0.25, 0.25, 0.25, 1.0));
	finalColor *= vertcol;

}