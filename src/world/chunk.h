#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../gl/glObjects.h"
#include "../util/entity.h"

enum NEIGHBOUR
{
    NORTH   = 0,
    SOUTH   = 1,
    EAST    = 2,
    WEST    = 3,
    ABOVE   = 4,
    BELOW   = 5
};

class Chunk
{
public:
    Chunk(glm::vec3 position, glm::uvec3 size, gl::TextureAtlas* atlas);

    void setBlockLocal(int x, int y, int z, int blockid);
    int  getBlockLocal(int x, int y, int z);

    void Update();
    void UpdateNeighbours();

    void setNeighbour(NEIGHBOUR n, Chunk* c);

    Entity chunk;
private:
    std::vector<uint8_t>    m_blocks;
    glm::uvec3              m_size;

    Chunk*                  m_neighbours[6];

    gl::TextureAtlas*       m_atlas;

    void generateMesh();

    int index3d(int x, int y, int z);
};

typedef std::shared_ptr<Chunk> ChunkRef;