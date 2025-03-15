#version 300 es

precision mediump float;

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D worldTexture;

void main()
{

   float type = texture(worldTexture, TexCoords).r * 255.0;
   float random = texture(worldTexture, TexCoords).a;

   if(type == 1.0)
   {
		FragColor = vec4(0.969, 0.922, 0.698, 1.0) * random;
   }
   else
   {
		FragColor = vec4(texture(worldTexture, TexCoords).g);
   }
}