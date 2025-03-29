#pragma once
#include "../world/grid.h"
#include "../util/Shader.h"

constexpr int numOfCascades = 4;

class Renderer
{
public:
	void init(Grid* grid, int screenWidth, int screenHeight);
	void update();
	void cleanup();
	
private:
	unsigned int screenVAO;
	unsigned int screenVBO;

	unsigned int radianceFBO;
	
	unsigned int gridTexture;
	unsigned int screenTexture;

	unsigned int unlitFBO;
	unsigned int unlitTexture;

	int screenWidth;
	int screenHeight;

	int probeWidthBase = 4;
	int probeHeightBase = 4;

	unsigned int cascades[numOfCascades];

	Grid* grid;

	Shader cellShader;
	Shader cascadesShader;
	Shader postShader;
};