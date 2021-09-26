#pragma once
#include "../gl/glObjects.h"
#include "chunk.h"

#define WATER_LEVEL 34

class ChunkManager
{
public:
    ChunkManager();

    void generateChunks(int x, int y, int z);

    void setChunkSize(int x, int y, int z);

    int  getBlockGlobal(int x, int y, int z);
    void setBlockGlobal(int x, int y, int z, int blockid);

    Chunk* getChunkFromGlobal(int x, int y, int z);

    void generateFlatTerrain(int minAmp);
    void generateTerrain(int minAmp, int maxAmp);

    gl::TextureAtlas        atlas;

    std::vector<ChunkRef>   chunks;
    glm::vec3               worldSize;
    glm::uvec3              chunkSize;

private:
    int IndexFrom3D(int x, int y, int z);
    bool ChunkOutOfBounds(int x, int y, int z);
};