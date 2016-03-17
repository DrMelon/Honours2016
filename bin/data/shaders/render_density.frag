#version 150

uniform sampler2DRect theTexture; // Using default pass-through texture
uniform samplerBuffer csgtex;
uniform float numberOfCSG;
uniform vec2 screenResolution;

uniform vec3 gridRes;
uniform vec3 gridOffset;

uniform float isolevel;

in vec2 texcoords;
out vec4 finalColor;

// This shader renders the density function at the grid sample points to a texture. 
// What this means is that after the density function has been evaluated for these points, the resultant texture 
// can then be used in the geometry shader in the next pass to sample the density with less intensity. 
// This method is less accurate, but much faster.

//
// Noise functions borrowed from: [SOURCE]
//
float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise_g(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

float CSG_Sphere( vec3 position, float size, vec3 worldspace )
{
	return length(worldspace - position) - size;
}

float CSG_Box(vec3 position, vec3 bounds, vec3 worldspace)
{
	return length(max(abs(worldspace - position) - bounds, 0.0));
}

float CSG_RoundBox(vec3 position, vec3 bounds, vec3 worldspace, float roundness)
{
	return length(max(abs(worldspace - position) - bounds, 0.0)) - roundness;
}

// And these functions are used to combine shapes and fields together.

float CSG_Union( float density1, float density2 )
{
	return max(density1, isolevel - density2);
}

float CSG_Subtract( float density1, float density2 )
{
	return min(density1, density2 - isolevel);
}

float csgTable(int x, int y)
{
	//return 0.0f;
	return float(texelFetch(csgtex, (x + 8*y)).r);
}


// This is the density function that represents our entire terrain. 
// It is through this value that the terrain can be explored.
//
float DensityFunction(vec3 worldspaceposition)
{
	float density = 0.0f;


	// Set a floor at 0, 0, 0.
	density = -worldspaceposition.y;

	return density;
	
	// Perturb the surface with noise.
	density += (noise_g(worldspaceposition / 80.0)) * 50.0f;
	density += (noise_g(worldspaceposition / 40.0)) * 50.0f;

	// Perform CSG functions here.

	for(int i = 1; i < int(numberOfCSG); i++)
	{

		if(csgTable(0, i) == 0)
		{
			//Add mode
			if(csgTable(1, i) == 0)
			{
				// Sphere mode
				density = CSG_Union(density, CSG_Sphere( vec3(csgTable(2, i), csgTable(3, i), csgTable(4, i)), csgTable(5, i), worldspaceposition));
			}
		}
		if(csgTable(0, i) == 1)
		{
			//Subtract mode
			if(csgTable(1, i) == 0)
			{
				// Sphere mode
				density = CSG_Subtract(density, CSG_Sphere( vec3(csgTable(2, i), csgTable(3, i), csgTable(4, i)), csgTable(5, i), worldspaceposition));
			}
		}
	}
	//density = CSG_Union(density, CSG_Sphere(vec3(0.0f, 150.0f*sin(time), 0.0f), 25.0f, worldspaceposition));
	//density = CSG_Subtract(density, CSG_Sphere(vec3(129.0f, 50.0f*sin(time), 0.0f), 50.0f, worldspaceposition));
	//density = CSG_Union(density, CSG_RoundBox(vec3(0.0f, 50.0f, 130.0f), vec3(30.0f, 45.0f*sin(time)+45.0f, 30.0f*cos(time)+30.0f), worldspaceposition, 5.0f));
	
	
	
	

	return density;
}


void main()
{

	vec2 uv = gl_FragCoord.xy;
	
	// Calculate new coordinates for 3D texture sample
	float xCoord = uv.x;
	float zCoord = 0;
	// Get wrapped amt 
	while(xCoord > gridRes.x)
	{
		xCoord -= gridRes.x;
		zCoord += 1.0f;
	}
	float yCoord = screenResolution.y - uv.y;

	// Sample density at this point
	float density = DensityFunction(vec3(xCoord,yCoord,zCoord) + gridOffset );

	// Colour.R = sampled density.

	finalColor = vec4(density, 0, 0, 1.0);
	
}