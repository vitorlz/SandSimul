#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D cascadeN1;
uniform sampler2D unlitScene;

uniform int cascadeIndex;
uniform int numOfCascades;
uniform vec2 probeIntervals;
uniform vec2 screenSize;

int intervalLengthBase = 4;

const float PI = 3.14159265359;

void main()
{
	// get the ray position relative to the probe and calculate its index
	
	vec2 rayPosInProbe = vec2(mod(gl_FragCoord.x, probeIntervals.x), mod(gl_FragCoord.y, probeIntervals.y));
	int rayIndex = int(rayPosInProbe.y * probeIntervals.x + rayPosInProbe.x);

	// use the ray index to calculate the ray angle and direction.
	 
	float rayAngle = 2.0 * PI * rayIndex / (probeIntervals.x * probeIntervals.y);
	vec2 rayDir =  vec2(cos(rayAngle), sin(rayAngle));

	// each fragment represents a ray. For each ray/fragment, raymarch through the screen texture in the direction calculated above. Get the
	// radiance when we hit something.

	// here we are using the formula for a geometric sequence: ar^(n - 1) for the current interval length and a(1 - r^n)/(1 - r) for the interval origin 
	// (sum of geometric sequence up to n).

	float intervalLength =  intervalLengthBase * pow(4, cascadeIndex);
	float intervalOrigin = intervalLengthBase * (1 - pow(4, cascadeIndex)) /  (1 - 4);

	// the start of the ray is just the rayDir scaled by interval origin because interval origin is just sum of the length of all other intervals. 
	vec2 rayStart = intervalOrigin * rayDir;
	vec2 rayEnd = rayStart + intervalLength * rayDir;

	// after we raymarch, merge this ray with the rays of cascade N + 1 using bilinear interpolation. Then in the end the radiance field is going to be 
	// encoded into cascade 0, and we sample that for the lighting.	
	
	FragColor = vec4(rayPosInProbe.x / probeIntervals.x, rayPosInProbe.y / probeIntervals.y, 0.0, 1.0);
	
}