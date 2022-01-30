~~vertex~~
#version 430 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

out vec2 tx_cord;


void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    tx_cord = tex_coord;
}


~~fragment~~
#version 430 core

#define CHUNK_SIZE 32
#define BLOCK_SIZE 1
#define PI 3.1415926535

const float c_twopi = 2.0f * PI;

struct Camera {
	vec3	cameraPos,
		cameraFront,
		cameraUp,
		cameraRight;
	float fov, dist;
};

struct Ray {
	vec3 rayOrig,
		 rayDir;
};

struct CollisionInfo {
	vec3 collisionPoint;
	vec3 normal;
	int side;
};

bool hit_sphere(vec3 center, float radius, Ray r) {
	vec3 oc = r.rayOrig - center;
	float a = dot(r.rayDir, r.rayDir);
	float b = 2.0 * dot(oc, r.rayDir);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	return (discriminant > 0);
}

bool ray_box_Intersec(Ray r, vec3 p1, vec3 p2, inout CollisionInfo cl) {
	if (r.rayOrig.x >= p1.x && r.rayOrig.y >= p1.y && r.rayOrig.z >= p1.z &&
		r.rayOrig.x <= p2.x && r.rayOrig.y <= p2.y && r.rayOrig.z <= p2.z) {
		cl.collisionPoint = r.rayOrig;
		return true;
	}


	vec3 A = p1,
		B = p2;

	vec3 t0 = (A - r.rayOrig) / r.rayDir,
		t1 = (B - r.rayOrig) / r.rayDir;

	vec3 tmin = min(t0, t1),
		tmax = max(t0, t1);

	float	mn = max(tmin.x, max(tmin.y, tmin.z)),
		mx = min(tmax.x, min(tmax.y, tmax.z));

	if (mx < 0) return false;
	if (mx >= mn) {
		cl.normal = -sign(r.rayDir) * step(tmin.yzx, tmin.xyz) * step(tmin.zxy, tmin.xyz);
		cl.side = int(abs(cl.normal.x)) * 0 + int(abs(cl.normal.y)) * 1 + int(abs(cl.normal.z)) * 2;
		cl.collisionPoint = mn * r.rayDir + r.rayOrig + cl.normal*0.001f;
	}
	return mx >= mn;
}

//variables

layout(std430, binding = 2) buffer ColorSSBO {
	uint world[];
};

//uniform usamplerBuffer world;
out vec4 color;
uniform float tr;
uniform vec2 resolution;
uniform Camera cam;
uniform int sz_x, sz_y, sz_z;

