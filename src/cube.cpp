#include "cube.h"
using namespace Cube;

Face Cube::getCubeFace(CubeFace face)
{
    if (face == CubeFace::BACK)
        return Face{
            {   // Verticies
                1,1,0,
                1,0,0,
                0,0,0,
                0,1,0
            },
            {   // Normals
                0,0,-1
            }
    };
    else if (face == CubeFace::FRONT)
        return Face{
            {
                0,1,1,
                0,0,1,
                1,0,1,
                1,1,1
            },
            {
                0,0,1
            }
    };
    else if (face == CubeFace::RIGHT)
        return Face{
            {
                1,1,1,
                1,0,1,
                1,0,0,
                1,1,0
            },
            {
                1,0,0
            }
    };
    else if (face == CubeFace::LEFT)
        return Face{
            {
                0,1,0,
                0,0,0,
                0,0,1,
                0,1,1
            },
            {
                -1,0,0
            }
    };
    else if (face == CubeFace::TOP)
        return Face{
            {
                0,1,1,
                1,1,1,
                1,1,0,
                0,1,0
            },
            {
                0,1,0
            }
    };
    else if (face == CubeFace::BOTTOM)
        return Face{
            {
                0,0,1,
                0,0,0,
                1,0,0,
                1,0,1
            },
            {
                0,-1,0
            }
    };

    return Face();
}