#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

// Functor
struct TextureFunctorDeleter
{
    void operator()(SDL_Texture *texture) const
    {
        SDL_DestroyTexture(texture);
    }
};

// Function Declarations
std::shared_ptr<SDL_Texture> make_shared_texture(SDL_Renderer *renderer, SDL_Surface *pixels);

/**
 * @brief Singleton asset resource manager
 *
 */
struct ResourceManager
{

    /**
     * @brief Get the Instance object
     *
     * @return ResourceManager&
     */
    static ResourceManager &Instance()
    {
        if (nullptr == mInstance)
        {
            mInstance = new ResourceManager;
        }

        return *mInstance;
    }

    /**
     * @brief Load a texture from file
     *
     * @param renderer SDL renderer
     * @param filepath Relative file path from the executable
     * @return std::shared_ptr<SDL_Texture>
     */
    std::shared_ptr<SDL_Texture> LoadTexture(SDL_Renderer *renderer, std::string filepath)
    {
        if (mTextureResouces.find(filepath) == mTextureResouces.end())
        {
            SDL_Surface *pixels = SDL_LoadBMP(filepath.c_str());

            if (nullptr == pixels)
            {
                SDL_Log("Could not load image %s: %s", filepath.c_str(), SDL_GetError());
                return nullptr;
            }
            SDL_SetSurfaceColorKey(pixels, SDL_TRUE, SDL_MapRGB(pixels->format, 0xFF, 0, 0xFF));
            std::shared_ptr<SDL_Texture> texture = make_shared_texture(renderer, pixels);

            mTextureResouces.insert({filepath, texture});

            SDL_DestroySurface(pixels);
            SDL_Log("Created new resource %s", filepath.c_str());
        }
        else
        {
            SDL_Log("Reused resource %s", filepath.c_str());
        }

        return mTextureResouces[filepath];
    }

private:
    /**
     * @brief Construct a new Resource Manager object
     *
     */
    ResourceManager()
    {
    }

    inline static ResourceManager *mInstance{nullptr};

    std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> mTextureResouces;
};
