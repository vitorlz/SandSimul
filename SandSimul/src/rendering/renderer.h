#pragma once
#include "../world/grid.h"
#include "../util/shaderManager.h"

constexpr int numOfCascades = 6;

class Renderer
{
public:
	void init(Grid* grid, int screenWidth, int screenHeight, ShaderManager* shaderManager);
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

	unsigned int cascadeMipmapFBO;
	unsigned int cascadeMipmapTexture;

	unsigned int jfaFbo;
	unsigned int jfaTex[2];
	unsigned int firstJfaTex;

	int screenWidth;
	int screenHeight;

	int probeWidthBase = 4;
	int probeHeightBase = 4;

	int maxJfaJump = 2048;

	unsigned int cascades[numOfCascades];

	ShaderManager* shaderManager;

	Grid* grid;
};