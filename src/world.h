#ifndef WORLD_H
#define WORLD_H
#include <cstdint>
#include <memory>
#include "entity.h"

#define CHUNK_SIZE 32
#define WATER_LEVEL 34

enum NEIGHBOUR
{
    NORTH = 0,
    SOUTH = 1,
    EAST  = 2,
    WEST  = 3,
    ABOVE = 4,
    BELOW = 5
};

class Chunk
{
public:
    Chunk(glm::vec3 position, gl::TextureAtlas* atlas);

    void setBlockLocal(int x, int y, int z, int blockid);
    int  getBlockLocal(int x, int y, int z);

    void Update();
    void UpdateNeighbours();

    void setNeighbour(NEIGHBOUR n, Chunk* c);

    Entity chunk;
private:
    std::uint8_t m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    Chunk* m_neighbours[6];

    void generateMesh();

    gl::TextureAtlas* m_atlas;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::unique_ptr<Chunk> ChunkRef;

class ChunkManager
{
public:
    ChunkManager();

    void generateChunks(int x, int y, int z);

    int  getBlockGlobal(int x, int y, int z);
    void setBlockGlobal(int x, int y, int z, int blockid);

    Chunk* getChunkFromGlobal(int x, int y, int z);

    void generateFlatTerrain(int minAmp);
    void generateTerrain(int minAmp, int maxAmp);

    gl::TextureAtlas atlas;

    std::vector<ChunkRef> chunks;
    glm::vec3 chunkSize;

private:
    int IndexFrom3D(int x, int y, int z);
    bool ChunkOutOfBounds(int x, int y, int z);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif