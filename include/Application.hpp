// Application.hpp
#pragma once

#include <vector>
#include <memory>
#include <box2d/box2d.h>
#include "Scene.hpp"
#include "SceneView.hpp"
#include "ResourceManager.hpp"
#include "Board.hpp"
#include "RenderingSystem.hpp"
#include "PhysicsSystem.hpp"
#include "InputSystem.hpp"

/**
 * @brief The main application class
 *
 */
class Application
{
public:
    /**
     * @brief Construct a new Application object
     *
     * @param board Game board
     * @param renderDebug Render ImGUI debug panels
     */
    Application(Board *board, bool renderDebug)
        : m_board(board),
          m_scene(),
          m_sceneView(m_scene),
          m_physicsWorld(new b2World(gravity)),
          m_renderingSystem(&m_scene, m_board),
          m_physicsSystem(&m_scene, m_board),
          m_inputSystem(&m_scene, m_renderingSystem.GetSDLLayer(), m_board, m_renderingSystem.GetImGuiLayer())
    {
        // Initialize Render Variables
        m_scene.m_showGrid = renderDebug;
        m_scene.m_showColliders = renderDebug;
    }

    /**
     * @brief Destroy the Application object
     *
     */
    ~Application();

    /**
     * @brief Handle user input events
     *
     * @param deltaTime time since last frame
     * @return true if application should quit
     * @return false if application should continue
     */
    bool Input(float deltaTime) const;

    /**
     * @brief Update the application state
     *
     * @param deltaTime time since last frame
     */
    void Update(float deltaTime) const;

    /**
     * @brief Render the application
     *
     */
    void Render();

    /**
     * @brief The main game loop
     *
     * @param targetFPS the target frames per second
     */
    void Loop(float targetFPS);

    /**
     * @brief Add a Sprite Component to an entity
     *
     * @param entity EntityID
     * @param filePath Filepath of sprite
     * @param width Width in Board Units
     * @param height Height in Board Units
     */
    void AddSprite(const EntityID &entity, const std::string filePath, const float width, const float height);

    /**
     * @brief Add a Box2D component to an entity
     *
     * @param entity EntityID
     * @param x Position in Pixels
     * @param y Position in Pixels
     * @param isStatic
     * @param isTrigger
     */
    void AddBox2D(const EntityID &entity, const float x, const float y, const float width, const float height, const bool isStatic, const bool isTrigger);

    /**
     * @brief Add a Box2D component to an entity
     *
     * @param levelPath
     * @param spritesheetPath
     */
    void
    ImportSpritesheetLevel(const std::string levelPath, const std::string spritesheetPath);

    const Scene &GetScene() { return m_scene; }

    bool m_isRunning = true;

private:
    b2Vec2 gravity{0.0f, 4.4f};
    Board *m_board;
    Scene m_scene;

    const SceneView<> m_sceneView;
    b2World *const m_physicsWorld;

    // Systems
    RenderingSystem m_renderingSystem;
    const PhysicsSystem m_physicsSystem;
    const InputSystem m_inputSystem;
};
