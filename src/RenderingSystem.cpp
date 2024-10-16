#include "RenderingSystem.hpp"

void RenderingSystem::Render()
{
    m_imguiLayer->PreDraw(true, GetWindowTexture());

    m_imguiLayer->Draw();
}

SDL_Texture *RenderingSystem::GetWindowTexture()
{
    if (m_viewportTexture)
    {
        // Free the texture if it already exists
        SDL_DestroyTexture(m_viewportTexture);
    }
    // Step 1: Create an SDL Texture for ImGui
    m_viewportTexture = SDL_CreateTexture(m_sdlLayer->GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                          m_board->m_boardWidth * m_board->m_tileSize, m_board->m_boardHeight * m_board->m_tileSize);

    // Step 2: Set the texture as the render target
    SDL_SetRenderTarget(m_sdlLayer->GetRenderer(), m_viewportTexture);
    SDL_RenderClear(m_sdlLayer->GetRenderer());

    // Step 3: Call all SDL rendering functions
    SDLRender(m_scene->m_showColliders);
    if (m_scene->m_showGrid)
    {
        m_board->Render(m_sdlLayer->GetRenderer());
    }

    // Step 4: Reset the render target to the window
    SDL_SetRenderTarget(m_sdlLayer->GetRenderer(), NULL);

    return m_viewportTexture;
}

void RenderingSystem::SDLRender(bool &showColliders) const
{
    // Render the sprites
    for (EntityID ent : SceneView<SpriteComponent, TransformComponent>(*m_scene))
    {
        TransformComponent *transformLocal = m_scene->Get<TransformComponent>(ent);
        SpriteComponent *spriteLocal = m_scene->Get<SpriteComponent>(ent);
        m_sdlLayer->DrawTexture(spriteLocal->texture.get(), transformLocal->x, transformLocal->y, spriteLocal->width * m_board->m_tileSize, spriteLocal->height * m_board->m_tileSize);
    }

    // Render the sprite sheets
    for (EntityID ent : SceneView<SpriteSheetComponent>(*m_scene))
    {
        SpriteSheetComponent *sheetLocal = m_scene->Get<SpriteSheetComponent>(ent);
        if (sheetLocal->importedSheet)
        {
            sheetLocal->spriteSheet->Render(m_sdlLayer);
        }
    }

    for (EntityID ent : SceneView<GridSimulationComponent>(*m_scene))
    {
        GridSimulationComponent *grid = m_scene->Get<GridSimulationComponent>(ent);

        // Render the grid 
        m_sdlLayer->DrawGrid(grid->rows, grid->cols, grid->gridData);
    }

    if (showColliders)
    {
        for (EntityID ent : SceneView<Box2DColliderComponent>(*m_scene))
        {
            Box2DColliderComponent *box2d = m_scene->Get<Box2DColliderComponent>(ent);

            // Get the body's position
            b2Vec2 position = box2d->body->GetPosition();

            b2AABB aabb = box2d->body->GetFixtureList()->GetAABB(0); // Get the shape's AABB

            // Calculate the position and size for rendering in pixels
            float renderX = position.x * m_board->m_tileSize;
            float renderY = position.y * m_board->m_tileSize;
            float renderWidth = (aabb.upperBound.x - aabb.lowerBound.x) * m_board->m_tileSize;
            float renderHeight = (aabb.upperBound.y - aabb.lowerBound.y) * m_board->m_tileSize;

            // Determine the color based on trigger status
            SDL_Color color = box2d->isTrigger ? SDL_Color{0, 255, 255, 255} : SDL_Color{255, 255, 0, 255};

            // Render the rectangle at the calculated position and size
            m_sdlLayer->DrawRectangle(renderX, renderY, renderWidth, renderHeight, color);
        }
    }
}