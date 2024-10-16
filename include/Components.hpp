#pragma once

#include <string>
#include <memory>
#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <functional>
#include <cmath>
#include <random>

/**
 * @brief Transform Component
 *
 */
struct TransformComponent
{
    float x{0.0f};         // X position in pixels
    float y{0.0f};         // Y position in pixels
    bool hasParent{false}; // Does this entity have a parent entity
};

/**
 * @brief Box2D Collider Component
 *
 */
struct Box2DColliderComponent
{
    b2BodyDef bodyDef;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    b2Body *body;
    bool isTrigger{false};

    // Callback function for collision
    std::function<void()> onCollisionEnter;

    // Function to call the collision callback
    void OnCollisionEnter()
    {
        if (onCollisionEnter)
        {
            onCollisionEnter();
        }
    }
};

/**
 * @brief Sprite Component
 *
 */
struct SpriteComponent
{
    std::string filePath;                 // Path to the sprite image file
    float width;                          // Width of the sprite
    float height;                         // Height of the sprite
    std::shared_ptr<SDL_Texture> texture; // The texture of the sprite
};

/**
 * @brief Input Component
 *
 */
struct InputComponent
{
    bool leftPress{false};  // Is left arrow key down
    bool rightPress{false}; // Is Right arrow key down
    bool spacePress{false}; // Space key pressed this frame
    float speed{0.1f};      // Movement speed
    float jumpSpeed{-5.5f}; // Jump speed
};

/**
 * @brief Sprite Sheet Component
 *
 */
struct SpriteSheetComponent
{
    SpriteSheet *spriteSheet;
    int selectedTileId = 0;
    bool importedSheet = false;
    bool tileMapSizeError = false;
    bool tileMapFileError = false;
};

/**
 * @brief Grid Simulation Component
 *
 */
struct GridSimulationComponent
{
    int rows = 256;
    int cols = 256;
    ParticleType brushType = ParticleType::SAND;

    // Grid data
    std::vector<particle_t> *gridData = new std::vector<particle_t>(rows * cols);

    // Destructor
    ~GridSimulationComponent()
    {
        delete gridData;
    }

    // Helper for setting the grid data
    void SetGridData(int x, int y, particle_t value)
    {
        gridData->at(x * cols + y) = value;
    }

    bool InBounds(int x, int y)
    {
        return x >= 0 && x < rows && y >= 0 && y < cols;
    }

    // Helper to check if empty
    bool IsEmpty(size_t index)
    {
        // if the index is out of bounds, return false
        if (index < 0 || index >= gridData->size())
        {
            return false;
        }
        const particle_t &particle = gridData->at(index);
        // Assuming particle_t has a default constructor that creates an "empty" or "null" state
        return particle.id == ParticleType::EMPTY;
    }

    void UpdateCircle(int xCenter, int yCenter)
    {
        const SDL_Color sandMaterial = {190, 140, 80, 255};
        const SDL_Color waterMaterial = {0, 110, 255, 255};
        const SDL_Color stoneMaterial = {100, 100, 100, 255};
        const SDL_Color emptyMaterial = {0, 0, 0, 0};

        // switch to determine the color
        SDL_Color material;
        switch (brushType)
        {
        case ParticleType::SAND:
            material = sandMaterial;
            break;
        case ParticleType::WATER:
            material = waterMaterial;
            break;
        case ParticleType::STONE:
            material = stoneMaterial;
            break;
        case ParticleType::EMPTY:
            material = emptyMaterial;
            break;
        default:
            material = emptyMaterial;
            break;
        }

        // Update the grid data in a cirlce
        int radius = 5;
        for (int i = -radius; i <= radius; i++)
        {
            for (int j = -radius; j <= radius; j++)
            {
                if (i * i + j * j <= radius * radius)
                {
                    // Set the grid data to a new value, with random color
                    SetGridData((yCenter + i), (xCenter + j), {brushType, 1.0f, b2Vec2(0.0f, 0.0f), varyColor(material), false});
                }
            }
        }
    }

    // Function to generate a random number within a range
    int random(int min, int max)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    // Function to vary the color in RGB mode
    SDL_Color varyColor(SDL_Color color)
    {
        // Vary each component of the color
        int r = color.r + random(-20, 20);
        int g = color.g + random(-20, 20);
        int b = color.b + random(-20, 20);

        // Ensure the values are in the valid range [0, 255]
        r = std::max(0, std::min<int>(255, r));
        g = std::max(0, std::min<int>(255, g));
        b = std::max(0, std::min<int>(255, b));
        
        // Return the new color
        return {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), color.a};
    }
};
