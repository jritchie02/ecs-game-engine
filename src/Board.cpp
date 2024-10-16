#include "Board.hpp"
#include <iostream>

void Board::Render(SDL_Renderer *const renderer)
{
    for (int x = 0; x < m_boardHeight; x++)
    {
        for (int y = 0; y < m_boardWidth; y++)
        {
            // Draw a rectangle with a border
            SDL_FRect rect = {static_cast<float>(y * m_tileSize), static_cast<float>(x * m_tileSize), static_cast<float>(m_tileSize), static_cast<float>(m_tileSize)};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderRect(renderer, &rect);
        }
    }
}