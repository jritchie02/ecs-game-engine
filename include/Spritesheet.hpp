#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Board.hpp"
#include "ResourceManager.hpp"
#include "SDLLayer.hpp"

/**
 * @brief Sprite Sheet class
 *
 */
class SpriteSheet
{
public:
    /**
     * @brief Construct a new Sprite Sheet object
     *
     */
    SpriteSheet() : m_tileSize(0), m_tilesetCols(0), m_tilesetRows(0) {}

    /**
     * @brief Construct a new Sprite Sheet object
     *
     * @param fileName
     * @param tileSize
     */
    SpriteSheet(std::string fileName, int tileSize)
        : m_fileName(fileName), m_tileSize(tileSize) {}

    /**
     * @brief Construct a new Sprite Sheet object with prebuilt board
     *
     * @param prebuiltBoardPath
     * @param board
     * @param renderer
     * @param spriteSheetPath
     */
    SpriteSheet(const std::string &prebuiltBoardPath, Board *board, SDL_Renderer *renderer, const std::string &spriteSheetPath);

    /**
     * @brief Load the sprite sheet image
     *
     * @param board
     * @param renderer
     */
    void Import(Board *board, SDL_Renderer *renderer);

    /**
     * @brief Export the sprite sheet image as .txt file
     *
     * @param exportFilePath
     */
    void ExportToFile(const std::string &exportFilePath);

    /**
     * @brief Add tile id to the vertex array
     *
     * @param id
     * @param xpos
     * @param ypos
     */
    void AddTileId(int id, int xpos, int ypos);

    /**
     * @brief Render the sprite sheet
     *
     * @param sdlLayer
     */
    void Render(SDLLayer *const sdlLayer) const;

    // Getters to Access SpriteSheet info
    SDL_Texture *GetTileSetTexture() const
    {
        return m_tilesetTexture;
    }

    int GetTileSize() const
    {
        return m_tileSize;
    }

    int GetSheetWidth() const
    {
        return m_tilesetCols;
    }

    int GetSheetHeight() const
    {
        return m_tilesetRows;
    }

    std::vector<int> GetTileIds() const
    {
        return m_tileIds;
    }

private:
    SDL_Texture *m_tilesetTexture; // Sprite sheet image texture
    std::string m_fileName;
    int m_tileSize; // width/height of single tile in pixels
    int m_tilesetCols;
    int m_tilesetRows;

    std::vector<int> m_tileIds; // Texture Mapping IDs

    Board *m_board;
};
