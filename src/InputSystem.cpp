#include "InputSystem.hpp"

bool InputSystem::Input(b2World *physicsWorld) const
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    SDL_Event event;
    bool isMouseDown = false;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            return false;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (event.window.windowID == SDL_GetWindowID(m_sdlLayer->GetWindow()))
            {
                return false;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            isMouseDown = false;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            isMouseDown = true;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (event.key.keysym.sym == SDLK_a)
            {
                for (EntityID ent : SceneView<InputComponent>(*m_scene))
                {
                    InputComponent *inputLocal = m_scene->Get<InputComponent>(ent);
                    inputLocal->leftPress = true;
                }
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                for (EntityID ent : SceneView<InputComponent>(*m_scene))
                {
                    InputComponent *inputLocal = m_scene->Get<InputComponent>(ent);
                    inputLocal->rightPress = true;
                }
            }
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                for (EntityID ent : SceneView<InputComponent>(*m_scene))
                {
                    InputComponent *inputLocal = m_scene->Get<InputComponent>(ent);
                    inputLocal->spacePress = true;
                }
            }
            else
            {
                for (EntityID ent : SceneView<InputComponent>(*m_scene))
                {
                    InputComponent *inputLocal = m_scene->Get<InputComponent>(ent);
                    inputLocal->spacePress = false;
                }
            }

            break;
        case SDL_EVENT_KEY_UP:
            if (event.key.keysym.sym == SDLK_a)
            {
                for (EntityID ent : SceneView<InputComponent>(*m_scene))
                {
                    InputComponent *inputLocal = m_scene->Get<InputComponent>(ent);
                    inputLocal->leftPress = false;
                }
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                for (EntityID ent : SceneView<InputComponent>(*m_scene))
                {
                    InputComponent *inputLocal = m_scene->Get<InputComponent>(ent);
                    inputLocal->rightPress = false;
                }
            }
            break;
        }
        if (isMouseDown)
        {
            // Check if there is a selected entity, and the mouse is inside the viewport
            if (m_scene->m_selectedEntity == static_cast<unsigned long long>(-1) ||
                m_imguiLayer->viewportMousePos.x < 0 ||
                m_imguiLayer->viewportMousePos.y < 0 ||
                m_imguiLayer->viewportMousePos.x > m_board->m_boardWidth * m_board->m_tileSize ||
                m_imguiLayer->viewportMousePos.y > m_board->m_boardHeight * m_board->m_tileSize)
            {
                break;
            }
            else
            {
                // Chevk if the entity has a GridSimulationComponent
                GridSimulationComponent *grid = m_scene->Get<GridSimulationComponent>(m_scene->m_selectedEntity);
                // Check if the entity has a SpriteSheetComponent (this is the only mouse input we handle now)
                SpriteSheetComponent *sheetLocal = m_scene->Get<SpriteSheetComponent>(m_scene->m_selectedEntity);
                if (!sheetLocal && !grid)
                {
                    break;
                }

                if (grid)
                {
                    float scale = 2.0f; // Scale factor (adjust as needed)

                    int gridPositionX = m_imguiLayer->viewportMousePos.x / scale;
                    int gridPositionY = m_imguiLayer->viewportMousePos.y / scale;
                    if (gridPositionX < 0 || gridPositionY < 0 || gridPositionX > grid->cols || gridPositionY > grid->rows)
                    {
                        break;
                    }
                    grid->UpdateCircle(gridPositionX, gridPositionY);
                }

                if (sheetLocal && sheetLocal->importedSheet)
                {
                    int gridPositionX = m_imguiLayer->viewportMousePos.x / m_board->m_tileSize;
                    int gridPositionY = m_imguiLayer->viewportMousePos.y / m_board->m_tileSize;
                    sheetLocal->spriteSheet->AddTileId(sheetLocal->selectedTileId, gridPositionX, gridPositionY);
                    m_scene->CreateSpriteSheetTile(gridPositionX, gridPositionY, physicsWorld, m_board);
                }
            }
        }
    }
    return true;
}