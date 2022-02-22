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

#define RANGE uvec4(65535u)
#define MAX_RANDOM float(RANGE-1u)

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
	vec2 uv;
	uint id;
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
		cl.collisionPoint = mn * r.rayDir + r.rayOrig + cl.normal * 0.001f;
	}
	return mx >= mn;
}

//variables

layout(std430, binding = 2) buffer ColorSSBO {
	uint world[];
};



out vec4 color;
uniform float tr;
uniform int curr_sample;
uniform vec2 resolution;
uniform Camera cam;
uniform float light_ratio;
uniform int sz_x, sz_y, sz_z;
uniform sampler2D texture_pack;

vec3 getCoord(vec3 pos, int layer) {
	return	vec3(int((pos.x - int(pos.x / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer) ) ,
				 int((pos.y - int(pos.y / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer) ) ,
				 int((pos.z - int(pos.z / CHUNK_SIZE) * CHUNK_SIZE) / (CHUNK_SIZE / layer) ) );
}

vec3 addCoord(vec3 pos, vec3 addr) {
	vec3 re = pos + addr;
	return	vec3(int(floor(re.x - int(pos.x) / CHUNK_SIZE * CHUNK_SIZE)),
		int(floor(re.y - int(pos.y) / CHUNK_SIZE * CHUNK_SIZE)),
		int(floor(re.z - int(pos.z) / CHUNK_SIZE * CHUNK_SIZE)));
}

void updateUV(inout CollisionInfo cl) {
	switch (cl.side) {
	case 0:
		cl.uv = vec2(1.f - 0.0000001f) - vec2(cl.collisionPoint.z - int(cl.collisionPoint.z), cl.collisionPoint.y - int(cl.collisionPoint.y));
		break;
	case 1:
		cl.uv = vec2(1.f - 0.0000001f) - vec2(cl.collisionPoint.x - int(cl.collisionPoint.x), cl.collisionPoint.z - int(cl.collisionPoint.z));
		break;
	case 2:
		cl.uv = vec2(1.f - 0.0000001f) - vec2(cl.collisionPoint.x - int(cl.collisionPoint.x), cl.collisionPoint.y - int(cl.collisionPoint.y));
		break;
	};
}


bool DDA_blocks(Ray r, inout CollisionInfo cl, int cur_chunk, int cur_side) {
	int he = 585;

	float unit = 1;
	vec3 startPoint = r.rayOrig;
	vec3 curBlock = getCoord(startPoint, CHUNK_SIZE);
	vec3 mapCoord = vec3(int(startPoint.x), int(startPoint.y), int(startPoint.z));

	vec3 rLen, dDist;

	int step[3];

	for (int i = 0; i < 3; ++i) {
		const float x = (r.rayDir.x / r.rayDir[i]);
		const float y = (r.rayDir.y / r.rayDir[i]);
		const float z = (r.rayDir.z / r.rayDir[i]);
		dDist[i] = sqrt(x * x + y * y + z * z);
		if (r.rayDir[i] < 0) {
			step[i] = -1;
			rLen[i] = (startPoint[i] - mapCoord[i]) * dDist[i];
		}
		else {
			step[i] = 1;
			rLen[i] = ((mapCoord[i] - startPoint[i]) + 1.f) * dDist[i];
		}
	}

	bool isIntersect = false;
	int side = cur_side;
	int it = 0, it1 = 0;

	uint density = world[cur_chunk * 33353] >> 8;

	while (it1 + it < 100) {
		if (world[cur_chunk * 33353 + he + int(curBlock.x) + int(curBlock.y) * CHUNK_SIZE + int(curBlock.z) * CHUNK_SIZE * CHUNK_SIZE] > 0) {
			isIntersect = true;

			if (it > 0) rLen[side] -= dDist[side];
			else rLen[side] = 0;

			cl.collisionPoint = startPoint + rLen[side] * r.rayDir;

				 if (side == 0) cl.normal = vec3(-1, 0, 0) * step[side];
			else if (side == 1) cl.normal = vec3(0, -1, 0) * step[side];
			else if (side == 2) cl.normal = vec3(0, 0, -1) * step[side];

			cl.side = side;
			cl.id = world[cur_chunk * 33353 + he + int(curBlock.x) + int(curBlock.y) * CHUNK_SIZE + int(curBlock.z) * CHUNK_SIZE * CHUNK_SIZE];
			updateUV(cl);
			break;
		}

		bool fl = false;
		uint l = 1u, lay = 0u;

		for (int j = 0; j < 4; l <<= 1, lay = (lay << 3u) + 1u, j++) {
			uint xx1 = CHUNK_SIZE / l, 
				 xx2 = xx1 >> 1u;
			if (((world[cur_chunk * 33353 + lay + uint(uint(curBlock.x) / xx1 + uint(curBlock.y) / xx1 * l + uint(curBlock.z) / xx1 * l * l)] & (1u << uint(uint(curBlock.x) % xx1 / xx2 + uint(curBlock.y) % xx1 / xx2 * 2 + uint(curBlock.z) % xx1 / xx2 * 4))) == 0)) {
				if (it > 0) rLen[side] -= dDist[side];
				else rLen[side] = 0;
				startPoint += rLen[side] * r.rayDir;

				vec3 A = vec3(int(startPoint.x) / xx2 * xx2, int(startPoint.y) / xx2 * xx2, int(startPoint.z) / xx2 * xx2),
					B = A + xx2;

				vec3 t0 = (A - startPoint) / r.rayDir;
				vec3 t1 = (B - startPoint) / r.rayDir;

				vec3 tm = max(t0, t1);

				for (int i = 0; i < 3; i++)
					if (tm[side] > tm[i]) side = i;

				if (side == 0) cl.normal = vec3(-1, 0, 0) * step[side];
				else if (side == 1) cl.normal = vec3(0, -1, 0) * step[side];
				else if (side == 2) cl.normal = vec3(0, 0, -1) * step[side];


				curBlock = addCoord(startPoint, r.rayDir * tm[side] - cl.normal * 0.001f);
				startPoint += r.rayDir * tm[side] - cl.normal * 0.001f;


				mapCoord = vec3(int(startPoint.x), int(startPoint.y), int(startPoint.z));
				for (int i = 0; i < 3; ++i) {
					if (r.rayDir[i] < 0) rLen[i] = (startPoint[i] - mapCoord[i]) * dDist[i];
					else 	rLen[i] = ((mapCoord[i] - startPoint[i]) + 1.f) * dDist[i];
				}

				it = 0;
				it1++;
				fl = true;
				break;
			}
		}
		
		if(!fl) {
			for (int i = 0; i < 3; ++i)
				if (rLen[side] > rLen[i] && r.rayDir[i] != 0)
					side = i;

			rLen[side] += dDist[side];
			curBlock[side] += step[side];
			it++;	
		}
		

		if (curBlock.x < 0 || curBlock.y < 0 || curBlock.z < 0 || curBlock.x >= CHUNK_SIZE || curBlock.y >= CHUNK_SIZE || curBlock.z >= CHUNK_SIZE) break;
	}

	return isIntersect;
}



bool DDA_chunks(Ray r, inout CollisionInfo cl) {
	float unit = BLOCK_SIZE * CHUNK_SIZE;

	vec3 w_size = vec3(sz_x * unit, sz_y * unit, sz_z * unit);

	CollisionInfo cl1, clX;
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

		while (it < 100) {
			if (world[int(curChunk.x + curChunk.y * sz_x + curChunk.z * sz_x * sz_y) * 33353] > 0) {
				if (it > 0) cl.collisionPoint = startPoint + (rLen[side] - dDist[side]) * r.rayDir;
				else cl.collisionPoint = startPoint;

				if (side == 0)		cl.normal = vec3(-1, 0, 0) * step[side];
				else if (side == 1) cl.normal = vec3(0, -1, 0) * step[side];
				else if (side == 2) cl.normal = vec3(0, 0, -1) * step[side];
				
				cl.side = side;

				
				r.rayOrig = cl.collisionPoint - cl.normal * 0.002f;
				
				isIntersect = DDA_blocks(r, clX, int(curChunk.x + curChunk.y * sz_x + curChunk.z * sz_x * sz_y), side);
				it++;
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

vec3 diffuseRayDir(inout Ray r, CollisionInfo cl, inout uint seed) {
	return normalize(cl.normal * 1.001f + RandomUnitVector(seed));
}

vec3 reflectRayDir(inout Ray r, CollisionInfo cl) {
	return normalize(r.rayDir - 2 * cl.normal * dot(r.rayDir, cl.normal));
}


const int count_of_blocks = 32;
const float G = 0.98f;

void main() {
	vec3 GlobalLight = vec3(6000, 10000, 6000);

	float pix = (tan(cam.fov / 2.f) * cam.dist * 2) / resolution.x;
	Ray r, mid;

	CollisionInfo cl1,cl2;

	vec3 clr = vec3(0.0), clor = vec3(0.0);
	vec3 throughput = vec3(1.0f, 1.0f, 1.0f);

	const int samples = 1;

	uint seed = uint(uint(gl_FragCoord.x) * uint(1973) + uint(gl_FragCoord.y) * uint(9277) + uint(curr_sample+tr) * uint(26699)) | uint(1);
	for (int j = 0; j < samples; j++) {
		clr = vec3(0.0);
		clor = vec3(0.0);
		throughput = vec3(1.0f, 1.0f, 1.0f);

		//anti-alsng
		vec2 jitter = vec2(RandomFloat01(seed), RandomFloat01(seed)) - 0.5f;

		r.rayOrig = cam.cameraPos;
		r.rayDir = normalize(cam.cameraFront * cam.dist
			+ vec3((gl_FragCoord.x + jitter.x - resolution.x / 2) * pix) * cam.cameraRight
			+ vec3((gl_FragCoord.y + jitter.y - resolution.y / 2) * pix) * cam.cameraUp);

		for (int i = 0; i < 8; ++i) {
			if (DDA_chunks(r, cl1)) {
				if (i == 0) color.a = length(r.rayOrig - cl1.collisionPoint);
				vec3 color_parameters = texture(texture_pack, vec2((cl1.uv.x + cl1.id - 1) / float(count_of_blocks), (cl1.uv.y + 1) / 2.f)).rgb;

				float roughness = color_parameters.r;
				float percentSpecular = color_parameters.g;
				int emission = int(color_parameters.b * 255);

				float doSpecular = (RandomFloat01(seed) < percentSpecular) ? 1.0f : 0.0f;

				throughput *= texture(texture_pack, vec2((cl1.uv.x + cl1.id - 1) / float(count_of_blocks), (cl1.uv.y + 0) / 2.f)).rgb;
				clr += emission * throughput;

				vec3 diffuseRayDir = diffuseRayDir(r, cl1, seed);
				vec3 specularRayDir = reflectRayDir(r, cl1);
				specularRayDir = normalize(mix(specularRayDir, diffuseRayDir, roughness * roughness));

				r.rayOrig = cl1.collisionPoint + cl1.normal * 0.003f;
				r.rayDir = mix(diffuseRayDir, specularRayDir, doSpecular);

				//Ray sunRay;
				//CollisionInfo CI_forSun;
				//sunRay.rayDir = normalize(GlobalLight - r.rayDir + RandomUnitVector(seed));
				//sunRay.rayOrig = r.rayOrig;
				//
				//if (!DDA_chunks(sunRay, CI_forSun)) {
				//	clr += vec4(1.0, 0.85, 0.75, 1.0) * light_ratio * vec4(throughput, 1);
				//	if (roughness < 1)break;
				//}
			}
			else {
				if (i == 0) color.a = -1;
				float tt = 0.5 * (r.rayDir.y + 1.0);
				clr += mix(vec3(1.0), vec3(0.5, 0.7, 1.0), tt) * light_ratio * throughput;
				//clr += mix(vec4(1.0, 0.6, 0.3, 1.0), vec4(0.5, 0.7, 1.0), tt) * light_ratio * vec4(throughput,1);
				break;
			}
		}
		clor += clr;
	}
	
	color = vec4(clor / float(samples), color.a);

	
	//sphere in collisionPoint
	//mid.rayOrig = cam.cameraPos;
	//mid.rayDir = cam.cameraFront;
	//
	//if(DDA_chunks(mid, cl2))
	//	if (hit_sphere(cl2.collisionPoint, 0.1f, r)) color = vec4(1, 0, 0, 1);


	//UI
	if ((gl_FragCoord.x - int(resolution.x) / 2) * (gl_FragCoord.x - int(resolution.x) / 2) +
		(gl_FragCoord.y - int(resolution.y) / 2) * (gl_FragCoord.y - int(resolution.y) / 2) < 3) color = vec4(1, 1, 1, 1);
}
