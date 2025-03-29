#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D unlitTexture;
uniform sampler2D cascade0;
uniform vec2 probeIntervalsCascade0;
uniform vec2 screenSize;

void main()
{

	float probeSize = probeIntervalsCascade0.x * probeIntervalsCascade0.y; 

	vec2 topLeftProbe =  vec2(gl_FragCoord.x / probeSize - 0.5, (gl_FragCoord.y / probeSize - 0.5));
	
	vec2 topLeftProbeCoord = floor(topLeftProbe);

	vec2 ratio = fract(topLeftProbe);
	
	// these are the weights for TL, TR, BL, BR respectively:
	vec4 weights = vec4((1.0 - ratio.x) * (1.0 - ratio.y), ratio.x * (1.0 - ratio.y), (1.0 - ratio.x) * ratio.y, ratio.x * ratio.y);
	
	vec2 offsets[4] = { vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1) };

	vec4 probeRadiance = vec4(0.0);
	vec4 totalRadiance = vec4(0.0);

	// we need to sum the rays of the 4 probes and add then to this pixel's lighting using the weights.

	for(int i = 0; i < 4; i++)
	{
		vec2 probe = topLeftProbeCoord + offsets[i];

		for(int j = 0; j < probeSize; j++)
		{
			
			// get the ray in probe N1
			int rayIndex = j; 

			vec2 rayTexelCoord = vec2( 
				probe.x * probeSize + mod(rayIndex, probeIntervalsCascade0.x), 
				floor(rayIndex / probeIntervalsCascade0.x) + probe.y * probeSize
			);

			//rayTexelCoord = clamp(rayTexelCoord, vec2(0.0), vec2(1.0));

			vec4 rayRadiance = texture(cascade0,vec2(rayTexelCoord.x / screenSize.x, 1.0 - (rayTexelCoord.y / screenSize.y)));

			if (any(isnan(rayRadiance.rgb)) || any(isinf(rayRadiance.rgb))) {
				
				 rayRadiance = vec4(0.0);  
			}

			// merge

			probeRadiance += vec4(rayRadiance.rgb, 1.0);
		}

		totalRadiance += probeRadiance * weights[i] / probeSize;
		
		probeRadiance = vec4(0.0);
	}


	float gamma = 2.2;

	vec4 texSample = texture(unlitTexture, TexCoords);

	vec3 color = pow(texSample.rgb + totalRadiance.rgb, vec3(1/gamma));

	FragColor = vec4(color, 1.0);
   
}