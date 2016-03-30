#version 330

uniform vec2 screenResolution = vec2(320, 240);
uniform vec3 cameraPosition = vec3(0,0,-5);
uniform vec3 cameraUpVector = vec3(0,1,0);
uniform vec3 cameraLookTarget = vec3(0,0,0);

in vec2 texCoord;
out vec4 finalColor;

// Raymarching Shader

//// Functions

// Raymarch Advance Function; very simple, steps a ray along in a direction.
vec3 AdvanceRay(vec3 startPoint, vec3 startDirection, float advanceAmt)
{
	return startPoint + advanceAmt * startDirection;
}

// Set of functions for representing distance-fields for shapes.
// The Y-Coordinate here is used for material/texturing information.
vec2 ShapeFlatFloor(vec3 worldPosition)
{
	return vec2(worldPosition.y + 10.0f, 0.0f);
}

vec2 CSG_Sphere( vec3 position, float size, vec3 worldPosition)
{
	return vec2(length(worldPosition - position) - size, 0.0f);
}

// The CSG functions are simple to calculate;
// Anything that is closer to the camera than the other will be shown instead.
vec2 CSG_Union(vec2 density1, vec2 density2)
{
	if(density1.x < density2.x)
	{
		return density1;
	}
	else
	{
		return density2;
	}
}

// This function calculates the density/distance field.
vec2 DistanceField(vec3 worldPosition)
{
	// for now, just display a flat plane
	vec2 Density;

	Density = ShapeFlatFloor(worldPosition);

	Density = CSG_Union(Density, CSG_Sphere(vec3(0,0,-50), 20, worldPosition));

	return Density;
}

void main()
{
	// Distance-field raymarch shader.
	// Based on collected works by Inigo Quilez.
	// [link]

	vec2 screenPosition = gl_FragCoord.xy / screenResolution;
	screenPosition.y = 1.0f - screenPosition.y;
	screenPosition.x = 1.0f - screenPosition.x;
	vec2 screenPositionOffset = -1.0f + 2.0f * screenPosition;

	

	// First, Initialize Camera
	vec3 cameraDirection = normalize(cameraLookTarget - cameraPosition);
	vec3 cameraRight = normalize(cross(cameraUpVector, cameraDirection));
	vec3 cameraLowerBound = cross(cameraDirection, cameraRight);
	vec3 cameraStep = cameraPosition + cameraDirection;
	
	vec3 screenCoordinate = cameraStep + (screenPositionOffset.x * cameraRight * 0.8) + (screenPositionOffset.y * cameraLowerBound * 0.8);
	vec3 eyeCoordinate = normalize(screenCoordinate - cameraPosition);

	// Now, do raymarching.
	vec3 minDistance = vec3(0.02, 0, 0);
	// This is the limit on how far the ray can go before deciding there's nothing there.
	float maximumDepth = 150.0f;

	vec2 currentDistance = minDistance.xy;

	vec3 currentColour;
	vec3 currentHitPosition;
	vec3 currentHitNormal;

	float rayDistanceTravelled = 1.0f;

	// Evalute distance field, 256 steps
	for(int i = 0; i < 256; i++)
	{
		if((abs(currentDistance.x) < 0.001) || (rayDistanceTravelled > maximumDepth))
		{
			break;
		}

		// Advance ray forwards.
		rayDistanceTravelled += currentDistance.x;
		currentHitPosition = cameraPosition + (eyeCoordinate * rayDistanceTravelled);
		currentDistance = DistanceField(currentHitPosition);
	}

	// If the ray hit something
	if(rayDistanceTravelled < maximumDepth)
	{
		currentColour = vec3(0.1f, 0.85f, 0.1f);

		// Calculate hit normal
		vec3 normalCalc = vec3(currentDistance.x - DistanceField((currentHitPosition - minDistance.xyy)).x,
							   currentDistance.x - DistanceField((currentHitPosition - minDistance.yxy)).x, 
							   currentDistance.x - DistanceField((currentHitPosition - minDistance.yyx)).x);

		currentHitNormal = normalize(normalCalc);

		// extremely simple light, light pos = cam pos
		float lightIntensity = dot(currentHitNormal, normalize(vec3(25,5,30) - currentHitPosition));

		// phong calc
		vec4 lightCol = vec4((lightIntensity * currentColour + pow(lightIntensity, 16.0f)) * (1.0 - length(vec3(25,5,30) - currentHitPosition) * 0.01f), 1.0f);

		finalColor = lightCol;

	}
	else
	{
		// Ray didn't hit, return darkness
		finalColor = vec4(0.0f,0.0f,0.0f,1);
	}






}

