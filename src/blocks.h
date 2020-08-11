#ifndef BLOCKS_H
#define BLOCKS_H

#include "gl/glObjects.h"
#include "cube.h"

class Blocks
{
public:
    enum BLOCK
    {
        AIR     = 0,
        DIRT    = 1,
        GRASS   = 2,
        STONE   = 3,
        LOG     = 4,
        LEAF    = 5,
        PLANKS  = 6,
        SAND    = 7,
        WATER   = 8
    };

    static std::vector<GLfloat> getTextureCoords(BLOCK id, Cube::CubeFace face, gl::TextureAtlas& atlas);
};

#endif