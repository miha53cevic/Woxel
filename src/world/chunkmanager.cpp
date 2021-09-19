#include "chunkmanager.h"

#include "../util/cube.h"
#include "../util/math.h"
#include "blocks.h"

ChunkManager::ChunkManager()
    : atlas("resources/textures/textureAtlas.png", 2048, 256)
{
    // Default chunk size
    chunkSize = { 32, 32, 32 };
}

void ChunkManager::generateChunks(int x, int y, int z)
{
    worldSize = glm::vec3(x, y, z);

    // Create the chunks
    for (int sx = 0; sx < x; sx++)
        for (int sy = 0; sy < y; sy++)
            for (int sz = 0; sz < z; sz++)
            {
                auto temp = std::make_unique<Chunk>(glm::vec3(sx * chunkSize.x, sy * chunkSize.y, sz * chunkSize.z), chunkSize, &atlas);
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
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(EAST, chunks[IndexFrom3D(sx - 1, sy, sz)].get());
                if (right)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(WEST, chunks[IndexFrom3D(sx + 1, sy, sz)].get());
                if (back)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(NORTH, chunks[IndexFrom3D(sx, sy, sz - 1)].get());
                if (front)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(SOUTH, chunks[IndexFrom3D(sx, sy, sz + 1)].get());
                if (top)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(ABOVE, chunks[IndexFrom3D(sx, sy + 1, sz)].get());
                if (bottom)
                    chunks[IndexFrom3D(sx, sy, sz)]->setNeighbour(BELOW, chunks[IndexFrom3D(sx, sy - 1, sz)].get());
            }
}

void ChunkManager::setChunkSize(int x, int y, int z)
{
    chunkSize = { x, y, z };
}

int ChunkManager::getBlockGlobal(int x, int y, int z)
{
    // Get the chunk from global block position
    int cx = x / chunkSize.x;
    int cy = y / chunkSize.y;
    int cz = z / chunkSize.z;

    // Convert to local block position
    int lx = x % chunkSize.x;
    int ly = y % chunkSize.y;
    int lz = z % chunkSize.z;

    if (ChunkOutOfBounds(cx, cy, cz))
    {
    #ifdef DEBUG
            printf("Tried to get out of bounds chunk!\n");
    #endif
        return -1;
    }

    return chunks.at(IndexFrom3D(cx, cy, cz))->getBlockLocal(lx, ly, lz);
}

void ChunkManager::setBlockGlobal(int x, int y, int z, int blockid)
{
    // Get the chunk from global block position
    int cx = x / chunkSize.x;
    int cy = y / chunkSize.y;
    int cz = z / chunkSize.z;

    // Convert to local block position
    int lx = x % chunkSize.x;
    int ly = y % chunkSize.y;
    int lz = z % chunkSize.z;

    if (ChunkOutOfBounds(cx, cy, cz))
    {
    #ifdef DEBUG
            printf("Tried to get out of bounds chunk!\n");
    #endif
        return;
    }

    chunks.at(IndexFrom3D(cx, cy, cz))->setBlockLocal(lx, ly, lz, blockid);
}

Chunk* ChunkManager::getChunkFromGlobal(int x, int y, int z)
{
    // Get the chunk from global block position
    int cx = x / chunkSize.x;
    int cy = y / chunkSize.y;
    int cz = z / chunkSize.z;

    if (ChunkOutOfBounds(cx, cy, cz))
        return nullptr;

    return chunks[IndexFrom3D(cx, cy, cz)].get();
}

void ChunkManager::generateFlatTerrain(int minAmp)
{
    auto createTerrain = [&](Chunk* chunk)
    {
        // Go through every block in the chunk
        for (int x = 0; x < (int)chunkSize.x; x++)
            for (int z = 0; z < (int)chunkSize.z; z++)
            {
                int height = minAmp;

                // Check if the height is valid
                if (height > (chunkSize.y * worldSize.y))
                    height = (chunkSize.y * worldSize.y);

                // For every y block in the chunk set its layers
                for (int y = 0; y < (int)chunkSize.y; y++)
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
        for (int x = 0; x < (int)chunkSize.x; x++)
            for (int z = 0; z < (int)chunkSize.z; z++)
            {
                // Calculate the peaks
                float posX = (chunk->chunk.position.x + x) / chunkSize.x;
                float posZ = (chunk->chunk.position.z + z) / chunkSize.z;

                // Combine 2 noise height maps for hilly and flat terrain combos
                float noise1 = Noise::simplex2(posX + seed, posZ + seed, firstNoise);
                float noise2 = Noise::simplex2(posX + seed, posZ + seed, secondNoise);
                float result = noise1 * noise2;

                int height = result * maxAmp + minAmp;

                // Check if the height is valid
                if (height > (chunkSize.y * worldSize.y))
                    height = (chunkSize.y * worldSize.y);

                // For every y block in the chunk set its layers
                for (int y = 0; y < (int)chunkSize.y; y++)
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
                                if (x > 0 && x + 1 < (int)chunkSize.x && z > 0 && z + 1 < (int)chunkSize.z)
                                {
                                    glm::vec3 location;
                                    location.x = chunk->chunk.position.x + x;
                                    location.y = (float)height;
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
    return ((x * worldSize.y + y) * worldSize.z) + z;
}

bool ChunkManager::ChunkOutOfBounds(int x, int y, int z)
{
    if (x < 0 || x >= worldSize.x || y < 0 || y >= worldSize.y || z < 0 || z >= worldSize.z)
        return true;
    else return false;
}