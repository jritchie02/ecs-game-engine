#include "ImGuiLayer.hpp"

ImGuiLayer::ImGuiLayer(Scene *scene, SDL_Renderer *renderer, SDL_Window *window, Board *board)
    : m_scene(scene), m_renderer(renderer), m_window(window), m_board(board)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer3_Init(m_renderer);

    // Set the UI Style
    SetStyle();
}

ImGuiLayer::~ImGuiLayer()
{
    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::PreDraw(bool showDebug, SDL_Texture *m_viewportTexture)
{
    // Our state
    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Setup Docking
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    bool open = true;
    bool *p_open = &open;
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Render Colliders", "", m_scene->m_showColliders))
            {
                m_scene->m_showColliders = !m_scene->m_showColliders;
            }
            if (ImGui::MenuItem("Render Grid", "", m_scene->m_showGrid))
            {
                m_scene->m_showGrid = !m_scene->m_showGrid;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    // Step 1: Create an SDL Texture for ImGui to render to, save relative mouse position
    float scale = 2.0f; // Scale factor (adjust as needed)

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    viewportMousePos = ImVec2(ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x, ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y);
    ImGui::Image((ImTextureID)m_viewportTexture, ImVec2(m_board->m_boardWidth * m_board->m_tileSize * scale, m_board->m_boardHeight * m_board->m_tileSize * scale));
    ImGui::End();
    ImGui::PopStyleVar();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    if (showDebug)
    {
        DisplaySceneHierarchy();
        DisplayEntityProperties();
    }

    ImGui::End();

    // New Frame Initialization
    ImGui::Render();
    SDL_SetRenderDrawColor(m_renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(m_renderer);
}

void ImGuiLayer::Draw() const
{
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer);
}

void ImGuiLayer::SetStyle()
{
    ImGuiIO &io = ImGui::GetIO();
    // Load Fonts
    io.Fonts->AddFontFromFileTTF("../assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("../assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

    // Handle colors
    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

    // Headers
    colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.150f, 0.151f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.3805f, 0.381f, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
}

void ImGuiLayer::DisplaySceneHierarchy()
{
    ImGui::Begin("Scene Hierarchy");
    ImGuiIO &io = ImGui::GetIO();
    for (EntityID ent : SceneView<>(*m_scene))
    {
        TransformComponent *transform = m_scene->Get<TransformComponent>(ent);

        if (transform && transform->hasParent)
        {
            // Skip rendering this entity
            continue;
        }

        DisplayEntity(ent);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::Text("SpriteSheet Tile Entites Not Shown");

    ImGui::End();
}

void ImGuiLayer::DisplayEntity(const EntityID &entityID)
{
    bool isSelected = (m_scene->m_selectedEntity == entityID);
    ImGui::Selectable(("Entity ID: " + std::to_string(entityID)).c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns);
    if (ImGui::IsItemClicked())
    {
        m_scene->m_selectedEntity = entityID;
    }
}

void ImGuiLayer::DisplayEntityProperties()
{
    EntityID ent = m_scene->m_selectedEntity;

    if (ent == static_cast<unsigned long long>(-1))
    {
        return;
    }

    ImGui::Begin("Entity Properties");

    TransformComponent *transform = m_scene->Get<TransformComponent>(ent);

    Box2DColliderComponent *collider = m_scene->Get<Box2DColliderComponent>(ent);
    SpriteComponent *sprite = m_scene->Get<SpriteComponent>(ent);
    SpriteSheetComponent *spriteSheet = m_scene->Get<SpriteSheetComponent>(ent);
    InputComponent *input = m_scene->Get<InputComponent>(ent);
    GridSimulationComponent *grid = m_scene->Get<GridSimulationComponent>(ent);

    if (transform)
    {
        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
        {
            DisplayVec2Control("Transform", transform->x, transform->y);
            ImGui::Text("Has Parent: %s", transform->hasParent ? "true" : "false");

            if (collider)
            {
                // Manually set the position of the collider
                collider->body->SetTransform(b2Vec2(transform->x / m_board->m_tileSize, transform->y / m_board->m_tileSize), 0.0f);
            }

            ImGui::TreePop();
        }
    }

    if (collider)
    {
        if (ImGui::TreeNodeEx("Box2D Collider", ImGuiTreeNodeFlags_DefaultOpen, "Box2D Collider"))
        {
            // Display the position, scale,
            ImGui::Text("Position: (%.2f, %.2f)", collider->body->GetPosition().x, collider->body->GetPosition().y);
            ImGui::TreePop();
        }
    }

    if (sprite)
    {
        if (ImGui::TreeNodeEx("Sprite", ImGuiTreeNodeFlags_DefaultOpen, "Sprite"))
        {
            ImGui::Text("File Path: %s", sprite->filePath.c_str());
            DisplayVec2Control("Size", sprite->width, sprite->height, 1);
            ImGui::TreePop();
        }
    }

    if (spriteSheet)
    {
        if (ImGui::TreeNodeEx("SpriteSheet", ImGuiTreeNodeFlags_DefaultOpen, "SpriteSheet"))
        {
            DisplayTileImport(spriteSheet);
            DisplaySpriteSheet(spriteSheet);

            // Render a text input and button to export the spritesheet
            static char export_file_path[128] = "";
            ImGui::InputText("Export File Path", export_file_path, 128);

            if (ImGui::Button("Export Level"))
            {
                spriteSheet->spriteSheet->ExportToFile(export_file_path);
            }
            ImGui::TreePop();
        }
    }

    if (input)
    {
        if (ImGui::TreeNodeEx("Input", ImGuiTreeNodeFlags_DefaultOpen, "Input"))
        {
            ImGui::Text("Left Press: %s", input->leftPress ? "true" : "false");
            ImGui::Text("Right Press: %s", input->rightPress ? "true" : "false");
            ImGui::Text("Space Press: %s", input->spacePress ? "true" : "false");
            ImGui::TreePop();
        }
    }

    if (grid)
    {
        if (ImGui::TreeNodeEx("Grid Simulation", ImGuiTreeNodeFlags_DefaultOpen, "Grid Simulation"))
        {
            ImGui::Text("Rows: %d", grid->rows);
            ImGui::Text("Cols: %d", grid->cols);

            // Button for SAND particle type
            if (ImGui::Button("Sand"))
            {
                grid->brushType = ParticleType::SAND;
            }

            // Button for WATER particle type
            if (ImGui::Button("Water"))
            {
                grid->brushType = ParticleType::WATER;
            }

            // Button for STONE particle type
            if (ImGui::Button("Stone"))
            {
                grid->brushType = ParticleType::STONE;
            }

            ImGui::TreePop();
        }
    }

    ImGui::End(); // End the ImGui window
}

void ImGuiLayer::DisplayTileImport(SpriteSheetComponent *sheetLocal)
{
    ImGui::Text("Enter image tile size in px and file path:");
    static char file_path[128] = "";
    ImGui::InputText("File Path", file_path, 128);

    if (sheetLocal->tileMapFileError)
    {
        ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color (RGBA)
        ImGui::TextColored(redColor, "Error: %s", "Unable to Open File with given path!");
    }

    static char tile_size[3] = "";
    ImGui::InputText("Tile Size (px)", tile_size, 3);

    if (sheetLocal->tileMapSizeError)
    {
        ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color (RGBA)
        ImGui::TextColored(redColor, "Error: %s", "Tile Size should be 8, 16, or 32!");
    }

    if (ImGui::Button("Import Image"))
    {
        try
        {
            int tile_size_int = std::stoi(tile_size);
            sheetLocal->spriteSheet = new SpriteSheet(file_path, tile_size_int);
            sheetLocal->spriteSheet->Import(m_board, m_renderer);

            sheetLocal->tileMapSizeError = false;
            sheetLocal->tileMapFileError = false;
            sheetLocal->importedSheet = true;
        }
        catch (const std::exception &ex)
        {
            // If an integer is not entered display tile size error
            if (dynamic_cast<const std::invalid_argument *>(&ex))
            {
                sheetLocal->tileMapSizeError = true;
            }
            sheetLocal->importedSheet = false;
        }
    }
}

void ImGuiLayer::DisplaySpriteSheet(SpriteSheetComponent *sheetLocal)
{
    if (sheetLocal && sheetLocal->importedSheet)
    {
        // Create a child window with scrolling
        ImGui::BeginChild("Tileset", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

        static int value = 32;   // Initial scale value
        const int minValue = 8;  // Minimum scale value
        const int maxValue = 64; // Maximum scale value

        ImGui::SliderInt("Scale", &value, minValue, maxValue);

        auto tileset_Texture = sheetLocal->spriteSheet->GetTileSetTexture();
        int tileset_cols = sheetLocal->spriteSheet->GetSheetWidth();
        int tileset_rows = sheetLocal->spriteSheet->GetSheetHeight();

        ImTextureID tilesetTextureId = (ImTextureID)(intptr_t)tileset_Texture; // Cast the texture ID to ImTextureID

        ImVec2 scale_factor = ImVec2(value, value);

        ImGui::BeginTable("TilesetTable", tileset_cols, ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY);
        for (int row = 0; row < tileset_rows; row++)
        {
            ImGui::TableNextRow();
            for (int col = 0; col < tileset_cols; col++)
            {
                ImGui::TableNextColumn();

                // Create a unique ID for the button using row and column indices
                ImGui::PushID(row * tileset_cols + col);

                ImVec2 uv0 = ImVec2(col / (float)tileset_cols, row / (float)tileset_rows);
                ImVec2 uv1 = ImVec2((col + 1) / (float)tileset_cols, (row + 1) / (float)tileset_rows);

                int current_id = row * tileset_cols + col;
                bool selected = sheetLocal->selectedTileId == current_id;

                if (selected)
                {
                    // You can adjust the border color and width here
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
                }

                if (ImGui::ImageButton((ImTextureID)tilesetTextureId, scale_factor, uv0, uv1, 0, ImVec4(0, 0, 0, 1), ImVec4(1, 1, 1, 1)))
                {
                    sheetLocal->selectedTileId = row * tileset_cols + col;
                }

                if (selected)
                {
                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();
                }

                ImGui::PopID();
            }
        }
        ImGui::EndTable();
        ImGui::EndChild();
    }
}

void ImGuiLayer::DisplayVec2Control(const std::string &label, float &value1, float &value2, float resetValue, float columnWidth)
{
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.05f, 0.1f, 1.0f));
    if (ImGui::Button("X", buttonSize))
    {
        value1 = resetValue;
    }

    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &value1, 0.1f);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));

    if (ImGui::Button("Y", buttonSize))
    {
        value2 = resetValue;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &value2, 0.1f);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PopStyleVar();
    ImGui::Columns(1);

    ImGui::PopID();
}
