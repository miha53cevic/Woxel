#include "playing.h"

#include "../app/app.h"
#include "../states/statemanager.h"

#define toStr(x) std::to_string(x)
#define HOTBAR_SIZE 7
#define CHUNK_SIZE 32

Playing::Playing()
    : uirenderer({ App::ScreenWidth(), App::ScreenHeight() })
{
}

void Playing::Event(SDL_Event & e)
{
    // Right mouse click - add blocks
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT)
    {
        if (lastRayPos.x != INFINITY)
        {
            glm::vec3 temp_ray = lastRayPos;
            // Keep looping until we either find an AIR block or get out of bounds and get -1
            while (chunk_manager.getBlockGlobal(temp_ray.x, temp_ray.y, temp_ray.z) != 0 &&
                chunk_manager.getBlockGlobal(temp_ray.x, temp_ray.y, temp_ray.z) != -1
                )
            {
                temp_ray += lastUnitRay * 0.05f;
            }

            // We still have to check if we got -1(OutOfBounds) or 0(AIR)
            if (chunk_manager.getBlockGlobal(temp_ray.x, temp_ray.y, temp_ray.z) == 0)
            {
                // Set the block and update the chunk and it's neighbours
                chunk_manager.setBlockGlobal(temp_ray.x, temp_ray.y, temp_ray.z, hotbar[hotbar_selection]);

                auto chunk = chunk_manager.getChunkFromGlobal(lastRayPos.x, lastRayPos.y, lastRayPos.z);
                chunk->Update(); // regenerate chunk mesh
                chunk->UpdateNeighbours();
            }
        }
    }

    // Hotbar scrolling
    if (e.type == SDL_MOUSEWHEEL)
    {
        // +y mouse wheel up and -y mousewheel down
        if (e.wheel.y > 0)      hotbar_selection -= 1;
        else if (e.wheel.y < 0) hotbar_selection += 1;

        if (hotbar_selection < 0)                       hotbar_selection = (HOTBAR_SIZE - 1);
        else if (hotbar_selection > (HOTBAR_SIZE - 1))  hotbar_selection = 0;

        uirenderer.setUI("hotbar_selection", { 416 + hotbar_selection * 64, 624 }, { 64, 64 }, 0.0f);
    }

    // Wireframe mode
    if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_TAB)
    {
        bWireframe = !bWireframe;
        App::WireFrame(bWireframe);
    }

    // Enter & Exit Creative Mode
    if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_C)
    {
        bCreativeMode = !bCreativeMode;
        if (bCreativeMode) printf("Enabled Creative Mode!\n");
        else               printf("Disabled Creative Mode!\n");
    }
}

void Playing::Setup()
{
    // Enable back face CCW culling
    App::Culling(true);

    App::ShowCursor(false);

    // Set Sky colour
    App::ClearColor(64, 191, 255, 255);

    // Initial player position is in the middle of the map
    camera.setPosition({ 8 * CHUNK_SIZE, CHUNK_SIZE * 2, 8 * CHUNK_SIZE });
    velocity = { 0, 0, 0 };

    // Initial hotbar items
    for (int i = 0; i < HOTBAR_SIZE; i++)
        hotbar[i] = i + 1;

    shader.setAttribute(0, "position");
    shader.setAttribute(1, "textureCoords");
    shader.createProgram("resources/shaders/shader");

    shader.setUniformLocation("MVPMatrix");

    // Create chunks
    chunk_manager.generateChunks(16, 4, 16);
    chunk_manager.generateTerrain(CHUNK_SIZE, CHUNK_SIZE * 3);
    printf("Created %d chunk(s)\n", chunk_manager.chunks.size());

    outline.setAttribute(0, "position");
    outline.createProgram("resources/shaders/outline_shader");

    outline.setUniformLocation("MVPMatrix");

    breakingCube.texture.loadTexture("resources/textures/textureAtlas.png");

    /*
        Explanation
        -----------
        Blocks::Block enum class has blocks ordered with numbers going from 0 thus
        we can just use those numbers as the identifier string for each ui block in the hotbar.

        P.S. toS is just a shortened version of std::to_string() function
    */
    uirenderer.addUI(toStr(Blocks::DIRT), "resources/textures/inventory/blocks/dirt.png");
    uirenderer.addUI(toStr(Blocks::GRASS), "resources/textures/inventory/blocks/grass.png");
    uirenderer.addUI(toStr(Blocks::LEAF), "resources/textures/inventory/blocks/leaves.png");
    uirenderer.addUI(toStr(Blocks::LOG), "resources/textures/inventory/blocks/log.png");
    uirenderer.addUI(toStr(Blocks::PLANKS), "resources/textures/inventory/blocks/plank.png");
    uirenderer.addUI(toStr(Blocks::SAND), "resources/textures/inventory/blocks/sand.png");
    uirenderer.addUI(toStr(Blocks::STONE), "resources/textures/inventory/blocks/stone.png");

    uirenderer.addUI("hotbar_selection", "resources/textures/inventory/hotbar_selection.png");
    uirenderer.setUI("hotbar_selection", { 416, 624 }, { 64, 64 }, 0.0f);

    uirenderer.addUI("hotbar", "resources/textures/inventory/hotbar.png");
    uirenderer.setUI("hotbar", { 416, 624 }, { 448, 64 }, 0.0f);

    uirenderer.addUI("xhair", "resources/textures/xhair.png");
    uirenderer.setUI("xhair", { 632, App::ScreenHeight() / 2 - 8 }, { 16, 16 }, 0.0f);
}

