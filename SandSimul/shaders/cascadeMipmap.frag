#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D cascade0;
uniform vec2 probeIntervalsCascade0;

const vec2 texelSizeCascade0 = 1.0 / textureSize(cascade0, 0);

vec4 calculateRadiance()
{
	float probeSize = probeIntervalsCascade0.x * probeIntervalsCascade0.y; 
	// the number of pixels in the render target is the same as the number of probes in cascade0;
	vec2 currentProbe =  gl_FragCoord.xy;
	vec4 probeRadiance = vec4(0.0);
	vec4 totalRadiance = vec4(0.0);

	for(int i = 0; i < probeSize; i++)
	{
		// get the ray in probe N1
		int rayIndex = i; 

		vec2 rayTexelCoord = vec2( 
			currentProbe.x * probeIntervalsCascade0.x + mod(rayIndex, probeIntervalsCascade0.x) - 2, 
			currentProbe.y * probeIntervalsCascade0.y + floor(float(rayIndex) / probeIntervalsCascade0.y) - 2
		);
		
		vec4 rayRadiance = texture(cascade0, rayTexelCoord * texelSizeCascade0);
		probeRadiance += vec4(rayRadiance.rgb, 1.0);
	}

	totalRadiance += probeRadiance / probeSize;
	return totalRadiance;
}


void main()
{
	vec4 radiance = calculateRadiance();
	FragColor = vec4(radiance.rgb, 1.0);
}