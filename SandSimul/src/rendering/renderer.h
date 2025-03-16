#pragma once
#include "../world/grid.h"
#include "../util/Shader.h"

class Renderer
{
public:
	void init(Grid* grid);
	void update();
	void cleanup();
	
private:
	unsigned int screenVAO;
	unsigned int screenVBO;
	unsigned int gridTexture;

	Grid* grid;

	Shader cellShader;
};