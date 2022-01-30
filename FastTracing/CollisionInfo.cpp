#include "CollisionInfo.h"

glm::vec3 getCoord(glm::vec3 pos, int layer) {
	return	glm::vec3(int((pos.x - int(pos.x / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer)),
		int((pos.y - int(pos.y / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer)),
		int((pos.z - int(pos.z / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer)));
}

glm::vec3 addCoord(glm::vec3 pos, glm::vec3 addr) {
	glm::vec3 re = pos + addr;
	return	glm::vec3(int(floor(re.x - int(pos.x) / CHUNK_SIZE * CHUNK_SIZE)),
		int(floor(re.y - int(pos.y) / CHUNK_SIZE * CHUNK_SIZE)),
		int(floor(re.z - int(pos.z) / CHUNK_SIZE * CHUNK_SIZE)));
}

bool ray_box_Intersec(Ray r, glm::vec3 p1, glm::vec3 p2, CollisionInfo& cl) {
	if (r.rayOrig.x >= p1.x && r.rayOrig.y >= p1.y && r.rayOrig.z >= p1.z &&
		r.rayOrig.x <= p2.x && r.rayOrig.y <= p2.y && r.rayOrig.z <= p2.z) {
		cl.collisionPoint = r.rayOrig;
		return true;
	}

	glm::vec3 A = p1,
		B = p2;

	glm::vec3 t0 = (A - r.rayOrig) / r.rayDir,
		t1 = (B - r.rayOrig) / r.rayDir;

	glm::vec3 tmin = min(t0, t1),
			tmax = max(t0, t1);

	float	mn = std::max(tmin.x, std::max(tmin.y, tmin.z)),
			mx = std::min(tmax.x, std::min(tmax.y, tmax.z));

	if (mx < 0) return false;
	if (mx >= mn) {
		cl.normal = -sign(r.rayDir) * glm::step(glm::vec3(tmin.y, tmin.z, tmin.x), glm::vec3(tmin.x, tmin.y, tmin.z)) * glm::step(glm::vec3(tmin.z, tmin.x, tmin.y), glm::vec3(tmin.x, tmin.y, tmin.z));
		cl.side = int(abs(cl.normal.x)) * 0 + int(abs(cl.normal.y)) * 1 + int(abs(cl.normal.z)) * 2;
		cl.collisionPoint = mn * r.rayDir + r.rayOrig + cl.normal * 0.001f;
	}
	return mx >= mn;
}

bool CollisionInfo::DDA_blocks(Ray r, int cur_chunk, int cur_side, World& w) {
	int layer = CHUNK_SIZE * BLOCK_SIZE;
	int he = 1;
	int layer_add = 0;


	float unit = BLOCK_SIZE * CHUNK_SIZE / layer;
	glm::vec3 startPoint = r.rayOrig;
	glm::vec3 curBlock = getCoord(startPoint, CHUNK_SIZE);
	glm::vec3 mapCoord = glm::vec3((int(startPoint.x / CHUNK_SIZE) * CHUNK_SIZE), (int(startPoint.y / CHUNK_SIZE) * CHUNK_SIZE), (int(startPoint.z / CHUNK_SIZE) * CHUNK_SIZE)) + getCoord(startPoint, layer) * unit;

	glm::vec3 rLen, dDist;

	int step[3];

	for (int i = 0; i < 3; ++i) {
		const float x = (r.rayDir.x / r.rayDir[i]);
		const float y = (r.rayDir.y / r.rayDir[i]);
		const float z = (r.rayDir.z / r.rayDir[i]);
		dDist[i] = sqrt(x * x + y * y + z * z) * unit;
		if (r.rayDir[i] < 0) {
			step[i] = -1;
			rLen[i] = (startPoint[i] - mapCoord[i]) / unit * dDist[i];
		}
		else {
			step[i] = 1;
			rLen[i] = ((mapCoord[i] - startPoint[i]) / unit + 1.f) * dDist[i];
		}
	}

	bool isIntersect = false;
	int side = cur_side;
	int it = 0;

	while (it < 200) {
		if (w.getData()[cur_chunk * 33353 + 585 + int(curBlock.x) + int(curBlock.y) * CHUNK_SIZE + int(curBlock.z) * CHUNK_SIZE * CHUNK_SIZE] > 0) {
			isIntersect = true;

			if (it > 0) rLen[side] -= dDist[side];
			else rLen[side] = 0;

			this->collisionPoint = startPoint + rLen[side] * r.rayDir;

			if (side == 0) this->normal = glm::vec3(-1 * step[side], 0, 0);
			else if (side == 1) this->normal = glm::vec3(0, -1 * step[side], 0);
			else if (side == 2) this->normal = glm::vec3(0, 0, -1 * step[side]);

			this->side = side;
			this->id = w.getData()[cur_chunk * 33353 + he + int(curBlock.x) + int(curBlock.y) * CHUNK_SIZE + int(curBlock.z) * CHUNK_SIZE * CHUNK_SIZE];
			this->voxel_pos = glm::vec3(((cur_chunk % (w.getSize().x * w.getSize().y)) % w.getSize().x)*32 + curBlock.x, ((cur_chunk % (w.getSize().x * w.getSize().y))/ w.getSize().x) * 32 + curBlock.y, (cur_chunk/(w.getSize().x * w.getSize().y)) * 32 + curBlock.z);
			break;
		}

		for (int i = 0; i < 3; ++i)
			if (rLen[side] > rLen[i] && r.rayDir[i] != 0)
				side = i;

		rLen[side] += dDist[side];
		curBlock[side] += step[side] * int(unit);


		if (curBlock.x < 0 || curBlock.y < 0 || curBlock.z < 0 || curBlock.x >= CHUNK_SIZE || curBlock.y >= CHUNK_SIZE || curBlock.z >= CHUNK_SIZE) break;
		it++;
	}

	return isIntersect;
}

bool CollisionInfo::DDA_chunks(Ray r, World& w) {
	float unit = BLOCK_SIZE * CHUNK_SIZE;

	glm::vec3 w_size = glm::vec3(w.getSize().x * unit, w.getSize().y * unit, w.getSize().z * unit);

	CollisionInfo cl1;
	if (ray_box_Intersec(r, glm::vec3(0), w_size, cl1)) {

		glm::vec3 startPoint = cl1.collisionPoint;

		glm::vec3 curChunk = glm::vec3(int(startPoint.x / unit), int(startPoint.y / unit), int(startPoint.z / unit));

		curChunk = max(curChunk, glm::vec3(0));
		curChunk = min(curChunk, glm::vec3(w.getSize().x - 1, w.getSize().y - 1, w.getSize().z - 1));

		glm::vec3 mapCoord = curChunk * unit;

		glm::vec3 rLen, dDist;

		int step[3];

		for (int i = 0; i < 3; ++i) {
			const float x = (r.rayDir.x / r.rayDir[i]);
			const float y = (r.rayDir.y / r.rayDir[i]);
			const float z = (r.rayDir.z / r.rayDir[i]);
			dDist[i] = sqrt(x * x + y * y + z * z) * unit;
			if (r.rayDir[i] < 0) {
				step[i] = -1;
				rLen[i] = (startPoint[i] - mapCoord[i]) / unit * dDist[i];
			}
			else {
				step[i] = 1;
				rLen[i] = ((mapCoord[i] - startPoint[i]) / unit + 1.f) * dDist[i];
			}
		}
		int side = cl1.side;
		bool isIntersect = false;
		int it = 0;

		while (it < 200) {
			if (w.getData()[int(curChunk.x + curChunk.y * w.getSize().x + curChunk.z * w.getSize().x * w.getSize().y) * 33353] > 0u) {
				if (it > 0) this->collisionPoint = startPoint + (rLen[side] - dDist[side]) * r.rayDir;
				else this->collisionPoint = startPoint;

				if (side == 0) this->normal = glm::vec3(-1 * step[side], 0, 0);
				else if (side == 1) this->normal = glm::vec3(0, -1 * step[side], 0);
				else if (side == 2) this->normal = glm::vec3(0, 0, -1 * step[side]);

				this->side = side;

				CollisionInfo clX;
				r.rayOrig = this->collisionPoint - this->normal * 0.002f;
				isIntersect = clX.DDA_blocks(r, int(curChunk.x + curChunk.y * w.getSize().x + curChunk.z * w.getSize().x * w.getSize().y), side, w);
				if (isIntersect) {
					*this = clX;
					break;
				}
			}

			for (int i = 0; i < 3; ++i)
				if (rLen[side] > rLen[i] && r.rayDir[i] != 0)
					side = i;
			rLen[side] += dDist[side];
			curChunk[side] += step[side];

			if (curChunk.x < 0 || curChunk.y < 0 || curChunk.z < 0 || curChunk.x >= w.getSize().x || curChunk.y >= w.getSize().y || curChunk.z >= w.getSize().z) break;
			it++;
		}

		return isIntersect;
	}
	return false;
}