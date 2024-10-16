#include "Scene.hpp"

int s_componentCounter = 0;

template <class T>
int Scene::GetId()
{
    static int s_componentId = s_componentCounter++;
    return s_componentId;
}

EntityID Scene::CreateEntityId(EntityIndex index, EntityVersion version)
{
    // Shift the index up 32, and put the version in the bottom
    return ((EntityID)index << 32) | ((EntityID)version);
}

EntityIndex Scene::GetEntityIndex(EntityID id)
{
    // Shift down 32 so we lose the version and get our index
    return id >> 32;
}

EntityVersion Scene::GetEntityVersion(EntityID id)
{
    // Cast to a 32 bit int to get our version number (loosing the top 32 bits)
    return (EntityVersion)id;
}

bool Scene::IsEntityValid(EntityID id)
{
    // Check if the index is our invalid index
    return (id >> 32) != EntityIndex(-1);
}

EntityID Scene::NewEntity()
{
    if (!freeEntities.empty())
    {
        EntityIndex newIndex = freeEntities.back();
        freeEntities.pop_back();
        EntityID newID = CreateEntityId(newIndex, GetEntityVersion(entities.at(newIndex).id));
        entities.at(newIndex).id = newID;
        return entities.at(newIndex).id;
    }
    entities.push_back({CreateEntityId(EntityIndex(entities.size()), 0), ComponentMask()});
    return entities.back().id;
}

template <typename T>
T *Scene::Assign(EntityID id)
{
    if (entities[GetEntityIndex(id)].id != id)
        return nullptr;
    size_t componentId = GetId<T>();

    // Resize the component pool if it doesn't exist yet
    if (componentPools.size() <= componentId)
    {
        componentPools.resize(componentId + 1, nullptr);
    }
    // If the component pool is null at the index, initialize a new one
    if (componentPools.at(componentId) == nullptr)
    {
        componentPools.at(componentId) = new ComponentPool(sizeof(T));
    }

    // Looks up the component in the pool, and initializes it with placement new
    T *pComponent = new (componentPools.at(componentId)->get(GetEntityIndex(id))) T();

    // Set the bit for this component to true and return the created component
    entities.at(GetEntityIndex(id)).mask.set(componentId);
    return pComponent;
}
template TransformComponent *Scene::Assign<TransformComponent>(EntityID id);
template SpriteComponent *Scene::Assign<SpriteComponent>(EntityID id);
template InputComponent *Scene::Assign<InputComponent>(EntityID id);
template SpriteSheetComponent *Scene::Assign<SpriteSheetComponent>(EntityID id);
template Box2DColliderComponent *Scene::Assign<Box2DColliderComponent>(EntityID id);
template GridSimulationComponent *Scene::Assign<GridSimulationComponent>(EntityID id);

template <typename T>
T *Scene::Get(EntityID id)
{
    if (entities[GetEntityIndex(id)].id != id)
        return nullptr;

    size_t componentId = GetId<T>();

    if (!entities.at(GetEntityIndex(id)).mask.test(componentId))
        return nullptr;

    T *pComponent = static_cast<T *>(componentPools.at(componentId)->get(GetEntityIndex(id)));

    // Log the entity id, entity index, and the component id
    // std::cout << "Entity ID: " << id << " Entity Index: " << GetEntityIndex(id) << " Component ID: " << componentId << std::endl;
    return pComponent;
}

template TransformComponent *Scene::Get<TransformComponent>(EntityID id);
template SpriteComponent *Scene::Get<SpriteComponent>(EntityID id);
template InputComponent *Scene::Get<InputComponent>(EntityID id);
template SpriteSheetComponent *Scene::Get<SpriteSheetComponent>(EntityID id);
template Box2DColliderComponent *Scene::Get<Box2DColliderComponent>(EntityID id);
template GridSimulationComponent *Scene::Get<GridSimulationComponent>(EntityID id);

template <typename T>
void Scene::Remove(EntityID id)
{
    // ensures you're not accessing an entity that has been deleted
    if (entities.at(GetEntityIndex(id)).id != GetEntityIndex(id))
        return;

    int componentId = GetId<T>();
    entities.at(GetEntityIndex(id)).mask.reset(componentId);
}

template void Scene::Remove<TransformComponent>(EntityID id);
template void Scene::Remove<SpriteComponent>(EntityID id);
template void Scene::Remove<InputComponent>(EntityID id);
template void Scene::Remove<SpriteSheetComponent>(EntityID id);
template void Scene::Remove<Box2DColliderComponent>(EntityID id);
template void Scene::Remove<GridSimulationComponent>(EntityID id);

void Scene::DestroyEntity(EntityID id)
{
    EntityID newID = CreateEntityId(EntityIndex(-1), GetEntityVersion(id) + 1);
    entities.at(GetEntityIndex(id)).id = newID;
    entities.at(GetEntityIndex(id)).mask.reset();
    freeEntities.push_back(GetEntityIndex(id));
}

void Scene::CreateSpriteSheetTile(int x, int y, b2World *physicsWorld, Board *board)
{
    EntityID entity = NewEntity();
    TransformComponent *trans = Assign<TransformComponent>(entity);
    trans->x = x * board->m_boardWidth;
    trans->y = y * board->m_boardHeight;
    trans->hasParent = true;

    AddBox2DCollider(entity, true, false, x, y, 1, 1, physicsWorld);
}

void Scene::AddBox2DCollider(EntityID entityID, bool isStatic, bool isTrigger, float x, float y, float width, float height, b2World *physicsWorld)
{
    Box2DColliderComponent *box2dCollider = Assign<Box2DColliderComponent>(entityID);

    box2dCollider->bodyDef.position.Set(x, y);

    if (isStatic)
    {
        box2dCollider->bodyDef.type = b2_staticBody; // Static body does not move
    }
    else
    {
        box2dCollider->bodyDef.type = b2_dynamicBody; // Dynamic body can move and collide with others
    }

    box2dCollider->body = physicsWorld->CreateBody(&box2dCollider->bodyDef);

    box2dCollider->shape.SetAsBox(width / 2, height / 2); // Set box shape (half-width, half-height)

    box2dCollider->fixtureDef.shape = &box2dCollider->shape;
    box2dCollider->fixtureDef.density = 1.0f;  // Set density for dynamic behavior
    box2dCollider->fixtureDef.friction = 0.2f; // Set friction

    b2BodyUserData data = box2dCollider->body->GetUserData();
    data.pointer = (uintptr_t)entityID;

    box2dCollider->body->CreateFixture(&box2dCollider->fixtureDef);

    if (isTrigger)
    {
        box2dCollider->isTrigger = true;
        box2dCollider->fixtureDef.isSensor = true;
        box2dCollider->onCollisionEnter = []()
        {
            SDL_Log("Trigger entered");
        };
    }
}