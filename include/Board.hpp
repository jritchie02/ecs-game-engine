#pragma once

#include <SDL3/SDL.h>
#include "Particle.hpp"
/**
 * @brief Size and scale of game world
 *
 */
class Board
{
public:
    /**
     * @brief Construct a new Board object
     *
     */
    Board() : m_tileSize(0), m_boardWidth(0), m_boardHeight(0) {}

    /**
     * @brief Construct a new Board object
     *
     * @param tileSize size of each tile in pixels
     * @param width width of board in grid units
     * @param height height of board in grid units
     */
    Board(const int tileSize, const int width, const int height) : m_tileSize(tileSize), m_boardWidth(width), m_boardHeight(height) {}

    /**
     * @brief Render the grid tiles of the board
     *
     * @param renderer SDL renderer
     */
    void Render(SDL_Renderer *const renderer);

    const int m_tileSize;
    const int m_boardWidth;
    const int m_boardHeight;
};