void Playing::Loop(float elapsed)
{
    // Update camera position and rotation
    camera.Update(App::GetWindow(), App::GetFocus());

    if (bCreativeMode) camera.Movement(App::GetKeys(), elapsed);
    else if (!bCreativeMode) CollisionMovement(10, elapsed);

    // Ray casting
    for (Math::Ray ray(camera.getPosition(), camera.getRotation()); ray.getLength() < 6; ray.step(0.05f))
    {
        glm::vec3 r = ray.getEnd();
        if (chunk_manager.getBlockGlobal(r.x, r.y, r.z) > 0)
        {
            // Save the first blocks position that was hit and break out of the loop
            lastRayPos = r;

            // Save the unit vector of the ray as well (check Ray code for formula)
            lastUnitRay.x = glm::cos(glm::radians(camera.getRotation().y + 90));
            lastUnitRay.z = glm::sin(glm::radians(camera.getRotation().y + 90));
            lastUnitRay.y = glm::tan(glm::radians(camera.getRotation().x));

            createCubeOutline(r.x, r.y, r.z, 5);
            break;
        }
        else lastRayPos = glm::vec3(INFINITY, 0, 0);
    }

    // Render chunks in the chunk_manager
    for (auto& i : chunk_manager.chunks)
        RenderChunk(i->chunk, shader);

    // Render selected block outline
    RenderEntity(voxelOutline, outline, GL_LINES);

    // Check for block breaking
    breakBlockAction(elapsed);

    // Render the UI
    uirenderer.DrawUI("xhair");
    // Render inventory icons
    for (int i = 1; i < 8; i++)
        uirenderer.DrawUI(toStr(i));

    // Render hotbar and the hotbar selection (white hotbar square)
    uirenderer.DrawUI("hotbar_selection");
    uirenderer.DrawUI("hotbar");

    // Setup hotbar icons
    for (int i = 0; i < HOTBAR_SIZE; i++)
    {
        uirenderer.setUI(toStr((Blocks::BLOCK)hotbar[i]), { 424 + i * 64, 632 }, { 48, 48 }, 0.0f);
    }
}

void Playing::Pause()
{
}

void Playing::Resume()
{
}

void Playing::RenderEntity(Entity & e, gl::Shader & s, GLenum mode)
{
    s.Bind();

    // Check if a texture exists and try to load it
    if (e.texture.texture != -1)
        e.texture.activateAndBind();

    s.loadMatrix(
        s.getUniformLocation("MVPMatrix"),
        Math::createMVPMatrix(glm::vec2(App::ScreenWidth(), App::ScreenHeight()), 90, 0.1f, 1000.0f, camera, e)
    );

    e.VAO.Bind();
    glDrawElements(mode, e.EBO.size, GL_UNSIGNED_INT, 0);
    e.VAO.Unbind();

    s.Unbind();
}

void Playing::RenderChunk(Entity & e, gl::Shader & s)
{
    s.Bind();

    // All chunks use the same texture atlas
    chunk_manager.atlas.texture.activateAndBind();

    s.loadMatrix(
        s.getUniformLocation("MVPMatrix"),
        Math::createMVPMatrix(glm::vec2(App::ScreenWidth(), App::ScreenHeight()), 90, 0.1f, 1000.0f, camera, e)
    );

    e.VAO.Bind();
    glDrawElements(GL_TRIANGLES, e.EBO.size, GL_UNSIGNED_INT, 0);
    e.VAO.Unbind();

    s.Unbind();
}

