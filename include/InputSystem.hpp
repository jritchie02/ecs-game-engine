#pragma once

#include "Scene.hpp"
#include "SDLLayer.hpp"
#include "SceneView.hpp"
#include "ImGuiLayer.hpp"
#include "Board.hpp"

/**
 * @brief Input handling system
 *
 */
class InputSystem
{
public:
    /**
     * @brief Initialize the Input System based on current scene
     *
     * @param scene Current scene
     * @param sdlLayer SDL Layer
     * @param board Game board
     * @param imguiLayer ImGui Layer
     */
    InputSystem(Scene *scene, SDLLayer *sdlLayer, Board *board, ImGuiLayer *imguiLayer)
        : m_scene(scene), m_sdlLayer(sdlLayer), m_board(board), m_imguiLayer(imguiLayer)
    {
    }

    /**
     * @brief Handle user input events
     *
     * @param deltaTime Time since last frame
     * @param physicsWorld Box2D physics world
     * @return true Application should quit
     * @return false Application should continue
     */
    bool Input(b2World *physicsWorld) const;

private:
    Scene *const m_scene;
    SDLLayer *const m_sdlLayer;
    Board *const m_board;
    ImGuiLayer *const m_imguiLayer;
};
