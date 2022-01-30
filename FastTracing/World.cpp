#include "World.h"

World::World(int x, int y, int z) {
	s_x = x, s_y = y, s_z = z;
	chunks = new Chunk[x * y * z];

	count = 0;
	for (int i = CHUNK_SIZE, h = 1; i > 2; i /= 2)
		count += h, h *= 8;
	count += CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

	data = new ui[count * x * y * z];

	for (int i = 0; i < count * x * y * z; i++)
		data[i] = 0;
	//printf("%d\n", count * x * y * z*sizeof(uc));
}

void World::setBlock(int x, int y, int z, int id) {
	int h = (x / CHUNK_SIZE) + s_x * (y / CHUNK_SIZE) + s_x * s_y * (z / CHUNK_SIZE);
	if (h >= s_x*s_y*s_z) {
		printf("World::setBlock cant be completed - out of range\n");
		return;
	}
	this->change_que.clear();
	chunks[h].setBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, id);
	auto itt = chunks[h].get_change_que();
	for (int i = 0; i < itt.size(); i++) {
		change_que.push_back(std::make_pair(h * count + itt[i].first, itt[i].second));
	}
	memcpy(&data[h * count], chunks[h].getData(), count * sizeof(ui));
}

glm::ivec3 World::getSize() {
	return glm::ivec3(s_x, s_y, s_z);
}

unsigned int World::getDataSize() {
	return count * s_x * s_y * s_z;
}

ui* World::getData() {
	return data;
}

std::vector<std::pair<int, int>> World::get_change_que() {
	return change_que;
}

void World::loadFromHeightMap(const char* path) {
	int wh, hh;
	unsigned char* image = SOIL_load_image(path, &wh, &hh, 0, SOIL_LOAD_L);

	for (int i = 0; i < wh; i++) {
		for (int j = 0; j < hh; j++) {
			for (int h = 0; h < (int)image[i*hh + j]; h++) {
				int it = (i / CHUNK_SIZE) + s_x * (h / CHUNK_SIZE) + s_x * s_y * (j / CHUNK_SIZE);
				chunks[it].setBlock(i % CHUNK_SIZE, h % CHUNK_SIZE, j % CHUNK_SIZE, 1);
			}
		}
	}
	for(int i=0; i< this->s_x* this->s_y* this->s_z; i++)
		memcpy(&data[i * count], chunks[i].getData(), count * sizeof(ui));

	SOIL_free_image_data(image);
}

World::~World() {
	delete[] chunks;
	delete[] data;
}