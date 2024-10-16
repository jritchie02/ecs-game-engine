#include "Spritesheet.hpp"

SpriteSheet::SpriteSheet(const std::string &filePath, Board *board, SDL_Renderer *renderer, const std::string &spriteSheetPath)
{
    m_fileName = spriteSheetPath;
    m_board = board;

    // Open the file (level)
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    // Read the file path from the first line
    std::string line;
    std::getline(file, line);
    m_fileName = line;

    // Read the tile size (8, 16, 32 px) from the second line
    std::string lineTileSize;
    std::getline(file, lineTileSize);
    m_tileSize = std::stoi(lineTileSize);

    // Import the texture and initialize other members
    Import(board, renderer);

    // Read tile IDs from the rest of the file
    int row = 0;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        int col = 0;
        int tileId;
        while (iss >> tileId)
        {
            if (tileId != -1)
            {
                AddTileId(tileId, col, row);
            }

            col++;
        }
        row++;
    }

    file.close();
}

void SpriteSheet::Import(Board *board, SDL_Renderer *renderer)
{
    auto sprite_texture = ResourceManager::Instance().LoadTexture(renderer, m_fileName);
    m_board = board;

    if (!sprite_texture)
    {
        std::cout << "Failed to load texture: " << m_fileName << std::endl;
    }
    m_tilesetTexture = sprite_texture.get();

    int textureWidth, textureHeight;
    SDL_QueryTexture(sprite_texture.get(), NULL, NULL, &textureWidth, &textureHeight);

    m_tilesetCols = std::round(textureWidth / m_tileSize);
    m_tilesetRows = std::round(textureHeight / m_tileSize);
    m_tileIds.resize(m_board->m_boardWidth * m_board->m_boardHeight, -1);
}

void SpriteSheet::ExportToFile(const std::string &exportFilePath)
{
    std::ofstream file(exportFilePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for export: " << exportFilePath << std::endl;
        return;
    }

    // Write the sprite sheet file path
    file << m_fileName << std::endl;

    // Write the tile size
    file << m_tileSize << std::endl;

    // Write the tile IDs
    for (int row = 0; row < m_board->m_boardHeight; ++row)
    {
        for (int col = 0; col < m_board->m_boardWidth; ++col)
        {
            int tileId = m_tileIds.at(col + row * m_board->m_boardWidth);
            file << tileId << " ";
        }
        file << std::endl;
    }

    file.close();
}

void SpriteSheet::AddTileId(int id, int xpos, int ypos)
{
    size_t index = xpos + ypos * m_board->m_boardWidth;

    if (index < m_tileIds.size())
    {
        m_tileIds.at(index) = id;
    }
}

void SpriteSheet::Render(SDLLayer *const sdlLayer) const
{
    // For each tile in the m_board draw a texture component at its position
    for (int row = 0; row < m_board->m_boardHeight; row++)
    {
        for (int col = 0; col < m_board->m_boardWidth; col++)
        {
            int index = col + row * m_board->m_boardWidth;
            int tile_id = m_tileIds.at(index);

            if (tile_id != -1)
            {
                int tile_x = tile_id % m_tilesetCols;
                int tile_y = tile_id / m_tilesetCols;

                sdlLayer->DrawTextureSample(m_tilesetTexture, col * m_board->m_tileSize, row * m_board->m_tileSize, m_board->m_tileSize, m_board->m_tileSize, tile_x * m_tileSize, tile_y * m_tileSize, m_tileSize, m_tileSize);
            }
        }
    }
}