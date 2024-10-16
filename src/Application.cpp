#include "Application.hpp"

Application::~Application()
{
    delete m_physicsWorld;
}

bool Application::Input(float deltaTime) const
{
    // Delta time will be used for future input handling, Box2D physics handles for now
    std::ignore = deltaTime;
    return m_inputSystem.Input(m_physicsWorld);
}

void Application::Update(float deltaTime) const
{
    // Delta time will be used for future updates, Box2D physics handles for now
    std::ignore = deltaTime;
    m_physicsSystem.Update();
}

void Application::Render()
{
    m_renderingSystem.Render();
}

void Application::Loop(float targetFPS)
{
    // Our infinite game/application loop
    Uint64 lastTime = SDL_GetTicks();
    float deltaTime = 1.0f / targetFPS; // Fixed time step for physics updates
    float accumulator = 0.0f;

    while (m_isRunning)
    {
        Uint64 currentTime = SDL_GetTicks();
        float frameTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        accumulator += frameTime;

        // Process input without considering deltaTime for now
        if (!Input(deltaTime))
        {
            m_isRunning = false;
        }

        int32 velocityIterations = 6; // Iterations for velocity calculations
        int32 positionIterations = 2; // Iterations for position calculations
        float timeStep = 1.0f / 60.0f;
        m_physicsWorld->Step(timeStep, velocityIterations, positionIterations);

        // Update the physics simulation using a fixed time step
        while (accumulator >= deltaTime)
        {
            Update(deltaTime);
            accumulator -= deltaTime;
        }

        // Render the scene
        Render();
    }
}

void Application::AddBox2D(const EntityID &entity, const float x, const float y, const float width, const float height, const bool isStatic, const bool isTrigger)
{
    // Assign a box2d body to the entity
    m_scene.AddBox2DCollider(entity, isStatic, isTrigger, x / m_board->m_tileSize, y / m_board->m_tileSize, width, height, m_physicsWorld);
}

void Application::AddSprite(const EntityID &entity, const std::string filePath, const float width, const float height)
{
    // Assign sprite component
    SpriteComponent *sprite = m_scene.Assign<SpriteComponent>(entity);
    sprite->filePath = filePath;
    sprite->width = width;
    sprite->height = height;
    auto sprite_texture = ResourceManager::Instance().LoadTexture(m_renderingSystem.GetSDLLayer()->GetRenderer(), sprite->filePath);
    sprite->texture = sprite_texture;
}

void Application::ImportSpritesheetLevel(const std::string levelPath, const std::string spritesheetPath)
{
    EntityID entity = m_scene.NewEntity();
    m_scene.Assign<SpriteSheetComponent>(entity);

    // set up spritesheet
    SpriteSheetComponent *sheetLocal = m_scene.Get<SpriteSheetComponent>(entity);
    sheetLocal->spriteSheet = new SpriteSheet(levelPath, m_board, m_renderingSystem.GetSDLLayer()->GetRenderer(), spritesheetPath);
    auto tiles = sheetLocal->spriteSheet->GetTileIds();
    sheetLocal->importedSheet = true;

    // for each tile add an entity
    for (size_t i = 0; i < tiles.size(); i++)
    {
        if (tiles.at(i) != -1)
        {
            int x = i % m_board->m_boardWidth; // Calculate x position
            int y = i / m_board->m_boardWidth; // Calculate y position
            m_scene.CreateSpriteSheetTile(x, y, m_physicsWorld, m_board);
        }
    }
}