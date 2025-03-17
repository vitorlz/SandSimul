#pragma once 
#include "../util/input.h"
#include <vector>
#include "grid.h"

class World
{
public:
	void init(Input* input);
	void updateCells();

	Input* input;
	Grid grid;

private:
	void updateSand(int x, int y);
	void createSand(int x, int y);

	void updateWater(int x, int y);
	void createWater(int x, int y);


	void updateWood(int x, int y);
	void createWood(int x, int y);

	void createAir(int x, int y);

	void createCell(int x, int y, CellType type);
	void spawnCells();

	void disperseFluid(int x, int y, int dispersion, int dirX, int dirY);

	int brushSize = 5;

	struct Brush
	{
		CellType type = SAND;
		CellKind kind = SOLID_MOVABLE;
	} brush;
	
};