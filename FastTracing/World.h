#pragma once
#include "Chunk.h"

class World{
public:
	World() {};
	World(int x, int y, int z);
	World(const char* path);
	void setBlock(int x, int y, int z, int id);
	glm::ivec3 getSize();
	ui* getData();
	void loadFromHeightMap(const char* path);
	void loadFromHeightMapToFile(const char* heightmap_path, const char* file_path);
	void loadFromFile(const char* path);
	void saveToFile(const char* path);
	void init(int x, int y, int z);
	unsigned int getDataSize();
	std::vector<std::pair<int, int>> get_change_que();
	~World();
private:
	std::vector<std::pair<int, int>> change_que;
	std::string file_pth;
	int s_x,s_y,s_z;
	Chunk* chunks;
	ui* data;
	int count;
};

