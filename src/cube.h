#ifndef CUBE_H
#define CUBE_H
#include <vector>
#include "glad/glad.h"

namespace Cube
{
    struct Face
    {
        std::vector<GLfloat> verticies;
        std::vector<GLfloat> normals;

        // Indicies and textureCoords of each face are the same
        std::vector<GLfloat> indicies = {
            0,1,3,
            3,1,2
        };

        std::vector<GLfloat> textureCoords = {
            0,0,
            0,1,
            1,1,
            1,0
        };;
    };

    enum class CubeFace
    {
        TOP, BOTTOM, LEFT, RIGHT, BACK, FRONT
    };

    Face getCubeFace(CubeFace face);

    const std::vector<GLfloat> verticies = {
        // Back face
        1,1,0,
        1,0,0,
        0,0,0,
        0,1,0,

        // Front face
        0,1,1,
        0,0,1,
        1,0,1,
        1,1,1,

        // Right face
        1,1,1,
        1,0,1,
        1,0,0,
        1,1,0,

        // Left Face
        0,1,0,
        0,0,0,
        0,0,1,
        0,1,1,

        // Top face
        0,1,1,
        1,1,1,
        1,1,0,
        0,1,0,

        // Bottom face
        0,0,1,
        0,0,0,
        1,0,0,
        1,0,1
    };

    const std::vector<GLfloat> textureCoords = {
         0,0,
         0,1,
         1,1,
         1,0,

         0,0,
         0,1,
         1,1,
         1,0,

         0,0,
         0,1,
         1,1,
         1,0,

         0,0,
         0,1,
         1,1,
         1,0,

         0,0,
         0,1,
         1,1,
         1,0,

         0,0,
         0,1,
         1,1,
         1,0
    };

    const std::vector<GLuint> indicies = {
         0,1,3,
         3,1,2,
         4,5,7,
         7,5,6,
         8,9,11,
         11,9,10,
         12,13,15,
         15,13,14,
         16,17,19,
         19,17,18,
         20,21,23,
         23,21,22
    };
};

#endif