void Playing::createCubeOutline(float x, float y, float z, int width)
{
    // Set to intiger space because the cubes are 1*1*1 in size
    x = (int)x;
    y = (int)y;
    z = (int)z;

    std::vector<float> vert = {
        x + 0, y + 1, z + 1,
        x + 0, y + 1, z + 0,
        x + 1, y + 1, z + 0,
        x + 1, y + 1, z + 1,

        x + 0, y + 0, z + 1,
        x + 0, y + 0, z + 0,
        x + 1, y + 0, z + 0,
        x + 1, y + 0, z + 1
    };

    // Lines are defined by 2 vertexes which index you get from the vert array
    std::vector<GLuint> indic = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        4, 5,
        5, 6,
        6, 7,
        7, 4,
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };

    // Set VBO creates a new VBO where update just changes the data
    if (voxelOutline.VBOs.empty())
    {
        voxelOutline.setVBO(vert, 0, 3);
        voxelOutline.setEBO(indic);
    }
    else voxelOutline.updateVBO(0, vert, 0, 3);

    glLineWidth(width);
}

void Playing::createBreakingAnimation(glm::ivec2 breakAnimTexCoords)
{
    // Cube is 1*1*1 - int space
    int x = (int)lastRayPos.x;
    int y = (int)lastRayPos.y;
    int z = (int)lastRayPos.z;

    // Setup for 6 faces
    std::vector<float> texCoords;
    for (int i = 0; i < 6; i++)
    {
        auto temp = chunk_manager.atlas.getTextureCoords(breakAnimTexCoords);
        texCoords.insert(texCoords.end(), temp.begin(), temp.end());
    }

    if (breakingCube.VBOs.empty())
    {
        // Koliko je kocka veæa od druge
        const float offset = 0.05f;
        const std::vector<GLfloat> verticies = {
            // Back face
            1 + offset,1 + offset,0 - offset,
            1 + offset,0 - offset,0 - offset,
            0 - offset,0 - offset,0 - offset,
            0 - offset,1 + offset,0 - offset,

            // Front face
            0 - offset,1 + offset,1 + offset,
            0 - offset,0 - offset,1 + offset,
            1 + offset,0 - offset,1 + offset,
            1 + offset,1 + offset,1 + offset,

            // Right face
            1 + offset,1 + offset,1 + offset,
            1 + offset,0 - offset,1 + offset,
            1 + offset,0 - offset,0 - offset,
            1 + offset,1 + offset,0 - offset,

            // Left Face
            0 - offset,1 + offset,0 - offset,
            0 - offset,0 - offset,0 - offset,
            0 - offset,0 - offset,1 + offset,
            0 - offset,1 + offset,1 + offset,

            // Top face
            0 - offset,1 + offset,1 + offset,
            1 + offset,1 + offset,1 + offset,
            1 + offset,1 + offset,0 - offset,
            0 - offset,1 + offset,0 - offset,

            // Bottom face
            0 - offset,0 - offset,1 + offset,
            0 - offset,0 - offset,0 - offset,
            1 + offset,0 - offset,0 - offset,
            1 + offset,0 - offset,1 + offset
        };

        // Load data
        breakingCube.setVBO(verticies, 0, 3);
        breakingCube.setVBO(texCoords, 1, 2);
        breakingCube.setEBO(Cube::indicies);
    }
    else breakingCube.updateVBO(1, texCoords, 1, 2);

    // Translate
    breakingCube.position = glm::vec3(x, y, z);

    // Render
    RenderEntity(breakingCube, shader, GL_TRIANGLES);
}

void Playing::breakBlockAction(float elapsed)
{
    // Break a block
    if (App::MouseHold(SDL_BUTTON_LEFT))
    {
        // Player changed of the block that was being broken so reset timer
        if (breakingBlockPos != glm::ivec3(lastRayPos))
            totalTime = 0.0f;

        breakingBlockPos = glm::ivec3(lastRayPos);

        // Get breaking time based on the block that we are breaking
        int block = chunk_manager.getBlockGlobal(breakingBlockPos.x, breakingBlockPos.y, breakingBlockPos.z);
        const float breakTime = Blocks::getBreakTime((Blocks::BLOCK)block);

        // Wait for a certain amount of time and then break the block
        if (totalTime >= breakTime)
        {
            // If the ray doesn't hit a block the X part gets set to INFINITY
            if (lastRayPos.x != INFINITY)
            {
                // Set the block to air if it's destroyed
                chunk_manager.setBlockGlobal(lastRayPos.x, lastRayPos.y, lastRayPos.z, 0);
                // Update the chunk mesh and the surrounding neighbours as well
                auto chunk = chunk_manager.getChunkFromGlobal(lastRayPos.x, lastRayPos.y, lastRayPos.z);
                chunk->Update();
                chunk->UpdateNeighbours();

                // Reset totalTime
                totalTime = 0.0f;
            }
        }

        // Cycle animation stages
        if (totalTime >= breakTime / 1.10f)         createBreakingAnimation({ 2, 7 });
        else if (totalTime >= breakTime / 2.0f)     createBreakingAnimation({ 1, 7 });
        else if (totalTime >= 0.25f)                createBreakingAnimation({ 0, 7 });

        // Add to timer deltaTime
        totalTime += elapsed;
    }
    else totalTime = 0.0f; // if left is not held keep totalTime at 0
}

