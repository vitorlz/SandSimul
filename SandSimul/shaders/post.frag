#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D unlitTexture;

void main()
{

	float gamma = 2.2;

	vec4 texSample = texture(unlitTexture, TexCoords);

	vec3 color = pow(texSample.rgb, vec3(1/gamma));

	FragColor = vec4(color, texSample.a);
   
}