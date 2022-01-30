#pragma once
#include "globalGL.h"

#define BLOCK_SIZE 1
#define CHUNK_SIZE 32


class Chunk{
public:
	Chunk();
	void setBlock(int x, int y, int z, int id);
	ui* getData();
	std::vector<std::pair<int, int>> get_change_que();
	~Chunk();
private:
	std::vector<std::pair<int, int>> change_que;
	ui* data;
};

