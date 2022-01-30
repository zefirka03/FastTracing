#pragma once
#include "Chunk.h"

class World{
public:
	World(int x, int y, int z);
	void setBlock(int x, int y, int z, int id);
	glm::ivec3 getSize();
	ui* getData();
	void loadFromHeightMap(const char* path);
	unsigned int getDataSize();
	std::vector<std::pair<int, int>> get_change_que();
	~World();
private:
	std::vector<std::pair<int, int>> change_que;
	int s_x,s_y,s_z;
	Chunk* chunks;
	int count;
	ui* data;
};

