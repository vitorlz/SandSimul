#include "world.h"
#include "iostream"
#include "random"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "algorithm"

void World::init(Input* input, int screenWidth, int screenHeight)
{
	this->input = input;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

void World::updateCells()
{
	if (input->keyPressed[GLFW_KEY_1])
	{
		brush = { SAND, SOLID_MOVABLE, 40.0f, 3 , false };
	}
	else if (input->keyPressed[GLFW_KEY_2])
	{
		brush = { WATER, FLUID, 40.0f, 3 , false };
	}
	else if (input->keyPressed[GLFW_KEY_3])
	{
		brush = { WOOD, SOLID_IMMOVABLE, 60.0f, 0, true };
	}
	else if (input->keyPressed[GLFW_KEY_BACKSPACE])
	{
		brush = { AIR, FLUID, 60.0f, 0, true };
	}
	else if (input->keyPressed[GLFW_KEY_4])
	{
		brush = { FIRE, REACTION, 60.0f, 0, true };
	}
	else if (input->keyPressed[GLFW_KEY_5])
	{
		brush = { STONE, SOLID_IMMOVABLE, 60.0f, 0, true };
	}
	else if (input->keyPressed[GLFW_KEY_6])
	{
		brush = { GUNPOWDER, SOLID_MOVABLE, 40.0f, 3, false };
	}
	else if (input->keyPressed[GLFW_KEY_7])
	{
		brush = { SMOKE, GAS, 40.0f, 3, false };
	}
	else if (input->keyPressed[GLFW_KEY_8])
	{
		brush = { STEAM, GAS, 40.0f, 3, false };
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
			if (grid.get(i, j).isUpdated)
				continue;

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
			case FIRE:
				updateFire(i, j);
				break;
			case STONE:
				updateStone(i, j);
				break;
			case GUNPOWDER:
				updateGunpowder(i, j);
				break;
			case SMOKE:
				updateSmoke(i, j);
				break;
			case STEAM:
				updateSteam(i, j);
				break;
			}

			grid.get(i, j).isUpdated = true;
		}
	}

	for (int i = grid.size - 1; i >= 0; i--)
	{
		for (int j = grid.size - 1; j >= 0; j--)
		{
			CellData& cell = grid.get(i, j);
			cell.isUpdated = false;
			
			// we can write different simulations here and just also update the texture that is rendered to the screen.
			// Like, we can have a separate particle simulation that tracks the cells' velocity and gravity. 
			// take a cell --> put it in particle simulation with velocity and gravity and make it fly into the air --> once it hits something
			// take it back to the sand simulation.
			
			grid.setCellTextureColor(i, j, cell.color);
		}
	}
}

void World::updateSand(int x, int y)
{
	CellData currentCell = grid.get(x, y);
	if (y < grid.size - 1)
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
	case FIRE:
		createFire(x, y);
		break;
	case STONE:
		createStone(x, y);
		break;
	case GUNPOWDER:
		createGunpowder(x, y);
		break;
	case SMOKE:
		createSmoke(x, y);
		break;
	case STEAM:
		createSteam(x, y);
		break;
	}
}

void World::spawnCells()
{
	static double lastTime = 0;

	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastTime;

	static int lastCircleCenterX;
	static int lastCircleCenterY;

	static CellType lastBrushType;
	
	float relativeX = input->mouseX / float(screenWidth);
	float relativeY = input->mouseY / float(screenHeight);

	int centerX = (int)(relativeX * grid.size);
	int centerY = (int)(relativeY * grid.size);

	if (!brush.continuous && deltaTime >= 1 / brush.spawnRate)
	{
		brushDraw(centerX, centerY);
		lastTime = currentTime;
		lastBrushType = brush.type;
	}
	else if (deltaTime >= 1 / brush.spawnRate && brush.continuous)
	{
		brushDraw(centerX, centerY);
		
		// fill gaps 
		if (mouseDownLastFrame && lastBrushType == brush.type)
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
					int x, y;
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

					brushDraw(centerX + x, centerY + y);
				}
			}

			lastTime = currentTime;
		}

		lastCircleCenterX = centerX;
		lastCircleCenterY = centerY;	

		lastBrushType = brush.type;
	}

	
}

void World::createSand(int x, int y)
{
	CellData sand =
	{
		.type = SAND,
		.kind = SOLID_MOVABLE,
		.color = types::color8{.r = 220, .g = 215, .b = 148 },
		.flammability = 0,
		.isUpdated = false
	};

	float f = types::genRandom(200, 255) / 255.0f;

	sand.color = sand.color * f;
	grid.set(x, y, sand);
}