vec3 getCoord(vec3 pos, int layer) {
	return	vec3(int((pos.x - int(pos.x / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer) ) ,
				 int((pos.y - int(pos.y / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer) ) ,
				 int((pos.z - int(pos.z / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer) ) );
}

bool DDA_blocks(Ray r, inout CollisionInfo cl, int cur_chunk, int cur_side) {
	int layer = CHUNK_SIZE * BLOCK_SIZE;
	int he = 1;
	int layer_add = 0;


	float unit = BLOCK_SIZE * CHUNK_SIZE / layer;
	vec3 startPoint = r.rayOrig;
	vec3 curBlock = getCoord(startPoint, CHUNK_SIZE);
	vec3 mapCoord = vec3((int(startPoint.x / CHUNK_SIZE) * CHUNK_SIZE), (int(startPoint.y / CHUNK_SIZE) * CHUNK_SIZE), (int(startPoint.z / CHUNK_SIZE) * CHUNK_SIZE)) + getCoord(startPoint, layer) * unit;

	vec3 rLen, dDist;

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
		if (world[cur_chunk * 33353 + 585 + int(curBlock.x) + int(curBlock.y) * CHUNK_SIZE + int(curBlock.z) * CHUNK_SIZE * CHUNK_SIZE] > 0) {
			isIntersect = true;

			if (it > 0) rLen[side] -= dDist[side];
			else rLen[side] = 0;

			cl.collisionPoint = startPoint + rLen[side] * r.rayDir;

				 if (side == 0) cl.normal = vec3(-1, 0, 0) * step[side];
			else if (side == 1) cl.normal = vec3(0, -1, 0) * step[side];
			else if (side == 2) cl.normal = vec3(0, 0, -1) * step[side];

			cl.side = side;
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

vec3 RandomHemispherePoint(vec2 rand)
{
	float cosTheta = sqrt(1.0 - rand.x);
	float sinTheta = sqrt(rand.x);
	float phi = 2.0 * PI * rand.y;
	return vec3(
		cos(phi) * sinTheta,
		sin(phi) * sinTheta,
		cosTheta
	);
}
uint wang_hash(inout uint seed)
{
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);
	return seed;
}
float RandomFloat01(inout uint state)
{
	return float(wang_hash(state)) / 4294967296.0;
}
vec3 RandomUnitVector(inout uint state)
{
	float z = RandomFloat01(state) * 2.0f - 1.0f;
	float a = RandomFloat01(state) * c_twopi;
	float r = sqrt(1.0f - z * z);
	float x = r * cos(a);
	float y = r * sin(a);
	return vec3(x, y, z);
}
vec3 NormalOrientedHemispherePoint(vec2 rand, vec3 n)
{
	vec3 v = RandomHemispherePoint(rand);
	return dot(v, n) < 0.0 ? -v : v;
}
float rand(vec2 n) {
	return fract(sin(dot(n.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void reflectR(inout vec3 r, vec3 norm) {
	r = normalize(vec3(-2.f) * dot(r, norm) * norm + r);
}

void G_Reflect(inout vec3 r, float seed, vec3 norm, float reflectance) {
	reflectR(r, norm);
	r = normalize(r * reflectance + (1.f - reflectance) * NormalOrientedHemispherePoint(vec2(rand(vec2(gl_FragCoord.x + seed + tr * r.x * r.z, gl_FragCoord.y + seed + tr * r.x * r.y)),
		rand(vec2(gl_FragCoord.y + seed + tr * r.z * r.y, gl_FragCoord.x + seed + tr * r.x * r.x))), norm));
}

void diffuse(inout vec3 r, float seed, vec3 norm) {
	r = NormalOrientedHemispherePoint(vec2(rand(vec2(gl_FragCoord.x + seed + tr * r.x * r.z, gl_FragCoord.y + seed + tr * r.x * r.y)),
		rand(vec2(gl_FragCoord.y + seed + tr * r.z * r.y, gl_FragCoord.x + seed + tr * r.z * r.x))), norm);
}



bool DDA_chunks(Ray r, inout CollisionInfo cl) {
	float unit = BLOCK_SIZE * CHUNK_SIZE;

	vec3 w_size = vec3(sz_x * unit, sz_y * unit, sz_z * unit);

	CollisionInfo cl1;
	if (ray_box_Intersec(r, vec3(0), w_size, cl1)) {

		vec3 startPoint = cl1.collisionPoint;

		vec3 curChunk = vec3(int(startPoint.x / unit), int(startPoint.y / unit), int(startPoint.z / unit));

		curChunk = max(curChunk, uvec3(0));
		curChunk = min(curChunk, uvec3(sz_x-1, sz_y-1, sz_z-1));

		vec3 mapCoord = curChunk * unit;
		
		vec3 rLen, dDist;

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
			if (world[int(curChunk.x + curChunk.y * sz_x + curChunk.z * sz_x * sz_y) * 33353] > 0) {
				if (it > 0) cl.collisionPoint = startPoint + (rLen[side]-dDist[side]) * r.rayDir;
				else cl.collisionPoint = startPoint;

				if (side == 0) cl.normal = vec3(-1, 0, 0) * step[side];
				else if (side == 1) cl.normal = vec3(0, -1, 0) * step[side];
				else if (side == 2) cl.normal = vec3(0, 0, -1) * step[side];

				cl.side = side;

				CollisionInfo clX;
				r.rayOrig = cl.collisionPoint - cl.normal * 0.002f;
				isIntersect = DDA_blocks(r, clX, int(curChunk.x + curChunk.y * sz_x + curChunk.z * sz_x * sz_y), side);
				if (isIntersect) {
					cl = clX;
					break;
				}
			}	

			for (int i = 0; i < 3; ++i)
				if (rLen[side] > rLen[i] && r.rayDir[i] != 0)
					side = i;
			rLen[side] += dDist[side];
			curChunk[side] += step[side];

			if (curChunk.x < 0 || curChunk.y < 0 || curChunk.z < 0 || curChunk.x >= sz_x || curChunk.y >= sz_y || curChunk.z >= sz_z) break;
			it++;
		}
		
		return isIntersect;
	}
	return false;
}



void main() {
	vec3 GlobalLight = vec3(1500, 1500, 1500);

	float pix = (tan(cam.fov / 2.f) * cam.dist * 2) / resolution.x;
	Ray r, mid;

	r.rayOrig = cam.cameraPos;
	r.rayDir = normalize(cam.cameraFront * cam.dist
		+ vec3((gl_FragCoord.x - resolution.x / 2) * pix) * cam.cameraRight
		+ vec3((gl_FragCoord.y - resolution.y / 2) * pix) * cam.cameraUp);

	CollisionInfo cl1, cl2;
	uint tr1 = uint(uint(gl_FragCoord.x) * uint(1973) + uint(gl_FragCoord.y) * uint(9277) + uint(tr) * uint(26699)) | uint(1);

	vec4 clr = vec4(1);
	if (DDA_chunks(r, cl1)) {
		//for (int i = 0; i < 6; i++) {
		//	r.rayOrig = cl1.collisionPoint + cl1.normal * 0.005f;
		//	diffuse(r.rayDir, RandomFloat01(tr1), cl1.normal);
		//	if (!DDA_chunks(r, cl1)) break;
		//	clr *= float(1) / float(2);
		//}
		//color = vec4(1) * clr;
		color = vec4( (1 +cl1.side) / 6.f);
	}
	else color = vec4(0.2, 0.2, 0.2, 1);

	//sphere in collisionPoint
	//mid.rayOrig = cam.cameraPos;
	//mid.rayDir = cam.cameraFront;
	//
	//if(DDA_chunks(mid, cl2))
	//	if (hit_sphere(cl2.collisionPoint, 0.1f, r)) color = vec4(1, 0, 0, 1);


	//UI
	if ((gl_FragCoord.x - int(resolution.x) / 2) * (gl_FragCoord.x - int(resolution.x) / 2) +
		(gl_FragCoord.y - int(resolution.y) / 2) * (gl_FragCoord.y - int(resolution.y) / 2) < 2) color = vec4(1, 1, 1, 1);
}
