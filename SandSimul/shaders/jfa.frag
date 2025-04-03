#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D tex;
uniform int jumpSize;

vec4 jfa()
{
	vec4 texSample = texture(tex, TexCoords);
	vec2 texelSize = 1.0 / textureSize(tex, 0);
	vec2 texSize = textureSize(tex, 0);
	
	// we are storing the distance in the r component and the tex coords of the next best sample in gb
	float dist = texSample.r;
	vec2  bestNeighborCoords = texSample.gb;
	for(int x = -1; x <= 1; x++)
	{
		for(int y= -1; y <= 1; y++)
		{
			vec2 offset = jumpSize * vec2(x, y);
			
			vec2 neighborCoords = TexCoords + offset * texelSize;

			if (neighborCoords != clamp(neighborCoords, 0.0, 1.0)) continue;
			
			vec4 neighbor = texture(tex, neighborCoords);

			vec2 candidateSeed = neighbor.gb;

			vec2 currentPixel = TexCoords * vec2(texSize);
			vec2 seedPixel   = candidateSeed * vec2(texSize);
			float newDist = length(currentPixel - seedPixel);
			
			if(newDist < dist)
			{
				dist = newDist;
				bestNeighborCoords = candidateSeed;	
			}
		}
	}

	return vec4(dist, bestNeighborCoords, texSample.a);

}

void main()
{
	FragColor = jfa();
}