void World::updateWater(int x, int y)
{
	int dispersion = 5;
	CellData currentCell = grid.get(x, y);
	if (y < grid.size - 1)
	{
		int randomOffset = types::genRandom(-1, 1);

		CellData cellBelow = grid.get(x, y + 1);
		CellData cellDiagonal = grid.get(x + randomOffset, y + 1);
		CellData cellSide = grid.get(x + randomOffset, y);

		if (cellBelow.type == AIR || cellBelow.kind == GAS)
		{
			grid.set(x, y, cellBelow);
			grid.set(x, y + 1, currentCell);
		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			if (cellDiagonal.type == AIR || cellDiagonal.kind == GAS)
			{
				// disperse diagonally 
				types::Pos lastAirCellPos = disperse(x, y, dispersion, randomOffset, 1);

				createAir(x, y);
				grid.set(lastAirCellPos.x, lastAirCellPos.y, currentCell);
			}
			else if (cellSide.type == AIR || cellSide.kind == GAS)
			{
				// disperse to the sides
				types::Pos lastAirCellPos = disperse(x, y, dispersion, randomOffset, 0);

				createAir(x, y);
				grid.set(lastAirCellPos.x, lastAirCellPos.y, currentCell);
				
			}
		}
	}
}

types::Pos World::disperse(int x, int y, int dispersion, int dirX, int dirY, bool stopAtNonEmpty)
{
	// iterate through the cells in the direction defined by dirX and dirY until there is no air or up to 
	// the value of dispersion, then swap with the last air cell found
	//CellData currentCell = grid.get(x, y);
	CellData cell;

	if(dirX > 0)
		dirX = dirX / abs(dirX);
	if(dirY > 0)
		dirY = dirY / abs(dirY);

	if (!stopAtNonEmpty)
	{
		if (!grid.isOutOfBounds(x + dirX * dispersion, y + dirY * dispersion))
		{
			return types::Pos{ x + dirX * dispersion, y + dirY * dispersion };
		}
	}

	types::Pos	 lastAirCellPos{0,0};
	int i = 1;
	while (i <= dispersion)
	{
		int xOffset = dirX * i;
		int yOffset = dirY * i;

		cell = grid.get(x + xOffset, y + yOffset);

		if (cell.type == AIR || cell.kind == GAS)
		{
			lastAirCellPos = types::Pos{ x + xOffset, y + yOffset };

			// if did not find non-air cells and reached max dispersion or end of the map, swap.
			if (i == dispersion || x + xOffset == grid.size - 1 || x + xOffset == 0 
				|| y + yOffset == grid.size - 1 || y + yOffset == 0)
			{	
				return types::Pos{ lastAirCellPos.x, lastAirCellPos.y };
				break;
			}
		}
		else 
		{
			// found non-air cell: swap with last air cell.
			return types::Pos{ lastAirCellPos.x, lastAirCellPos.y };
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
			if (!(types::genRandom(0, brush.sparsity) == 0))
				continue;

			if (distanceSquared <= radiusSquared + brushSize * 0.5f) {
				int gridX = centerX + i;
				int gridY = centerY + j;

				if (!grid.isOutOfBounds(gridX, gridY)) {

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
		.type = WATER,
		.kind = FLUID,
		.color = types::color8{ 43, 153, 235 },
		.flammability = 0,
		.isUpdated = false
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
		.type = WOOD,
		.kind = SOLID_IMMOVABLE,
		.color = types::color8{ 79, 42, 21, 100 },
		.flammability = 130,
		.combustsInto = SMOKE,
		.isUpdated = false
	};

	float f = types::genRandom(150, 255) / 255.0f;

	wood.color = wood.color * f;
	grid.set(x, y, wood);
}

void World::updateFire(int x, int y)
{
	if (y <= 0)
	{
		createAir(x, y);
		return;
	}
	
	CellData currentCell = grid.get(x, y);

	int dispersion = 1;
	int randomOffsetSides = types::genRandom(-1, 1);
	int randomOffsetUp = 1;

	CellData cellAbove = grid.get(x, y - randomOffsetUp);

	bool flammableNeighbor = false;
	
	bool isAirNeighbor = false;
	types::Pos airNeighborPos;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (!grid.isOutOfBounds(x + i, y + j))
			{
				CellData cell = grid.get(x + i, y + j);

				if (cell.type == AIR)
				{
					isAirNeighbor = true;

					airNeighborPos = { x + i, y + j };
				}
				else if (cell.flammability > 0)
				{
					flammableNeighbor = true;

					int destructionProb = std::round((float)cell.flammability / 255.0f * 5000);

					if (types::genRandom(destructionProb, 5000) == destructionProb)
					{	
						float f = types::genRandom(150, 255) / 255.0f;
						currentCell.color = types::color8{ 255, 86, 0 } * f;

						grid.set(x + i, y + j, currentCell);

						if(types::genRandom(0, 1) == 0)
							createCell(x, y, cell.combustsInto);
					}

					int emberProb = std::round((float)cell.flammability / 255.0f * 400);

					if (isAirNeighbor && types::genRandom(emberProb, 400) == emberProb)
					{
						// throw an ember to a random direction
						types::Pos randomPos = disperse(x, y, types::genRandom(1, 5), types::genRandom(-1, 1), types::genRandom(-1, 1), false);

						if(grid.get(randomPos.x, randomPos.y).flammability > 0)
							grid.set(randomPos.x, randomPos.y, currentCell);
					}
				}
				else if (cell.type == WATER)
				{
					if(types::genRandom(0, 10) == 0)
					{
						createSteam(x + i, y + j);
					}
					createAir(x, y);
					return;
				}
			}
		}
	}

	if ((!isAirNeighbor && !flammableNeighbor))
	{
		createAir(x, y);
		return;
	}
		
	if (cellAbove.type == AIR && types::genRandom(0, 20) == 0)
	{
		if (types::genRandom(0, 4) == 0)
		{
			grid.set(x, y - randomOffsetUp, currentCell);

		}
		else if (!flammableNeighbor)
		{
			createAir(x, y);
		}
	}
	else if (!grid.isOutOfBounds(x + randomOffsetSides, y - 1))
	{
		CellData cellDiagonal = grid.get(x + randomOffsetSides, y - 1);
		CellData cellSide = grid.get(x + randomOffsetSides, y);

		if (cellDiagonal.type == AIR && types::genRandom(0, 2) == 0)
		{
			// disperse diagonally
			types::Pos lastAirCellPos = disperse(x, y, dispersion, randomOffsetSides, -1);

			if (types::genRandom(0, 1) == 0 && !flammableNeighbor)
			{
				createAir(x, y);
			}
			else
			{
				grid.set(lastAirCellPos.x, lastAirCellPos.y, currentCell);
			}

		}
		else if (cellSide.type == AIR && types::genRandom(0, 4) == 0 || ((cellDiagonal.type != AIR && cellAbove.type != AIR && cellDiagonal.type != FIRE && cellAbove.type != FIRE)))
		{
			// disperse to the sides
			types::Pos lastAirCellPos = disperse(x, y, dispersion, randomOffsetSides, 0);

			if (types::genRandom(0, 1) == 0)
			{
				if(!flammableNeighbor)
					createAir(x, y);
			}
			else
			{
				grid.set(lastAirCellPos.x, lastAirCellPos.y, currentCell);
			}

		}
	}
}

void World::createFire(int x, int y)
{
	CellData fire =
	{
		.type = FIRE,
		.kind = REACTION,
		.color = types::color8{ 255, 86, 0 },
		.flammability = 0,
		.isUpdated = false
	};

	float f = types::genRandom(150, 255) / 255.0f;

	fire.color = fire.color * f;
	grid.set(x, y, fire);
}

void World::createStone(int x, int y)
{
	CellData stone =
	{
		.type = STONE,
		.kind = SOLID_IMMOVABLE,
		.color = types::color8{ 255, 255, 255 },
		.flammability = 0,
		.isUpdated = false
	};

	float f = types::genRandom(200, 255) / 255.0f;

	stone.color = stone.color * f;
	grid.set(x, y, stone);
}

void World::updateGunpowder(int x, int y)
{
	updateSand(x, y);
}

void World::createGunpowder(int x, int y)
{
	CellData gunpowder =
	{
		.type = GUNPOWDER,
		.kind = SOLID_MOVABLE,
		.color = types::color8{ 70, 70, 70 },
		.flammability = 254,
		.combustsInto = SMOKE,
		.isUpdated = false
	};

	float f = types::genRandom(150, 255) / 255.0f;

	gunpowder.color = gunpowder.color * f;
	grid.set(x, y, gunpowder);
}

void World::updateSmoke(int x, int y)
{
	CellData currentCell = grid.get(x, y);
	float f = types::genRandom(100, 255) / 255.0f;
	grid.get(x, y).color = types::color8{ 70, 70, 70 } * f;
	
	if (y > 0)
	{
		int randomOffset = types::genRandom(-1, 1);
		currentCell.isUpdated = true;
		
		CellData cellAbove = grid.get(x, y - 1);
	
		if ((cellAbove.kind == FLUID || cellAbove.kind == GAS) && types::genRandom(0, 20) == 0)
		{
			grid.set(x, y, cellAbove);
			grid.set(x, y - 1, currentCell);

		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			CellData cellDiagonal = grid.get(x + randomOffset, y - 1);
			CellData cellSide = grid.get(x + randomOffset, y);

			if ((cellDiagonal.kind == FLUID || cellDiagonal.kind == GAS) && types::genRandom(0, 10) == 0)
			{
				grid.set(x, y, cellDiagonal);
				grid.set(x + randomOffset, y - 1, currentCell);
			}
			else if ((cellSide.kind == FLUID || cellSide.kind == GAS) && types::genRandom(0, 2) == 0)
			{
				grid.set(x, y, cellSide);
				grid.set(x + randomOffset, y, currentCell);
			}
			else if (types::genRandom(0, 1000) == 0)
			{
				if (y < grid.size - 1)
				{
					CellData cellBelow = grid.get(x, y + 1);

					if (cellBelow.kind != GAS)
					{
						createAir(x, y);
					}
				}
			}
		}
	}
	else if (y == 0)
	{
		createAir(x, y);
	}

}

void World::createSmoke(int x, int y)
{
	CellData smoke =
	{
		.type = SMOKE,
		.kind = GAS,
		.color = types::color8{ 70, 70, 70 },
		.flammability = 0,
		.isUpdated = false
	};

	float f = types::genRandom(150, 255) / 255.0f;

	smoke.color = smoke.color * f;
	grid.set(x, y, smoke);
}

void World::updateSteam(int x, int y)
{
	CellData currentCell = grid.get(x, y);
	float f = types::genRandom(100, 255) / 255.0f;
	grid.get(x, y).color = types::color8{ 120, 120, 120 } *f;

	if (y > 0)
	{
		int randomOffset = types::genRandom(-1, 1);
		currentCell.isUpdated = true;

		// this is going out of bounds
		CellData cellAbove = grid.get(x, y - 1);
		
		if ((cellAbove.kind == FLUID || cellAbove.kind == GAS) && types::genRandom(0, 20) == 0)
		{
			grid.set(x, y, cellAbove);
			grid.set(x, y - 1, currentCell);

		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			CellData cellDiagonal = grid.get(x + randomOffset, y - 1);
			CellData cellSide = grid.get(x + randomOffset, y);

			if ((cellDiagonal.kind == FLUID || cellDiagonal.kind == GAS) && types::genRandom(0, 10) == 0)
			{
				grid.set(x, y, cellDiagonal);
				grid.set(x + randomOffset, y - 1, currentCell);
			}
			else if ((cellSide.kind == FLUID || cellSide.kind == GAS) && types::genRandom(0, 2) == 0)
			{
				grid.set(x, y, cellSide);
				grid.set(x + randomOffset, y, currentCell);
			}
			else if (types::genRandom(0, 10000) == 0)
			{
				if (y < grid.size - 1)
				{
					CellData cellBelow = grid.get(x, y + 1);

					if (cellBelow.kind != GAS && cellAbove.kind == GAS)
					{
						createWater(x, y);
					}
				}
			}
		}
	}
	else if (y == 0)
	{
		createAir(x, y);
	}
}

void World::createSteam(int x, int y)
{
	CellData steam =
	{
		.type = STEAM,
		.kind = GAS,
		.color = types::color8{ 120, 120, 120 },
		.flammability = 0,
		.isUpdated = false
	};

	float f = types::genRandom(150, 255) / 255.0f;

	steam.color = steam.color * f;
	grid.set(x, y, steam);
}


void World::updateStone(int x, int y)
{
}

void World::createAir(int x, int y)
{
	CellData air =
	{
		.type = AIR,
		.kind = FLUID,
		.color = types::color8{ 0, 0, 0, 0 },
		.flammability = 0,
		.isUpdated = false
	};

	grid.set(x, y, air);
}

