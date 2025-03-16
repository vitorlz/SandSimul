#include "world.h"
#include "iostream"
#include "random"
#include <GLFW/glfw3.h>

void World::init(Input* input)
{
	this->input = input;
}

void World::updateCells()
{
	if (input->keyPressed[GLFW_KEY_1])
	{
		brushType = SAND;
	}
	else if (input->keyPressed[GLFW_KEY_2])
	{
		brushType = WATER;
	}

	if (input->isleftMouseDown)
	{
		spawnCells();
	}

	for (int i = grid.size - 1; i >= 0; i--)
	{
		for (int j = grid.size - 1; j >= 0; j--)
		{
			switch (grid.get(i, j).type)
			{
			case SAND:
				updateSand(i, j);
				break;
			case WATER:
				updateWater(i, j);
				break;
			}
		}
	}

	for (int i = grid.size - 1; i >= 0; i--)
	{
		for (int j = grid.size - 1; j >= 0; j--)
		{
			CellData& cell = grid.get(i, j);
			cell.isUpdated = false;
			
			grid.setCellTextureColor(i, j, cell.color);
		}
	}
}

void World::updateSand(int x, int y)
{
	CellData currentCell = grid.get(x, y);
	if (y < grid.size - 1 && !currentCell.isUpdated)
	{
		CellData cellBelow = grid.get(x, y + 1);

		int randomOffset = types::genRandom(-1, 1);
		currentCell.isUpdated = true;

		if (cellBelow.type == AIR || cellBelow.type == WATER)
		{
			grid.set(x, y, cellBelow);
			grid.set(x, y + 1, currentCell);
			
		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			CellData cellDiagonal = grid.get(x + randomOffset, y + 1);

			if (cellDiagonal.type == AIR || cellDiagonal.type == WATER)
			{
				grid.set(x, y, cellDiagonal);
				grid.set(x + randomOffset, y + 1, currentCell);
			}
		}
	}
}

void World::createCell(int x, int y, CellType type)
{
	switch (type)
	{
	case SAND:
		createSand(x, y);
		break;
	case WATER:
		createWater(x, y);
		break;
	}
}

static double lastTime = 0;
static double limit = 1 / 20.0;

void World::spawnCells()
{
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastTime;

	if (deltaTime >= limit)
	{
		int mouseX = input->mouseX;
		int mouseY = input->mouseY;

		float relativeX = input->mouseX / 800.0;
		float relativeY = input->mouseY / 800.0;

		for (int i = -brushSize; i <= brushSize; i++) {
			for (int j = -brushSize; j <= brushSize; j++) {
				float distanceSquared = i * i + j * j;
				float radiusSquared = brushSize * brushSize;

				// randomize cell position in circle
				if ((int)distanceSquared % (rand() % 3 + 1) == 0)
					continue;
				
				if (distanceSquared <= radiusSquared + brushSize * 0.5f) {
					int gridX = (int)(relativeX * grid.size) + i;
					int gridY = (int)(relativeY * grid.size) + j;

					if (gridX >= 0 && gridX < grid.size && gridY >= 0 && gridY < grid.size) {

						if (grid.get(gridX, gridY).type == AIR)
						{
							createCell(gridX, gridY, brushType);
						}
					}
				}
			}
		}

		lastTime = currentTime;
	}
}

void World::createSand(int x, int y)
{
	CellData sand =
	{
		SAND,
		types::color8{.r = 247, .g = 235, .b = 178 },
		false
	};

	float f = types::genRandom(200, 255) / 255.0f;

	sand.color = sand.color * f;
	grid.set(x, y, sand);
}

void World::updateWater(int x, int y)
{
	int dispersion = 5;
	CellData currentCell = grid.get(x, y);
	if (y < grid.size - 1 && !currentCell.isUpdated)
	{
		CellData cellBelow = grid.get(x, y + 1);

		int randomOffset = types::genRandom(-1, 1);

		currentCell.isUpdated = true;

		if (cellBelow.type == AIR)
		{
			grid.set(x, y, cellBelow);
			grid.set(x, y + 1, currentCell);

		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			CellData cellDiagonal = grid.get(x + randomOffset, y + 1);
			CellData cellSide = grid.get(x + randomOffset, y);

			if (cellDiagonal.type == AIR)
			{
				grid.set(x, y, cellDiagonal);
				grid.set(x + randomOffset, y + 1, currentCell);
			}
			else if (cellSide.type == AIR)
			{
				// iterate through the cells on the side randomOffset is pointing at (-1 = left, 1 = right) until there is no air or up to 
				// the value of dispersion.
				disperseFluid(x, y, dispersion, randomOffset, 0);
			}
		}
	}
}

void World::disperseFluid(int x, int y, int dispersion, int dirX, int dirY)
{
	CellData currentCell = grid.get(x, y);
	CellData cellSide;

	CellData lastAirCell;

	int i = 1;
	while (i <= dispersion && ((x + dirX * i) < grid.size) && ((x + dirX * i) >= 0))
	{
		cellSide = grid.get(x + dirX * i, y);

		if (cellSide.type == AIR)
		{
			if (i == dispersion)
			{
				// reached maximum dispersion: swap.
				grid.set(x, y, cellSide);
				grid.set(x + dirX * i, y, currentCell);

				break;
			}
		}
		else 
		{
			// found non-air cell: swap with last air cell.
			grid.set(x, y, grid.get(x + dirX * i - dirX, y));
			grid.set(x + dirX * i - dirX, y, currentCell);
			
			break;
		}

		i++;
	}
}

void World::createWater(int x, int y)
{
	CellData water =
	{
		WATER,
		types::color8{ 43, 153, 235 },
		false
	};

	float f = types::genRandom(230, 255) / 255.0f;

	water.color = water.color * f;
	grid.set(x, y, water);
}


