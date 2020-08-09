#include "blocks.h"

std::vector<GLfloat> Blocks::getTextureCoords(BLOCK id, Cube::CubeFace face, gl::TextureAtlas& atlas)
{
    glm::ivec2 coords;
    switch (id)
    {
    case GRASS:
        if (face == Cube::CubeFace::TOP)
            coords = { 0, 0 };
        else if (face == Cube::CubeFace::BOTTOM)
            coords = { 2, 0 };
        else
            coords = { 1, 0 };
        break;

    case DIRT:
        coords = { 2, 0 };
        break;

    case STONE:
        coords = { 3, 0 };
        break;

    case LOG:
        if (face == Cube::CubeFace::TOP || face == Cube::CubeFace::BOTTOM)
            coords = { 4, 0 };
        else
            coords = { 5, 0 };
        break;

    case LEAF:
        coords = { 6,0 };
        break;

    case PLANKS:
        coords = { 7, 0 };
        break;

    case WATER:
        coords = { 0, 1 };
        break;

    // if no cases are valid return error texture coords
    default:
        coords = { 7, 7 };
    }

    return atlas.getTextureCoords(coords);
}
