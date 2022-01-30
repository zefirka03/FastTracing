#pragma once
#include "globalGL.h";
#include "World.h";

struct Ray {
	Ray() {};
	glm::vec3 rayOrig,
			  rayDir;
};

class CollisionInfo{
public:
	CollisionInfo() { side = 1; };
	bool DDA_blocks(Ray r, int cur_chunk, int cur_side, World &w);
	bool DDA_chunks(Ray r, World& w);
	~CollisionInfo() {};

	glm::vec3 collisionPoint;
	glm::vec3 voxel_pos;
	glm::vec3 normal;
	int side;
	int id;
};

