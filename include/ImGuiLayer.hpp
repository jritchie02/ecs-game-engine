#pragma once

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>
#include <SDL3/SDL.h>
#include <stdio.h>
#include "SceneView.hpp"
#include "Scene.hpp"
#include "Spritesheet.hpp"

/**
 * @brief DearImGUI Rendering Logic
 *
 */
class ImGuiLayer
{
public:
    /**
     * @brief Construct a new Im Gui Layer object
     *
     * @param scene The scene object
     * @param renderer The SDL renderer
     * @param window The SDL window
     * @param board The application board
     */
    ImGuiLayer(Scene *scene, SDL_Renderer *renderer, SDL_Window *window, Board *board);

    /**
     * @brief Destroy the Im Gui Layer object
     *
     */
    ~ImGuiLayer();

    /**
     * @brief Draw the ImGui Layer before SDL rendering
     *
     */
    void PreDraw(bool showDebug, SDL_Texture *m_viewportTexture);

    /**
     * @brief Draw the ImGui Layer after SDL rendering
     *
     */
    void Draw() const;

    /**
     * @brief Set the Style of the ImGui Layer
     *
     */
    static void SetStyle();

    ImVec2 viewportMousePos;

private:
    Scene *const m_scene;
    SDL_Renderer *const m_renderer;
    SDL_Window *const m_window;
    Board *const m_board;

    /**
     * @brief Render all entites in the scene
     *
     */
    void DisplaySceneHierarchy();

    /**
     * @brief Render the components of the selected entity
     *
     */
    void DisplayEntity(const EntityID &entityID);

    /**
     * @brief Render the list of game entites in the scene
     *
     */
    void DisplayEntityProperties();

    /**
     * @brief Render SpriteSheet Input section
     *
     * @param sheetLocal
     */
    void DisplayTileImport(SpriteSheetComponent *sheetLocal);

    /**
     * @brief Render SpriteSheet selectable Tiles
     *
     * @param sheetLocal
     */
    void DisplaySpriteSheet(SpriteSheetComponent *sheetLocal);

    /**
     * @brief Render Vector2 Control panel to modify float values with mouse click
     * Code derived from The Cherno ImGui Tutorial
     * https://www.youtube.com/watch?v=8JqcXYbzPJc&t=137s
     *
     * @param label The label of the control
     * @param value1 The first float value
     * @param value2 The second float value
     * @param resetValue The reset value
     * @param columnWidth The width of the column
     */
    static void DisplayVec2Control(const std::string &label, float &value1, float &value2, float resetValue = 0.0f, float columnWidth = 100.0f);
};