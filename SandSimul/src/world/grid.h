#pragma once
#include <cstdint>
#include <vector>
#include "../util/types.h"

enum CellType : uint8_t
{
	AIR = 0,
	SAND = 1,
	WATER = 2
};

struct CellData
{
	CellType type = AIR;
	types::color8 color = { 255, 255, 255 };
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

	const int size = 300;
private:
	std::vector<CellData> cellGrid;
	std::vector<types::color8> cellTexture;
};