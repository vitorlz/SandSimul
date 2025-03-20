#pragma once 
#include "../util/input.h"
#include <vector>
#include "grid.h"
#include "../util/types.h"

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

	void updateFire(int x, int y);
	void createFire(int x, int y);

	void updateStone(int x, int y);
	void createStone(int x, int y);

	void updateGunpowder(int x, int y);
	void createGunpowder(int x, int y);

	void updateSmoke(int x, int y);
	void createSmoke(int x, int y);

	void updateSteam(int x, int y);
	void createSteam(int x, int y);

	void createAir(int x, int y);

	void createCell(int x, int y, CellType type);
	void spawnCells();

	// returns last air cell found in the direction specified.
	types::Pos disperse(int x, int y, int dispersion, int dirX, int dirY, bool stopAtNonEmpty = true);

	int brushSize = 5;

	void brushDraw(int centerX, int centerY);

	struct Brush
	{
		CellType type = SAND;
		CellKind kind = SOLID_MOVABLE;
		float spawnRate = 40.0f;
		int sparsity = 3;
		bool continuous = false;
	} brush;

	bool mouseDownLastFrame = false;
};