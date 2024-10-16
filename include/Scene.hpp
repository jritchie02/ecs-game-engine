#pragma once

#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Spritesheet.hpp"
#include "Components.hpp"
#include "Constants.hpp"

/**
 * @brief Bitmask storage for each entities components
 *
 */
struct ComponentPool
{
    /**
     * @brief Construct a new Component Pool object based on Component size
     *
     * @param elementsize
     */
    ComponentPool(size_t elementsize)
    {
        // We'll allocate enough memory to hold MAX_ENTITIES, each with element size
        elementSize = elementsize;
        pData = new char[elementSize * MAX_ENTITIES];
    }

    /**
     * @brief Destroy the Component Pool object
     *
     */
    ~ComponentPool()
    {
        delete[] pData;
    }

    /**
     * @brief Get the component value at the desired index
     *
     * @param index
     * @return void*
     */
    inline void *get(size_t index)
    {
        // looking up the component at the desired index
        return pData + index * elementSize;
    }

    char *pData{nullptr};
    size_t elementSize{0};
};

/**
 * @brief Scene object to hold all entities and components
 *
 */
struct Scene
{
    /**
     * @brief Entity Description
     *
     */
    struct EntityDesc
    {
        EntityID id;
        ComponentMask mask;
    };

    /**
     * @brief Construct a new Entity object
     *
     * @return EntityID
     */
    EntityID NewEntity();

    /**
     * @brief Assign a component to an entity
     *
     * @tparam T Component
     * @param id Entity ID
     * @return T* Pointer to the component
     */
    template <typename T>
    T *Assign(EntityID id);

    /**
     * @brief Get the component object from an entity
     *
     * @tparam T Component
     * @param id Entity ID
     * @return T* Pointer to the component
     */
    template <typename T>
    T *Get(EntityID id);

    /**
     * @brief Remove a component from an entity
     *
     * @tparam T Component
     * @param id Entity ID
     */
    template <typename T>
    void Remove(EntityID id);

    /**
     * @brief Destroy an entity
     *
     * @param id Entity ID
     */
    void DestroyEntity(EntityID id);

    /**
     * @brief Create a new SpriteSheet entity
     *
     * @param x
     * @param y
     * @param sheetLocal
     * @param physicsWorld
     */
    void CreateSpriteSheetTile(int x, int y, b2World *physicsWorld, Board *board);

    /**
     * @brief Add a Box2D collider to an entity
     *
     * @param entityID
     * @param isStatic
     * @param x
     * @param y
     * @param physicsWorld
     */
    void AddBox2DCollider(EntityID entityID, bool isStatic, bool isTrigger, float x, float y, float width, float height, b2World *physicsWorld);

    template <class T>
    int GetId();

    EntityID CreateEntityId(EntityIndex index, EntityVersion version);
    EntityIndex GetEntityIndex(EntityID id);
    EntityVersion GetEntityVersion(EntityID id);
    bool IsEntityValid(EntityID id);

    std::vector<EntityDesc> entities;
    std::vector<ComponentPool *> componentPools;
    std::vector<EntityIndex> freeEntities;

    // toggles
    bool m_showGrid = true;
    bool m_showColliders = true;

    EntityID m_selectedEntity = -1;
};
