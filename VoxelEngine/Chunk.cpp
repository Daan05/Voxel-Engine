#include "Chunk.h"

Voxel::Voxel(glm::uvec3 _pos, bool _isSolid)
{
	pos = _pos;
	isSolid = _isSolid;
}

Chunk::Chunk(glm::ivec3 cPos, int _width, int _height, int _depth)
	: pos(cPos), width(_width), height(_height), depth(_depth)
{
	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				const unsigned w = 8 * sizeof(unsigned);
				const unsigned s = w / 2;
				unsigned a = i, b = j, c = k;
				a *= 3284157443;

				c ^= a << s | a >> w - s;
				c *= 1532907213;

				b ^= c << s | c >> w - s;
				b *= 1911520717;

				a ^= b << s | b >> w - s;
				a *= 2048419325;
				float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

				voxels.push_back(Voxel(glm::ivec3(k, j, i), random > 3.1415));
			}
		}
	}
}

Chunk::Chunk(glm::ivec3 cPos, int size)
	: pos(cPos), width(size), height(size), depth(size)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < size; k++)
			{
				const unsigned w = 8 * sizeof(unsigned);
				const unsigned s = w / 2;
				unsigned a = i, b = j, c = k;
				a *= 3284157443;

				c ^= a << s | a >> w - s;
				c *= 1532907213;

				b ^= c << s | c >> w - s;
				b *= 1911520717;

				a ^= b << s | b >> w - s;
				a *= 2048419325;
				float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

				voxels.push_back(Voxel(glm::ivec3(k, j, i), random > 3.1415));
			}
		}
	}
}

Chunk::~Chunk()
{

}
