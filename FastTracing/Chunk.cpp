#include "Chunk.h"

Chunk::Chunk() {
	int count = 0;
	for (int i = CHUNK_SIZE, h = 1; i > 2; i /= 2)
		count += h, h *= 8;
	count += CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
	data = new ui[count];

	for (int i = 0; i < count; i++) data[i] = 0;
}

void Chunk::setBlock(int x, int y, int z, int id) {
	if (x > CHUNK_SIZE || y > CHUNK_SIZE || z > CHUNK_SIZE) {
		printf("Chunk::setBlock cant be completed - out of range\n");
		return;
	}

	change_que.clear();

	if (id) {
		int ps = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;

		int it = 0, h = 1;
		for (int i = CHUNK_SIZE; i > 2; i /= 2)
			it += h, h *= 8;
		
		if (!data[it + ps]) {
			data[it + ps] = id;
			change_que.push_back(std::make_pair(it + ps, id));

			h = 1, it = 0;
			for (int i = CHUNK_SIZE; i > 2; i /= 2) {
				int poss = it + (x / i) + (CHUNK_SIZE / i) * (y / i) + (CHUNK_SIZE / i) * (CHUNK_SIZE / i) * (z / i);
				ui cnt = data[poss] >> 8;
				ui date = data[poss] & 255u;
				cnt++;
				date |= 1u << (((x % i) / (i / 2)) + 2 * ((y % i) / (i / 2)) + 4 * ((z % i) / (i / 2)));

				data[poss] = (cnt << 8) | date;
				change_que.push_back(std::make_pair(poss, (cnt << 8) | date));

				it += h;
				h *= 8;
			}
		}
		else { 
			data[it + ps] = id; 
			change_que.push_back(std::make_pair(it + ps, id));
		}
	}
	else {
		int ps = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;

		int it = 0, h = 1;
		for (int i = CHUNK_SIZE; i > 2; i /= 2)
			it += h, h *= 8;

		if (data[it + ps]) {
			data[it + ps] = id;
			change_que.push_back(std::make_pair(it + ps, id));
			
			int tmp = 0;
			for (int j = 0; j < 8; j++)
				tmp += data[it + (x - x % 2 + j % 2) + (CHUNK_SIZE) * (y - y%2 + (j % 4) / 2) + (CHUNK_SIZE) * (CHUNK_SIZE) * (z - z%2 + j / 4)];

			if (!tmp) {
				h /= 8;
				it -= h;

				for (int i = 4; i <= CHUNK_SIZE; i *= 2) {
					int poss = it + (x / i) + (CHUNK_SIZE / i) * (y / i) + (CHUNK_SIZE / i) * (CHUNK_SIZE / i) * (z / i);
					ui cnt = data[poss] >> 8;
					ui date = data[poss] & 255u;
					date &= ~(1u << (((x % i) / (i / 2)) + 2 * ((y % i) / (i / 2)) + 4 * ((z % i) / (i / 2))));
					cnt--;

					data[poss] = (cnt << 8) | date;
					change_que.push_back(std::make_pair(poss, (cnt << 8) | date));

					if (data[poss]) break;
					h /= 8;
					it -= h;
				}
			}
		}
	}
}

std::vector<std::pair<int, int>> Chunk::get_change_que() { return change_que; }

ui* Chunk::getData() {
	return data;
}

Chunk::~Chunk() {
	//delete[] data;
}