void Playing::CollisionMovement(float speed, float elapsed)
{
    const float Offset = 0.25f;
    const float Height = 1.35f;

    glm::vec3 position = camera.getPosition();
    glm::vec3 rotation = camera.getRotation();

    if (App::GetKeys()[SDL_SCANCODE_W])
    {
        // Calculate the directional vector and add it to camera position
        velocity.z -= speed * elapsed * cosf(glm::radians(-rotation.y));
        velocity.x -= speed * elapsed * sinf(glm::radians(-rotation.y));
    }
    if (App::GetKeys()[SDL_SCANCODE_S])
    {
        velocity.z += speed * elapsed * cosf(glm::radians(-rotation.y));
        velocity.x += speed * elapsed * sinf(glm::radians(-rotation.y));
    }
    if (App::GetKeys()[SDL_SCANCODE_D])
    {
        velocity.z -= speed * elapsed * sinf(glm::radians(-rotation.y));
        velocity.x += speed * elapsed * cosf(glm::radians(-rotation.y));
    }
    if (App::GetKeys()[SDL_SCANCODE_A])
    {
        velocity.z += speed * elapsed * sinf(glm::radians(-rotation.y));
        velocity.x -= speed * elapsed * cosf(glm::radians(-rotation.y));
    }

    if (App::GetKeys()[SDL_SCANCODE_SPACE])
    {
        // Only jump if the player donesn't have air blocks under him
        if (chunk_manager.getBlockGlobal(position.x, position.y - Height - 1, position.z) > 0)
            velocity.y = 10.0f * elapsed * Height;
    }

    velocity.y -= 1.0f * elapsed;

    // Add extra offset to movedPos because the camera clips the faces otherwise
    // check if we need to add + or - offset depending on the direction we are moving to so (newPos - curPos)
    glm::vec3 movedPos = velocity + position;
    if (movedPos.x - camera.getPosition().x < 0)
        movedPos.x -= Offset;
    else
        movedPos.x += Offset;

    if (movedPos.y - camera.getPosition().y < 0)
        movedPos.y -= Offset;
    else
        movedPos.y += Offset;

    if (movedPos.z - camera.getPosition().z < 0)
        movedPos.z -= Offset;
    else
        movedPos.z += Offset;

    if (chunk_manager.getBlockGlobal(movedPos.x, movedPos.y, movedPos.z) >= 0)
    {
        // NOTE: The player is 2 blocks in height so we have to check 2 blocks for x, y, z

        // X-Axis
        // Check upper (camera) block
        if (chunk_manager.getBlockGlobal(movedPos.x, position.y, position.z) != 0)
            velocity.x = 0;
        // Check lower block
        if (chunk_manager.getBlockGlobal(movedPos.x, position.y - Height, position.z) != 0)
            velocity.x = 0;

        // Y-Axis
        // Same thing, check the block under the player and above the player
        if (chunk_manager.getBlockGlobal(position.x, movedPos.y - Height, position.z) != 0)
            velocity.y = 0;
        if (chunk_manager.getBlockGlobal(position.x, movedPos.y, position.z) != 0)
            velocity.y = 0;

        // Z-Axis
        if (chunk_manager.getBlockGlobal(position.x, position.y, movedPos.z) != 0)
            velocity.z = 0;
        if (chunk_manager.getBlockGlobal(position.x, position.y - Height, movedPos.z) != 0)
            velocity.z = 0;

    }
    // Keep the player inside of chunk borders by making sure he can only travel through air blocks
    else if (chunk_manager.getBlockGlobal(movedPos.x, movedPos.y, movedPos.z) == -1)
    {
        velocity = { 0, 0, 0 };
    }

    // Update the position
    camera.setPosition(position + velocity);

    // clear velocity after each update
    velocity.x = 0;
    velocity.z = 0;

}
