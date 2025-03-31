#version 460 core

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

vec4 raymarch(vec2 start, vec2 end, vec2 dir, float stepSize, float maxSteps);

vec4 merge(float rayIndex, vec4 rayRadiance, vec2 probeCoord, vec2 probeCenter)
{
	if (rayRadiance.a == 0.0 || cascadeIndex >= numOfCascades - 1.0)
		return vec4(rayRadiance.rgb, 1.0 - rayRadiance.a);

	vec2 probeSizeN1 = probeIntervals * 2;

	vec2 topLeftN1 =  vec2(probeCenter.x / probeSizeN1.x - 0.5, probeCenter.y / probeSizeN1.y - 0.5);

	vec2 ratio = fract(topLeftN1);

	vec2 topLeftProbeCoordN1 = floor(topLeftN1);

	// ratio.x gives how far away the probe from cascade N is from the top left probe in cascade N + 1 horizontally, ratio.y does the same but vertically.
	
	// these are the weights for TL, TR, BL, BR respectively:
	vec4 weights = vec4((1.0 - ratio.x) * (1.0 - ratio.y), ratio.x * (1.0 - ratio.y), (1.0 - ratio.x) * ratio.y, ratio.x * ratio.y);

	//vec4 weights = vec4(pow(0.25,2), 0.25*0.75, pow(0.75,2), 0.1875);

	// now we need to get the radiance of the 4 rays that point in the same direction as the current ray and add their weighted radiance to the radiance
	// of the current ray. We also need to account for the visibility term.
	
	vec2 offsets[4] = { vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1) };

	vec4 mergedRadiance = vec4(0.0);
	vec4 totalMergedRadiance = vec4(0.0);

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			vec2 probeN1 = topLeftProbeCoordN1 + offsets[j];

			// get the ray in probe N1
			

			float rayIndexN1 = rayIndex * 4 + i;

			vec2 rayN1TexelCoord = vec2( 
				probeN1.x * probeSizeN1.x + mod(rayIndexN1, probeSizeN1.x), 
				probeN1.y * probeSizeN1.y + floor(float(rayIndexN1) / (probeSizeN1.y))
			);

			vec2 texCoord = rayN1TexelCoord / (screenSize);

			vec4 rayRadianceN1 = texture(cascadeN1, texCoord);

			// merge 

			mergedRadiance +=  vec4(rayRadianceN1.rgb * rayRadiance.a, rayRadiance.a) * weights[j];
		}
		
		totalMergedRadiance += mergedRadiance / 4.0;

		mergedRadiance = vec4(0.0);
		
	}


	// based on the current probe's position bilinearly interpolate between the 4 nearest probes
	// get the average of 4 rays in the direction of the current ray in each probe in n + 1 and use the weights to add
	// that to the radiance of the current ray.

	return vec4(rayRadiance.rgb + totalMergedRadiance.rgb, 1.0);
}

void main()
{
	// get the ray position relative to the probe and calculate its index 

	vec2 rayPosInProbe = vec2(mod(gl_FragCoord.x, probeIntervals.x), mod(gl_FragCoord.y, probeIntervals.y));
	float rayIndex = rayPosInProbe.y * probeIntervals.x + rayPosInProbe.x;

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
	//float probeSize = probeIntervals.x * probeIntervals.y;
	
	vec2 probeCoord = vec2(floor(gl_FragCoord.x / probeIntervals.x), floor(gl_FragCoord.y / probeIntervals.y));
	vec2 probeCenter = (probeCoord + 0.5) * probeIntervals;

	vec2 rayStart = probeCenter + intervalOrigin * rayDir;
	vec2 rayEnd = rayStart + intervalLength * rayDir;

	vec4 rayRadiance = raymarch(rayStart, rayEnd, rayDir, 1.0, intervalLength);

	// after we raymarch, merge this ray with the rays of cascade N + 1 using bilinear interpolation. Then in the end the radiance field is going to be 
	// encoded into cascade 0, and we sample that for the lighting.	

	vec4 color = merge(rayIndex, rayRadiance, probeCoord, probeCenter);

	FragColor = color;

}


// naive and unoptimized raymarching for testing

// TODO: sdf

vec4 raymarch(vec2 start, vec2 end, vec2 dir, float stepSize, float intervalLength)
{
	
	float dist = intervalLength;

	vec2 uv = start;

	float steps = int(ceil(dist/stepSize));

	vec2 marchStep = (dir );

	for(int i = 0; i <= steps; i++)
	{
		vec2 texCoord = (uv ) / (screenSize);

		if (texCoord.x < 0.0 || texCoord.x > 1.0 || texCoord.y < 0.0 || texCoord.y > 1.0) 
		{
			return vec4(0.0);
		}

		vec4 currentPixel = texture(unlitScene, texCoord);

		//currentPixel = vec4(pow(currentPixel.rgb, vec3(2.2)), currentPixel.a);

		if(currentPixel.a > 0.2)
		{
			// alpha here becomes the visibility term. It is 0 if we hit something and 1 if we don't.
			if(currentPixel.a == 1.0)
				return vec4(currentPixel.rgb, 0.0);
			else 
				return vec4(0.0);
		}

		uv += marchStep;
	}

	return  vec4(0.0, 0.0, 0.0, 1.0);
}


