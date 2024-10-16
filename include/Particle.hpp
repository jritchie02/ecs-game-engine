#pragma once
#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <cmath>

/**
 * @brief Enum for the particle types
 * 
 * 
 */
enum ParticleType { 
    EMPTY,
    SAND, 
    WATER,
    STONE
};

typedef struct particle_t
{
    ParticleType id;
    float life_time;
    b2Vec2 velocity;
    SDL_Color color;
    bool has_been_updated_this_frame;
} particle_t;

