#include "World.h"

World::World(int x, int y, int z) {
	this->init(x, y, z);
}

World::World(const char* path) {
	this->loadFromFile(path);
}


void World::init(int x, int y, int z) {
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

	this->init((wh + CHUNK_SIZE - 1) / CHUNK_SIZE, 8, (hh + CHUNK_SIZE - 1) / CHUNK_SIZE);
	printf("HeightMap loaded:\nChunk sizes: %dx8x%d", (wh + CHUNK_SIZE - 1) / CHUNK_SIZE, (hh + CHUNK_SIZE - 1) / CHUNK_SIZE);

	for (int i = 0; i < wh; i++) {
		for (int j = 0; j < hh; j++) {
			for (int h = 0; h < (int)image[i*hh + j]; h++) {
				int it = (i / CHUNK_SIZE) + s_x * (h / CHUNK_SIZE) + s_x * s_y * (j / CHUNK_SIZE);
				chunks[it].setBlock(i % CHUNK_SIZE, h % CHUNK_SIZE, j % CHUNK_SIZE, 1);
			}
			//int it = (i / CHUNK_SIZE) + s_x * ((int)image[i * hh + j] / CHUNK_SIZE) + s_x * s_y * (j / CHUNK_SIZE);
			//chunks[it].setBlock(i % CHUNK_SIZE, (int)image[i * hh + j] % CHUNK_SIZE, j % CHUNK_SIZE, 3);
		}
	}
	for(int i=0; i< this->s_x* this->s_y* this->s_z; i++)
		memcpy(&data[i * count], chunks[i].getData(), count * sizeof(ui));

	SOIL_free_image_data(image);
}

void World::loadFromHeightMapToFile(const char* heightmap_path, const char* file_path) {
	file_pth = new char[256];
	this->file_pth = file_path;

	this->loadFromHeightMap(heightmap_path);
	this->saveToFile(file_path);
}

void World::loadFromFile(const char* path) {
	FILE* file;
	this->file_pth = path;
	if (!fopen_s(&file, path, "rb")) {
		int sz_x, sz_y, sz_z;
		fscanf_s(file, "Chunks sizes:\nChunk x: %d\nChunk y: %d\nChunk z: %d\n", &sz_x, &sz_y, &sz_z);
		printf("Chunks sizes:\nChunk x: %d\nChunk y: %d\nChunk z: %d\n", sz_x, sz_y, sz_z);
		this->init(sz_x, sz_y, sz_z);
		fread(this->data, sizeof(ui), this->getDataSize(), file);
		for (int i = 0; i < sz_x * sz_y * sz_z; i++)
			memcpy(chunks[i].getData(), &this->data[i * count], count * sizeof(ui));
		fclose(file);
	}
	else {
		int size = 2;
		this->init(size, size, size);
		for (int i = 0; i < size * size * CHUNK_SIZE * CHUNK_SIZE; i++)
			this->setBlock(i / (size * CHUNK_SIZE), 0, i % (size * CHUNK_SIZE), 1);
		this->saveToFile(path);
	}
}


void World::saveToFile(const char* path) {
	FILE* file;
	fopen_s(&file, path, "wb");

	fprintf(file, "Chunks sizes:\n");
	fprintf(file, ("Chunk x: " + std::to_string(this->getSize().x) + "\n").c_str());
	fprintf(file, ("Chunk y: " + std::to_string(this->getSize().y) + "\n").c_str());
	fprintf(file, ("Chunk z: " + std::to_string(this->getSize().z) + "\n").c_str());

	fwrite(this->getData(), sizeof(ui), this->getDataSize(), file);

	printf("saved to file: %s\n", path);

	fclose(file);
}


World::~World() {
	if (!file_pth.empty()) 
		this->saveToFile(file_pth.c_str());
	delete[] chunks;
	delete[] data;
}