#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D unlitTexture;
uniform sampler2D cascade0;
uniform vec2 probeIntervalsCascade0;
uniform vec2 screenSize;

float exposure = 2.0;

vec3 aces_approx(vec3 v)
{
    v *= exposure * 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}


void main()
{
	float gamma = 2.2;
	vec4 texSample = texture(unlitTexture, TexCoords);
	vec4 lighting = texture(cascade0, TexCoords);
	vec3 combined = texSample.rgb + lighting.rgb;
    vec3 mapped = aces_approx(combined);
	vec3 color = pow(mapped, vec3(1.0/gamma));

	FragColor = vec4(color, 1.0);
}