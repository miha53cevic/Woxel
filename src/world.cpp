#include "world.h"
#include "cube.h"

Chunk::Chunk(glm::vec3 position)
{
    chunk.position = position;

    for (int i = 0; i < 6; i++)
        m_neighbours[i] = nullptr;

    generateChunk();
}

void Chunk::setBlockLocal(int x, int y, int z, int blockid)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
        return;
    else
        m_blocks[x][y][z] = blockid;
}

int Chunk::getBlockLocal(int x, int y, int z)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
        return -1;
    else
        return m_blocks[x][y][z];
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

void Chunk::generateChunk()
{
    for (int x = 0; x < CHUNK_SIZE; x++)
        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                m_blocks[x][y][z] = 1;
                if (y == CHUNK_SIZE - 1)
                    m_blocks[x][y][z] = 0;
            }

    for (int x = 1; x < CHUNK_SIZE - 1; x++)
        for (int y = 1; y < CHUNK_SIZE - 1; y++)
            for (int z = 1; z < CHUNK_SIZE - 1; z++)
            {
                m_blocks[x][y][z] = 1;
            }

    generateMesh();
}

void Chunk::generateMesh()
{
    std::vector<GLfloat> temp_verticies;
    std::vector<GLfloat> temp_textureCoords;
    std::vector<GLuint>  temp_indicies;

    int indicies = 0;
    auto createFace = [&](Cube::Face face, int x, int y, int z)
    {
        for (int i = 0; i < (int)face.verticies.size() / 3; i++)
        {
            temp_verticies.push_back(face.verticies[0 + i * 3] + x);
            temp_verticies.push_back(face.verticies[1 + i * 3] + y);
            temp_verticies.push_back(face.verticies[2 + i * 3] + z);
        }

        temp_textureCoords.insert(temp_textureCoords.end(), face.textureCoords.begin(), face.textureCoords.end());
        
        temp_indicies.push_back(indicies + 0);
        temp_indicies.push_back(indicies + 1);
        temp_indicies.push_back(indicies + 3);
        temp_indicies.push_back(indicies + 3);
        temp_indicies.push_back(indicies + 1);
        temp_indicies.push_back(indicies + 2);
        indicies += 4;
    };

    for (int x = 0; x < CHUNK_SIZE; x++)
        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                bool left = false, right = false, front = false, back = false, top = false, bottom = false;
                if (x != 0) left = true;
                if (y != 0) bottom = true;
                if (z != 0) back = true;
                if (x != CHUNK_SIZE - 1) right = true;
                if (y != CHUNK_SIZE - 1) top = true;
                if (z != CHUNK_SIZE - 1) front = true;

                if (m_blocks[x][y][z] == 0)
                    continue;

                if (left)
                    if (m_blocks[x - 1][y][z] == 0) createFace(Cube::getCubeFace(Cube::CubeFace::LEFT), x, y, z);
                if(right)
                    if (m_blocks[x + 1][y][z] == 0) createFace(Cube::getCubeFace(Cube::CubeFace::RIGHT), x, y, z);
                if (bottom)
                    if (m_blocks[x][y - 1][z] == 0) createFace(Cube::getCubeFace(Cube::CubeFace::BOTTOM), x, y, z);
                if (top)
                    if (m_blocks[x][y + 1][z] == 0) createFace(Cube::getCubeFace(Cube::CubeFace::TOP), x, y, z);
                if (back)
                    if (m_blocks[x][y][z - 1] == 0) createFace(Cube::getCubeFace(Cube::CubeFace::BACK), x, y, z);
                if (front)
                    if (m_blocks[x][y][z + 1] == 0) createFace(Cube::getCubeFace(Cube::CubeFace::FRONT), x, y, z);

                // Check with 1 block width of neighbouring chunks
                if (!right && m_neighbours[WEST] != nullptr)
                    if (m_neighbours[WEST]->getBlockLocal(x, y, z) == 0)
                        printf("Hello\n");

                if (!left && m_neighbours[EAST] != nullptr)
                    if (m_neighbours[EAST]->getBlockLocal(CHUNK_SIZE-1,y,z) == 0) createFace(Cube::getCubeFace(Cube::CubeFace::LEFT), x, y, z);
                if (!right && m_neighbours[WEST] != nullptr)
                    if (m_neighbours[WEST]->getBlockLocal(0,y,z) == 0) createFace(Cube::getCubeFace(Cube::CubeFace::RIGHT), x, y, z);
                if (!bottom && m_neighbours[BELOW] != nullptr)
                    if (m_neighbours[BELOW]->getBlockLocal(x,CHUNK_SIZE-1,z) == 0) createFace(Cube::getCubeFace(Cube::CubeFace::BOTTOM), x, y, z);
                if (!top && m_neighbours[ABOVE] != nullptr)
                    if (m_neighbours[ABOVE]->getBlockLocal(x,0,z) == 0) createFace(Cube::getCubeFace(Cube::CubeFace::TOP), x, y, z);
                if (!back && m_neighbours[NORTH] != nullptr)
                    if (m_neighbours[NORTH]->getBlockLocal(x,y,0) == 0) createFace(Cube::getCubeFace(Cube::CubeFace::BACK), x, y, z);
                if (!front && m_neighbours[SOUTH] != nullptr)
                    if (m_neighbours[SOUTH]->getBlockLocal(x,y,CHUNK_SIZE-1) == 0) createFace(Cube::getCubeFace(Cube::CubeFace::FRONT), x, y, z);
            }

    chunk.setVBO(temp_verticies, 0, 3);
    chunk.setVBO(temp_textureCoords, 1, 2);
    chunk.setEBO(temp_indicies);

    printf("Chunk Verticies: %d\n", temp_verticies.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ChunkManager::ChunkManager()
{
}

void ChunkManager::generateChunks(int x, int y, int z)
{
    chunkSize = glm::vec3(x, y, z);
    
    // Create the chunks
    for (int sx = 0; sx < x; sx++)
        for (int sy = 0; sy < y; sy++)
            for (int sz = 0; sz < z; sz++)
            {
                auto temp = std::make_unique<Chunk>(glm::vec3(sx * CHUNK_SIZE, sy * CHUNK_SIZE, sz * CHUNK_SIZE));
                temp->chunk.texture.loadTexture("resources/textures/plank_draft.png");
                chunks.push_back(std::move(temp));
            }

    // Set each chunks neighbours
    for (int sx = 0; sx < x; sx++)
        for (int sy = 0; sy < y; sy++)
            for (int sz = 0; sz < z; sz++)
            {
                bool left = false, right = false, front = false, back = false, top = false, bottom = false;
                if (sx != 0) left = true;
                if (sy != 0) bottom = true;
                if (sz != 0) back = true;
                if (sx != x - 1) right = true;
                if (sy != y - 1) top = true;
                if (sz != z - 1) front = true;

                if (left)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(EAST, chunks[IndexFrom3D(sx-1, sy, sz)].get());
                if (right)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(WEST, chunks[IndexFrom3D(sx+1, sy, sz)].get());
                if (back)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(NORTH, chunks[IndexFrom3D(sx, sy, sz-1)].get());
                if (front)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(SOUTH, chunks[IndexFrom3D(sx, sy, sz+1)].get());
                if (top)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(ABOVE, chunks[IndexFrom3D(sx, sy+1, sz)].get());
                if (bottom)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(BELOW, chunks[IndexFrom3D(sx, sy-1, sz)].get());
            }

    for (auto& chunk : chunks)
        chunk->Update();
}

int ChunkManager::getBlockGlobal(int x, int y, int z)
{
    // Get the chunk from global block position
    int cx = x / CHUNK_SIZE;
    int cy = y / CHUNK_SIZE;
    int cz = z / CHUNK_SIZE;

    // Convert to local block position
    int lx = x % CHUNK_SIZE;
    int ly = y % CHUNK_SIZE;
    int lz = z % CHUNK_SIZE;

    if (ChunkOutOfBounds(cx, cy, cz))
        return -1;

    return chunks.at(IndexFrom3D(cx, cy, cz))->getBlockLocal(lx, ly, lz);
}

void ChunkManager::setBlockGlobal(int x, int y, int z, int blockid)
{
    // Get the chunk from global block position
    int cx = x / CHUNK_SIZE;
    int cy = y / CHUNK_SIZE;
    int cz = z / CHUNK_SIZE;

    // Convert to local block position
    int lx = x % CHUNK_SIZE;
    int ly = y % CHUNK_SIZE;
    int lz = z % CHUNK_SIZE;

    if (ChunkOutOfBounds(cx, cy, cz))
        return;

    chunks.at(IndexFrom3D(cx, cy, cz))->setBlockLocal(lx, ly, lz, blockid);
}

Chunk* ChunkManager::getChunkFromGlobal(int x, int y, int z)
{
    // Get the chunk from global block position
    int cx = x / CHUNK_SIZE;
    int cy = y / CHUNK_SIZE;
    int cz = z / CHUNK_SIZE;

    if (ChunkOutOfBounds(cx, cy, cz))
        return nullptr;

    return chunks[IndexFrom3D(cx, cy, cz)].get();
}

int ChunkManager::IndexFrom3D(int x, int y, int z)
{
    // Index = ((x * YSIZE + y) * ZSIZE) + z;
    return ((x * chunkSize.y + y) * chunkSize.z) + z;
}

bool ChunkManager::ChunkOutOfBounds(int x, int y, int z)
{
    if (x < 0 || x >= chunkSize.x || y < 0 || y >= chunkSize.y || z < 0 || z >= chunkSize.z)
        return true;
    else return false;
}