#version 300 es

precision mediump float;

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D worldTexture;

void main()
{

	FragColor = vec4(texture(worldTexture, TexCoords).rgb, 1.0);
   
}