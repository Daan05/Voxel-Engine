#pragma once
#include "glm/glm.hpp"

#include <iostream>
#include <vector>

struct Voxel
{
	Voxel(glm::uvec3 _pos, bool _isSolid);

	glm::uvec3 pos;
	bool isSolid;
};

struct Chunk
{
	Chunk(glm::ivec3 cPos, int width, int height, int depth);
	Chunk(glm::ivec3 cPos, int size);
	~Chunk();

	int width, height, depth;

	glm::ivec3 pos; // ???
	std::vector<Voxel> voxels; // rename? // change to other data type for more speed?
};

