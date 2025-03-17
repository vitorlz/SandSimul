#include "world.h"
#include "iostream"
#include "random"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

void World::init(Input* input)
{
	this->input = input;
}

void World::updateCells()
{
	if (input->keyPressed[GLFW_KEY_1])
	{
		brush = { SAND, SOLID_MOVABLE, false };
	}
	else if (input->keyPressed[GLFW_KEY_2])
	{
		brush = { WATER, FLUID, false };
	}
	else if (input->keyPressed[GLFW_KEY_3])
	{
		brush = { WOOD, SOLID_IMMOVABLE, true };
	}
	else if (input->keyPressed[GLFW_KEY_BACKSPACE])
	{
		brush = { AIR, FLUID, true };
	}

	if (input->isleftMouseDown)
	{
		spawnCells();
		mouseDownLastFrame = true;
	}
	else 
	{
		mouseDownLastFrame = false;
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
			case WOOD:
				updateWood(i, j);
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
	case AIR:
		createAir(x, y);
		break;
	case SAND:
		createSand(x, y);
		break;
	case WATER:
		createWater(x, y);
		break;
	case WOOD:
		createWood(x, y);
		break;
	}
}

static double lastTime = 0;
static double limit = 1 / 20.0;

void World::spawnCells()
{
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastTime;

	static int lastCircleCenterX;
	static int lastCircleCenterY;

	if (deltaTime >= limit)
	{
		int mouseX = input->mouseX;
		int mouseY = input->mouseY;

		float relativeX = input->mouseX / 800.0;
		float relativeY = input->mouseY / 800.0;

		int centerX = (int)(relativeX * grid.size);
		int centerY = (int)(relativeY * grid.size);

		brushDraw(centerX, centerY);

		if (mouseDownLastFrame && brush.continuous)
		{
			int distX = lastCircleCenterX - centerX;
			int distY = lastCircleCenterY - centerY;

			float m = 0;

			if (distX != 0)
			{
				m = (float)distY / (float)distX;
			}			

			if (distX == 0)
			{
				for (int i = 1; i <= std::abs(distY); i++)
				{
					int y = distY < 0 ? -i : i;

					brushDraw(centerX, centerY + y);
				}
			}
			else
			{
				int distance = distX;
				bool usingYDistance = false;
				if (std::abs(distX) < std::abs(distY))
				{
					distance = distY;
					usingYDistance = true;
				}

				for (int i = 1; i <= std::abs(distance); i++)
				{
					
					int x;
					int y;

					if (usingYDistance)
					{
						y = distY < 0 ? -i : i;
						x = std::round(float(y) / m);
						
					}
					else
					{
						x = distX < 0 ? -i : i;
						y = std::round(m * (float)x);
					}

					brushDraw(centerX + x,centerY + y);
				}
			}
		}

		lastTime = currentTime;

		lastCircleCenterX = centerX;
		lastCircleCenterY = centerY;
	}
}

void World::createSand(int x, int y)
{
	CellData sand =
	{
		SAND,
		SOLID_MOVABLE,
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
				// disperse diagonally 
				disperseFluid(x, y, dispersion, randomOffset, 1);
			}
			else if (cellSide.type == AIR)
			{
				// disperse to the sides
				disperseFluid(x, y, dispersion, randomOffset, 0);
			}
		}
	}
}

void World::disperseFluid(int x, int y, int dispersion, int dirX, int dirY)
{
	// iterate through the cells in the direction defined by dirX and dirY until there is no air or up to 
	// the value of dispersion, then swap with the last air cell found
	CellData currentCell = grid.get(x, y);
	CellData cellSide;

	glm::vec2 lastAirCellPos{};
	int i = 1;
	while (i <= dispersion)
	{
		int xOffset = dirX * i;
		int yOffset = dirY * i;

		cellSide = grid.get(x + xOffset, y + yOffset);

		if (cellSide.type == AIR)
		{
			lastAirCellPos = glm::vec2(x + xOffset, y + yOffset);

			// if did not find non-air cells and reached max dispersion or end of the map, swap.
			if (i == dispersion || x + xOffset == grid.size - 1 || x + xOffset == 0 
				|| y + yOffset == grid.size - 1 || y + yOffset == 0)
			{	
				grid.set(x, y, cellSide);
				grid.set(lastAirCellPos.x, lastAirCellPos.y, currentCell);

				break;
			}
		}
		else 
		{
			// found non-air cell: swap with last air cell.
			grid.set(x, y, grid.get(lastAirCellPos.x, lastAirCellPos.y));
			grid.set(lastAirCellPos.x, lastAirCellPos.y, currentCell);

			break;
		}
		i++;
	}
}

void World::brushDraw(int centerX, int centerY)
{
	for (int i = -brushSize; i <= brushSize; i++) {
		for (int j = -brushSize; j <= brushSize; j++) {
			float distanceSquared = i * i + j * j;
			float radiusSquared = brushSize * brushSize;

			// randomize cell position in circle
			if ((int)distanceSquared % (rand() % 3 + 1) == 0 && brush.kind != SOLID_IMMOVABLE && brush.type != AIR)
				continue;

			if (distanceSquared <= radiusSquared + brushSize * 0.5f) {
				int gridX = centerX + i;
				int gridY = centerY + j;

				if (gridX >= 0 && gridX < grid.size && gridY >= 0 && gridY < grid.size) {

					if (grid.get(gridX, gridY).type == AIR || brush.type == AIR)
					{
						createCell(gridX, gridY, brush.type);
					}
				}
			}
		}
	}

}

void World::createWater(int x, int y)
{
	CellData water =
	{
		WATER,
		FLUID,
		types::color8{ 43, 153, 235 },
		false
	};

	float f = types::genRandom(230, 255) / 255.0f;

	water.color = water.color * f;
	grid.set(x, y, water);
}


void World::updateWood(int x, int y)
{
}

void World::createWood(int x, int y)
{
	CellData wood =
	{
		WOOD,
		SOLID_IMMOVABLE,
		types::color8{ 51, 27, 12 },
		false
	};

	float f = types::genRandom(200, 255) / 255.0f;

	wood.color = wood.color * f;
	grid.set(x, y, wood);
}

void World::createAir(int x, int y)
{
	CellData air =
	{
		AIR,
		FLUID,
		types::color8{ 255, 255, 255 },
		false
	};

	grid.set(x, y, air);
}

