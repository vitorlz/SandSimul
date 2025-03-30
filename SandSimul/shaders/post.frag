#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D unlitTexture;
uniform sampler2D cascade0;
uniform vec2 probeIntervalsCascade0;
uniform vec2 screenSize;

void main()
{
	float gamma = 2.2;

	vec4 texSample = texture(unlitTexture, TexCoords);

	vec4 lighting = texture(cascade0, TexCoords);

	vec3 combined = texSample.rgb + lighting.rgb;

	vec3 color = pow(combined, vec3(1.0/gamma));

	FragColor = vec4(color, 1.0);
   
}