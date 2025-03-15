#pragma once
#include <cstdint>
#include <vector>

enum CellType : uint8_t
{
	AIR = 0,
	SAND = 1
};

struct CellData
{
	CellType type = AIR;
	uint8_t color = 255;
	uint8_t padding1 = 0;
	uint8_t padding2 = 0;
};

class Grid
{
public:
	Grid(int size);

	int size;

	CellData& get(int x, int y);
	void set(int x, int y, CellData data);

	std::vector<CellData> getGrid();

private:

	std::vector<CellData> grid;

};