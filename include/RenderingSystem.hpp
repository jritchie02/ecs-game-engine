#pragma once

#include "Scene.hpp"
#include "SDLLayer.hpp"
#include "SceneView.hpp"
#include "ImGuiLayer.hpp"

/**
 * @brief Rendering System
 *
 */
class RenderingSystem
{
public:
    /**
     * @brief Initialize the Rendering System
     *
     * @param scene Current scene
     * @param sdlLayer SDL Layer
     * @param board Game board
     */
    RenderingSystem(Scene *scene, Board *board)
        : m_scene(scene), m_board(board)
    {
    }

    /**
     * @brief Destroy the Rendering System
     *
     */
    ~RenderingSystem()
    {
        delete m_imguiLayer;
        delete m_sdlLayer;
    }

    /**
     * @brief Renders the ImGui Editor and the SDL scene to the ImGui Window as a texture
     *
     */
    void Render();

    /**
     * @brief Get an SDL Texture of the SDL Viewport
     *
     * @return SDL_Texture* The SDL Texture
     */
    SDL_Texture *GetWindowTexture();

    /**
     * @brief Render the SDL scene to the renderer
     *
     * @param showColliders Render colliders to SDL
     */
    void SDLRender(bool &showColliders) const;

    /**
     * @brief Get the Im Gui Layer object
     *
     * @return ImGuiLayer*
     */
    ImGuiLayer *GetImGuiLayer() const { return m_imguiLayer; }

    /**
     * @brief Get the SDL Layer object
     *
     * @return SDL_Layer*
     */
    SDLLayer *GetSDLLayer() const { return m_sdlLayer; }

private:
    Scene *const m_scene;
    Board *const m_board;
    SDLLayer *const m_sdlLayer = new SDLLayer();
    ImGuiLayer *m_imguiLayer = new ImGuiLayer(m_scene, m_sdlLayer->GetRenderer(), m_sdlLayer->GetWindow(), m_board);
    SDL_Texture *m_viewportTexture;
};