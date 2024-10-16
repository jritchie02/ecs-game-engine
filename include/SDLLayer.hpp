#pragma once

#include <SDL3/SDL.h>
#include <cstdio>
#include "Particle.hpp"

/**
 * @brief SDL Rendering
 *
 */
class SDLLayer
{
public:
    /**
     * @brief Initialize SDL Window and Renderer
     *
     */
    SDLLayer()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0)
        {
            printf("Error: SDL_Init(): %s\n", SDL_GetError());
        }

        // Enable native IME.
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

        // Create window with SDL_Renderer graphics context
        Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
        m_window = SDL_CreateWindow("Engine", 1920, 1080, window_flags);

        if (m_window == nullptr)
        {
            printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
            // Handle error
        }

        m_renderer = SDL_CreateRenderer(m_window, nullptr, SDL_RENDERER_ACCELERATED);
        if (m_renderer == nullptr)
        {
            SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
            // Handle error
        }
        SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(m_window);
    }

    /**
     * @brief Destroy SDL Window and Renderer
     *
     */
    ~SDLLayer()
    {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    /**
     * @brief Get the Renderer object
     *
     * @return SDL_Renderer*
     */
    SDL_Renderer *GetRenderer() const
    {
        return m_renderer;
    }

    /**
     * @brief Get the Window object
     *
     * @return SDL_Window*
     */
    SDL_Window *GetWindow() const
    {
        return m_window;
    }

    /**
     * @brief Draw a rectangle at the given position with the given width, height, and color
     *
     * @param x
     * @param y
     * @param width
     * @param height
     * @param color
     */
    void DrawRectangle(const int x, const int y, const int width, const int height, const SDL_Color color)
    {
        SDL_FRect rect{static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)};
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderRect(m_renderer, &rect);
    }

    /**
     * @brief Draw a texture at the given position with the given width and height
     *
     * @param texture
     * @param x
     * @param y
     * @param width
     * @param height
     */
    void DrawTexture(SDL_Texture *const texture, const float x, const float y, const float width, const float height)
    {
        SDL_FRect dstRect{static_cast<float>(x), static_cast<float>(y), width, height};
        SDL_RenderTexture(m_renderer, texture, nullptr, &dstRect);
    }

    /**
     * @brief Draw a texture taken from the sample at a given position with the given width, height
     *
     * @param texture
     * @param x
     * @param y
     * @param width
     * @param height
     * @param sampleX
     * @param sampleY
     * @param sampleWidth
     * @param sampleHeight
     */
    void DrawTextureSample(SDL_Texture *const texture, const float x, const float y, const float width, const float height,
                           const float sampleX, const float sampleY, const float sampleWidth, const float sampleHeight)
    {
        SDL_FRect srcRect{sampleX, sampleY, sampleWidth, sampleHeight};
        SDL_FRect dstRect{x, y, width, height};
        SDL_RenderTexture(m_renderer, texture, &srcRect, &dstRect);
    }

    void DrawGrid(int rows, int cols, std::vector<particle_t> *gridData)
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                // SDL Draw Pixel
                const particle_t &particle = gridData->at(i * cols + j);
                SDL_SetRenderDrawColor(m_renderer, particle.color.r, particle.color.g, particle.color.b, particle.color.a);
                SDL_RenderPoint(m_renderer, j, i);
            }
        }
    }

private:
    SDL_Renderer *m_renderer;
    SDL_Window *m_window;
};
