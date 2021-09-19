#include "chunk.h"

#include "blocks.h"

Chunk::Chunk(glm::vec3 position, glm::uvec3 size, gl::TextureAtlas* atlas)
    : m_atlas(atlas)
{
    chunk.position = position;
    m_size = size;

    for (int i = 0; i < 6; i++)
        m_neighbours[i] = nullptr;

    // By default the chunk is empty with AIR blocks
    for (int i = 0; i < (int)(size.x * size.y * size.z); i++)
        m_blocks.push_back(Blocks::AIR);
}

void Chunk::setBlockLocal(int x, int y, int z, int blockid)
{
    if (x < 0 || x >= (int)m_size.x || y < 0 || y >= (int)m_size.y || z < 0 || z >= (int)m_size.z)
    {
    #ifdef DEBUG
            printf("Set out of bounds block!\n");
    #endif
        return;
    }
    else
        m_blocks[index3d(x, y, z)] = blockid;
}

int Chunk::getBlockLocal(int x, int y, int z)
{
    if (x < 0 || x >= (int)m_size.x || y < 0 || y >= (int)m_size.y || z < 0 || z >= (int)m_size.z)
    {
    #ifdef DEBUG
            printf("Got out of bounds block!\n");
    #endif
        return -1;
    }
    else
        return m_blocks[index3d(x, y, z)];
}

void Chunk::Update()
{
    generateMesh();
}

void Chunk::UpdateNeighbours()
{
    // Update all of the neighbours
    for (int i = 0; i < 6; i++)
        if (m_neighbours[i] != nullptr)
            m_neighbours[i]->Update();
}

void Chunk::setNeighbour(NEIGHBOUR n, Chunk * c)
{
    m_neighbours[n] = c;
}

void Chunk::generateMesh()
{
    std::vector<GLfloat> temp_verticies;
    std::vector<GLfloat> temp_textureCoords;
    std::vector<GLuint>  temp_indicies;

    int indicies = 0;
    auto createFace = [&](Cube::CubeFace cubeface, int x, int y, int z)
    {
        auto face = Cube::getCubeFace(cubeface);

        for (int i = 0; i < (int)face.verticies.size() / 3; i++)
        {
            temp_verticies.push_back(face.verticies[0 + i * 3] + x);
            temp_verticies.push_back(face.verticies[1 + i * 3] + y);
            temp_verticies.push_back(face.verticies[2 + i * 3] + z);
        }

        auto texCoords = Blocks::getTextureCoords((Blocks::BLOCK)m_blocks[index3d(x, y, z)], cubeface, *m_atlas);
        temp_textureCoords.insert(temp_textureCoords.end(), texCoords.begin(), texCoords.end());

        temp_indicies.push_back(indicies + 0);
        temp_indicies.push_back(indicies + 1);
        temp_indicies.push_back(indicies + 3);
        temp_indicies.push_back(indicies + 3);
        temp_indicies.push_back(indicies + 1);
        temp_indicies.push_back(indicies + 2);
        indicies += 4;
    };

    for (int x = 0; x < (int)m_size.x; x++)
        for (int y = 0; y < (int)m_size.y; y++)
            for (int z = 0; z < (int)m_size.z; z++)
            {
                bool left = false, right = false, front = false, back = false, top = false, bottom = false;
                if (x != 0) left = true;
                if (y != 0) bottom = true;
                if (z != 0) back = true;
                if (x != m_size.x - 1) right = true;
                if (y != m_size.y - 1) top = true;
                if (z != m_size.z - 1) front = true;

                if (m_blocks[index3d(x, y, z)] == 0)
                    continue;

                if (left)
                    if (m_blocks[index3d(x - 1, y, z)] == 0) createFace(Cube::CubeFace::LEFT, x, y, z);
                if (right)
                    if (m_blocks[index3d(x + 1, y, z)] == 0) createFace(Cube::CubeFace::RIGHT, x, y, z);
                if (bottom)
                    if (m_blocks[index3d(x, y - 1, z)] == 0) createFace(Cube::CubeFace::BOTTOM, x, y, z);
                if (top)
                    if (m_blocks[index3d(x, y + 1, z)] == 0) createFace(Cube::CubeFace::TOP, x, y, z);
                if (back)
                    if (m_blocks[index3d(x, y, z - 1)] == 0) createFace(Cube::CubeFace::BACK, x, y, z);
                if (front)
                    if (m_blocks[index3d(x, y, z + 1)] == 0) createFace(Cube::CubeFace::FRONT, x, y, z);


                // Check 1 block width with neighbouring chunks for only the outer chunk blocks
                // Primjer:
                // ---------
                // - Ako gledamo recimo lijevi/WEST neighbour onda gledamo sa krajnom kockom tog
                // - susjednog chunka, a ako recimo gledamo desno/EAST onda trebamo provjeriti s
                // - prvom ili nultom kockom tog susjednog chunka
                //
                // - !left i drugi znaèi da su to vanjske kocke chunka tj. da nemaju susjeda na toj strani
                if (!left && m_neighbours[EAST] != nullptr)
                    if (m_neighbours[EAST]->getBlockLocal(m_size.x - 1, y, z) == 0) createFace(Cube::CubeFace::LEFT, x, y, z);
                if (!right && m_neighbours[WEST] != nullptr)
                    if (m_neighbours[WEST]->getBlockLocal(0, y, z) == 0) createFace(Cube::CubeFace::RIGHT, x, y, z);
                if (!bottom && m_neighbours[BELOW] != nullptr)
                    if (m_neighbours[BELOW]->getBlockLocal(x, m_size.y - 1, z) == 0) createFace(Cube::CubeFace::BOTTOM, x, y, z);
                if (!top && m_neighbours[ABOVE] != nullptr)
                    if (m_neighbours[ABOVE]->getBlockLocal(x, 0, z) == 0) createFace(Cube::CubeFace::TOP, x, y, z);
                if (!back && m_neighbours[NORTH] != nullptr)
                    if (m_neighbours[NORTH]->getBlockLocal(x, y, m_size.z - 1) == 0) createFace(Cube::CubeFace::BACK, x, y, z);
                if (!front && m_neighbours[SOUTH] != nullptr)
                    if (m_neighbours[SOUTH]->getBlockLocal(x, y, 0) == 0) createFace(Cube::CubeFace::FRONT, x, y, z);
            }

    // Set VBO adds a new VBO to the entity
    // so we have to use update which just changes the data.
    // We still have to initially set them though
    if (chunk.VBOs.empty())
    {
        chunk.setVBO(temp_verticies, 0, 3);
        chunk.setVBO(temp_textureCoords, 1, 2);
    }
    else
    {
        chunk.updateVBO(0, temp_verticies, 0, 3);
        chunk.updateVBO(1, temp_textureCoords, 1, 2);
    }
    chunk.setEBO(temp_indicies);

    #ifdef DEBUG
        printf("Chunk Verticies: %d\n", temp_verticies.size());
        printf("VBOs: %d\n\n", chunk.VBOs.size());
    #endif
}

int Chunk::index3d(int x, int y, int z)
{
    // Index = ((x * YSIZE + y) * ZSIZE) + z;
    int h = m_size.y;
    int d = m_size.z;
    return ((x * h + y) * d) + z;
}