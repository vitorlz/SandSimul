#pragma once
#include <cstdint>
#include <vector>
#include "../util/types.h"

enum CellType : uint8_t
{
	AIR = 0,
	SAND,
	WATER,
	WOOD, 
	FIRE,
	STONE,
	GUNPOWDER,
	SMOKE,
	STEAM,
	LIGHT
};

enum CellKind : uint8_t
{
	FLUID = 0,
	SOLID_IMMOVABLE,
	SOLID_MOVABLE,
	REACTION,
	GAS
};

struct CellData
{
	CellType type = AIR;
	CellKind kind = FLUID;
	types::color8 color = { 0, 0, 0, 0 };
	uint8_t flammability = 0;
	CellType combustsInto = AIR;
	bool isUpdated = false;
};

class Grid
{
public:
	Grid();
	CellData& get(int x, int y);
	void set(int x, int y, CellData data);
	std::vector<CellData>& getGrid();
	std::vector<types::color8>& getCellTexture();

	void setCellTextureColor(int x, int y, types::color8 color);

	bool isOutOfBounds(int x, int y);

	const int size = 300;
private:
	std::vector<CellData> cellGrid;
	std::vector<types::color8> cellTexture;
};