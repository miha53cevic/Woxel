#include "world.h"
#include "cube.h"
#include "math.h"
#include "blocks.h"

#define DEBUG

Chunk::Chunk(glm::vec3 position, gl::TextureAtlas* atlas)
    : m_atlas(atlas)
{
    chunk.position = position;

    for (int i = 0; i < 6; i++)
        m_neighbours[i] = nullptr;

    // By default the chunk is empty with AIR blocks
    for (int x = 0; x < CHUNK_SIZE; x++)
        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int z = 0; z < CHUNK_SIZE; z++)
                m_blocks[x][y][z] = Blocks::AIR;
}

void Chunk::setBlockLocal(int x, int y, int z, int blockid)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        #ifdef DEBUG
            printf("Set out of bounds block!\n");
        #endif
        return;
    }
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

        auto texCoords = Blocks::getTextureCoords((Blocks::BLOCK)m_blocks[x][y][z], cubeface, *m_atlas);
        temp_textureCoords.insert(temp_textureCoords.end(), texCoords.begin(), texCoords.end());
        
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
                    if (m_blocks[x - 1][y][z] == 0) createFace(Cube::CubeFace::LEFT, x, y, z);
                if (right)
                    if (m_blocks[x + 1][y][z] == 0) createFace(Cube::CubeFace::RIGHT, x, y, z);
                if (bottom)
                    if (m_blocks[x][y - 1][z] == 0) createFace(Cube::CubeFace::BOTTOM, x, y, z);
                if (top)
                    if (m_blocks[x][y + 1][z] == 0) createFace(Cube::CubeFace::TOP, x, y, z);
                if (back)
                    if (m_blocks[x][y][z - 1] == 0) createFace(Cube::CubeFace::BACK, x, y, z);
                if (front)
                    if (m_blocks[x][y][z + 1] == 0) createFace(Cube::CubeFace::FRONT, x, y, z);


                // Check 1 block width with neighbouring chunks for only the outer chunk blocks
                // Primjer:
                // ---------
                // - Ako gledamo recimo lijevi/WEST neighbour onda gledamo sa krajnom kockom tog
                // - susjednog chunka, a ako recimo gledamo desno/EAST onda trebamo provjeriti s
                // - prvom ili nultom kockom tog susjednog chunka
                //
                // - !left i drugi znaèi da su to vanjske kocke chunka tj. da nemaju susjeda na toj strani
                if (!left && m_neighbours[EAST] != nullptr)
                    if (m_neighbours[EAST]->getBlockLocal(CHUNK_SIZE - 1, y, z) == 0) createFace(Cube::CubeFace::LEFT, x, y, z);
                if (!right && m_neighbours[WEST] != nullptr)
                    if (m_neighbours[WEST]->getBlockLocal(0, y, z) == 0) createFace(Cube::CubeFace::RIGHT, x, y, z);
                if (!bottom && m_neighbours[BELOW] != nullptr)
                    if (m_neighbours[BELOW]->getBlockLocal(x, CHUNK_SIZE - 1, z) == 0) createFace(Cube::CubeFace::BOTTOM, x, y, z);
                if (!top && m_neighbours[ABOVE] != nullptr)
                    if (m_neighbours[ABOVE]->getBlockLocal(x, 0, z) == 0) createFace(Cube::CubeFace::TOP, x, y, z);
                if (!back && m_neighbours[NORTH] != nullptr)
                    if (m_neighbours[NORTH]->getBlockLocal(x, y, CHUNK_SIZE - 1) == 0) createFace(Cube::CubeFace::BACK, x, y, z);
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

///////////////////////////////////////////////////////////////////////////////////////////////////

ChunkManager::ChunkManager()
    : atlas("resources/textures/textureAtlas.png", 2048, 256)
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
                auto temp = std::make_unique<Chunk>(glm::vec3(sx * CHUNK_SIZE, sy * CHUNK_SIZE, sz * CHUNK_SIZE), &atlas);
                //temp->chunk.texture.loadTexture("resources/textures/textureAtlas.png");
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

                // boolean values tell us if a given chunk can have a neighbour on that side and if it can
                // we add a reference to that neighbour to the given chunk[x][y][z]
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

void ChunkManager::generateFlatTerrain(int minAmp)
{
    auto createTerrain = [&](Chunk* chunk)
    {
        // Go through every block in the chunk
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                int height = minAmp;

                // Check if the height is valid
                if (height > (CHUNK_SIZE * chunkSize.y))
                    height = (CHUNK_SIZE * chunkSize.y);

                // For every y block in the chunk set its layers
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    // Get the voxels global Y position
                    int voxelY = chunk->chunk.position.y + y;

                    if (voxelY == height)     chunk->setBlockLocal(x, y, z, Blocks::GRASS);
                    else if (voxelY < height) chunk->setBlockLocal(x, y, z, Blocks::DIRT);
                }
            }
    };

    for (auto& chunk : chunks)
        createTerrain(chunk.get());

    /*
    *   Note
    *   -----
    *   - After generating the terrain we need to update every chunk.
    *   - If we don't the changes to the chunks blocks won't be able to be seen from other chunks
    *   Example
    *   - 1. chunk gets it's blocks set and since the other chunks blocks haven't been set and by default
    *   - a multi dimensional array has a default value of 0 it sees that it should make a wall with the other
    *   - chunks even though they haven't been setup properly yet.
    *   - This is also why generateChunkMesh isn't called in the chunk->generateTerrain functions since it needs to be called
    *   - after every chunk has had it's blocks set up.
    */
    for (auto& chunk : chunks)
        chunk->Update();
}

