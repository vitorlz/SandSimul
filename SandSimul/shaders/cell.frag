#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 DistanceField;

in vec2 TexCoords;

uniform sampler2D worldTexture;

void main()
{
	vec4 texSample = texture(worldTexture, TexCoords);

	// first texture for jfa
	vec2 seed = (texSample.a > 0.2) ? TexCoords : vec2(-2.0, -2.0);
	float dist = (texSample.a > 0.2) ? 0.0 : 9999.0;
	DistanceField = vec4(dist, seed, texSample.a);

	FragColor = texSample; 
}