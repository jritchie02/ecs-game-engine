#pragma once

#include "Scene.hpp"
#include "SceneView.hpp"
#include "Board.hpp"

/**
 * @brief Physics System
 *
 */
class PhysicsSystem
{
public:
    /**
     * @brief Initialize the Physics System
     *
     * @param scene Current scene
     * @param board Current game board
     */
    PhysicsSystem(Scene *scene, Board *board)
        : m_scene(scene), m_board(board)
    {
    }

    /**
     * @brief Update the Physics System
     *
     */
    void Update() const;

private:
    Scene *const m_scene;
    Board *const m_board;

    /**
     * @brief Update entities with Box2D and Input components based on input state
     *
     */
    void HandlePlayerMovement() const;

    /**
     * @brief Check if any colliders are triggered
     *
     */
    void CheckTriggers() const;

    /**
     * @brief Update the transforms of entities based on the physics simulation
     *
     */
    void UpdateTransforms() const;

    void UpdateSand(int row, int col, GridSimulationComponent *grid, std::vector<particle_t> &newGrid) const;

    void UpdateWater(int row, int col, GridSimulationComponent *grid, std::vector<particle_t> &newGrid) const;
};