void ChunkManager::generateTerrain(int minAmp, int maxAmp)
{
    auto createTree = [&](glm::vec3 location)
    {
        int treeHeight = Math::iRandom(4, 7);
        for (int i = 0; i < treeHeight; i++)
            setBlockGlobal(location.x, location.y + i, location.z, Blocks::LOG);

        for (int x = -2; x <= 2; x++)
            for (int y = -2; y < 1; y++)
                for (int z = -2; z <= 2; z++)
                {
                    if (x == 0 && y < 0 && z == 0)
                        continue;

                    if (y == -1 && (abs(x) == 2 || abs(z) == 2))
                        continue;

                    if (y == 0 && (abs(x) >= 1 || abs(z) >= 1))
                        continue;

                    setBlockGlobal(location.x + x, location.y + y + treeHeight, location.z + z, Blocks::LEAF);
                }

        setBlockGlobal(location.x + 1, location.y + treeHeight, location.z + 0, Blocks::LEAF);
        setBlockGlobal(location.x - 1, location.y + treeHeight, location.z + 0, Blocks::LEAF);
        setBlockGlobal(location.x + 0, location.y + treeHeight, location.z + 1, Blocks::LEAF);
        setBlockGlobal(location.x + 0, location.y + treeHeight, location.z - 1, Blocks::LEAF);
    };

    int seed = Math::iRandom(0, 65536);

    Noise::NoiseOptions firstNoise;
    firstNoise.octaves = 6;
    firstNoise.frequency = 0.25f;
    firstNoise.roughness = 0.5f;
    firstNoise.redistribution = 1.0f;

    Noise::NoiseOptions secondNoise;
    secondNoise.octaves = 4;
    secondNoise.frequency = 0.1f;
    secondNoise.roughness = 0.48f;
    secondNoise.redistribution = 2.5f;

    auto createTerrain = [&](Chunk* chunk)
    {
        // Go through every block in the chunk
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                // Calculate the peaks
                float posX = (chunk->chunk.position.x + x) / CHUNK_SIZE;
                float posZ = (chunk->chunk.position.z + z) / CHUNK_SIZE;

                // Combine 2 noise height maps for hilly and flat terrain combos
                float noise1 = Noise::simplex2(posX + seed, posZ + seed, firstNoise);
                float noise2 = Noise::simplex2(posX + seed, posZ + seed, secondNoise);
                float result = noise1 * noise2;

                int height = result * maxAmp + minAmp;

                // Check if the height is valid
                if (height > (CHUNK_SIZE * chunkSize.y))
                    height = (CHUNK_SIZE * chunkSize.y);

                // For every y block in the chunk set its layers
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    // Get the voxels global Y position
                    int voxelY = chunk->chunk.position.y + y;

                    /*
                    *   First layer is grass
                    *   the next 3 layers are dirt
                    *   everything below that is stone
                    *   Water spawns above the peaks(height) if they are below the
                    *   given water_level threshold
                    */ 

                    // If we went over the peaks any blocks above them are AIR
                    // so if they are under the WATER_LEVEL we can place WATER BLOCK
                    // there instead of AIR blocks
                    if (voxelY > height)
                    {
                        if (voxelY < WATER_LEVEL)
                            chunk->setBlockLocal(x, y, z, Blocks::WATER);
                    }
                    else if (voxelY == height)
                    {
                        // Set SAND to spawn next to WATER
                        if (voxelY < WATER_LEVEL)
                            chunk->setBlockLocal(x, y, z, Blocks::SAND);
                        // Set the top block
                        else
                        {
                            chunk->setBlockLocal(x, y, z, Blocks::GRASS);

                            if (Math::fRandom(0, 1) >= 0.99f)
                                if (x > 0 && x + 1 < CHUNK_SIZE && z > 0 && z + 1 < CHUNK_SIZE)
                                {
                                    glm::vec3 location;
                                    location.x = chunk->chunk.position.x + x;
                                    location.y = height;
                                    location.z = chunk->chunk.position.z + z;
                                    createTree(location);
                                }
                        }
                    }
                    else if (voxelY < height && voxelY > height - 4) chunk->setBlockLocal(x, y, z, Blocks::DIRT);
                    else if (voxelY < height) chunk->setBlockLocal(x, y, z, Blocks::STONE);
                }
            }
    };

    for (auto& chunk : chunks)
        createTerrain(chunk.get());

    // Check note in generateFlatTerrain() function
    for (auto& chunk : chunks)
        chunk->Update();
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