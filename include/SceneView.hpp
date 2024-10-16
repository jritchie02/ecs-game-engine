#pragma once

#include <iostream>
#include <vector>
#include "Scene.hpp"

/**
 * @brief Iterator for Entites of the scene
 *
 * @tparam ComponentTypes
 */
template <typename... ComponentTypes>
struct SceneView
{
    /**
     * @brief Construct a new Scene View object
     *
     * @param scene Scene
     */
    SceneView(Scene &scene) : pScene(&scene)
    {
        if (sizeof...(ComponentTypes) == 0)
        {
            all = true;
        }
        else
        {
            // Unpack the template parameters into an initializer list
            int componentIds[] = {0, scene.GetId<ComponentTypes>()...};
            for (size_t i = 1; i < (sizeof...(ComponentTypes) + 1); i++)
                componentMask.set(componentIds[i]);
        }
    }

    /**
     * @brief Iterator for the SceneView
     *
     */
    struct Iterator
    {
        Iterator(Scene *pScene, EntityIndex index, ComponentMask mask, bool all)
            : index(index), pScene(pScene), mask(mask), all(all) {}

        EntityID operator*() const
        {
            return pScene->entities[index].id;
        }

        bool operator==(const Iterator &other) const
        {
            return index == other.index || index == pScene->entities.size();
        }

        bool operator!=(const Iterator &other) const
        {
            return index != other.index && index != pScene->entities.size();
        }

        bool ValidIndex()
        {
            return
                // It's a valid entity ID
                pScene->IsEntityValid(pScene->entities[index].id) &&
                // It has the correct component mask
                (all || mask == (mask & pScene->entities[index].mask));
        }

        Iterator &operator++()
        {
            do
            {
                index++;
            } while (index < pScene->entities.size() && !ValidIndex());
            return *this;
        }

        EntityIndex index;
        Scene *pScene;
        ComponentMask mask;
        bool all{false};
    };

    const Iterator begin() const
    {
        size_t firstIndex = 0;
        while (firstIndex < pScene->entities.size() &&
               (componentMask != (componentMask & pScene->entities[firstIndex].mask) || !pScene->IsEntityValid(pScene->entities[firstIndex].id)))
        {
            firstIndex++;
        }
        return Iterator(pScene, firstIndex, componentMask, all);
    }

    const Iterator end() const
    {
        return Iterator(pScene, EntityIndex(pScene->entities.size()), componentMask, all);
    }

    Scene *pScene{nullptr};
    ComponentMask componentMask;
    bool all{false};
};
