#include "PhysicsSystem.hpp"

void PhysicsSystem::Update() const
{
    // Update transforms based on physics simulation
    UpdateTransforms();

    // Handle player movement
    HandlePlayerMovement();

    // Check for collisions
    CheckTriggers();
}

void PhysicsSystem::UpdateTransforms() const
{
    for (EntityID ent : SceneView<TransformComponent, Box2DColliderComponent>(*m_scene))
    {
        TransformComponent *transformLocal = m_scene->Get<TransformComponent>(ent);
        Box2DColliderComponent *boxColliderLocal = m_scene->Get<Box2DColliderComponent>(ent);

        // Update the position of the entity based on the physics simulation
        transformLocal->x = boxColliderLocal->body->GetPosition().x * m_board->m_tileSize;
        transformLocal->y = boxColliderLocal->body->GetPosition().y * m_board->m_tileSize;
    }

    for (EntityID ent : SceneView<GridSimulationComponent>(*m_scene))
    {
        GridSimulationComponent *grid = m_scene->Get<GridSimulationComponent>(ent);

        // Update the grid simulation based on pixel physics
        // If data is 1, move tile down, if tile is occupied move to left or right
        // Copy the current grid data to avoid modifying it while iterating
        std::vector<particle_t> &newGrid = *grid->gridData;

        for (int i = grid->rows - 1; i >= 0; i--)
        {
            for (int j = 0; j < grid->cols; j++)
            {
                const particle_t current = grid->gridData->at(i * grid->cols + j);
                if (current.id == ParticleType::SAND)
                {
                    UpdateSand(i, j, grid, newGrid);
                }
                else if (current.id == ParticleType::WATER)
                {
                    UpdateWater(i, j, grid, newGrid);
                }
            }
        }
    }
}

void PhysicsSystem::UpdateSand(int row, int col, GridSimulationComponent *grid, std::vector<particle_t> &newGrid) const
{
    int index = row * grid->cols + col;
    int cellBelow = (row + 1) * grid->cols + col;
    int cellLeft = (row + 1) * grid->cols + col - 1;
    int cellRight = (row + 1) * grid->cols + col + 1;
    const particle_t empty = {ParticleType::EMPTY, 0.0f, b2Vec2(0.0f, 0.0f), {0, 0, 0, 0}, false};
    const particle_t current = grid->gridData->at(index);

    // Randomly choose left or right
    int direction = rand() % 2 == 0 ? -1 : 1; // -1 for left, 1 for right

    // Move the pixel down if the cell below is empty and within bounds
    if (grid->IsEmpty(cellBelow))
    {
        newGrid[cellBelow] = current;
        newGrid[index] = empty;
    }
    // Move the pixel left if not at left edge and cell to the left is empty
    else if (direction == -1 && col > 0 && grid->IsEmpty(cellLeft) && grid->InBounds(row + 1, col - 1))
    {
        newGrid[cellLeft] = current;
        newGrid[index] = empty;
    }
    // Move the pixel right if not at right edge and cell to the right is empty
    else if (direction == 1 && col < grid->cols - 1 && grid->IsEmpty(cellRight) && grid->InBounds(row + 1, col + 1))
    {
        newGrid[cellRight] = current;
        newGrid[index] = empty;
    }
}

void PhysicsSystem::UpdateWater(int row, int col, GridSimulationComponent *grid, std::vector<particle_t> &newGrid) const
{
    int cellDirectLeft = row * grid->cols + col - 1;
    int cellDirectRight = row * grid->cols + col + 1;
    int index = row * grid->cols + col;
    int cellBelow = (row + 1) * grid->cols + col;
    int cellLeft = (row + 1) * grid->cols + col - 1;
    int cellRight = (row + 1) * grid->cols + col + 1;
    const particle_t empty = {ParticleType::EMPTY, 0.0f, b2Vec2(0.0f, 0.0f), {0, 0, 0, 0}, false};
    const particle_t current = grid->gridData->at(index);

    // Randomly choose left or right
    int direction = rand() % 2 == 0 ? -1 : 1; // -1 for left, 1 for right

    // Move the pixel down if the cell below is empty and within bounds
    if (grid->IsEmpty(cellBelow))
    {
        newGrid[cellBelow] = current;
        newGrid[index] = empty;
    }
    // Move the pixel left if not at left edge and cell to the left is empty
    else if (direction == -1 && col > 0 && grid->IsEmpty(cellLeft) && grid->InBounds(row + 1, col - 1))
    {
        newGrid[cellLeft] = current;
        newGrid[index] = empty;
    }
    // Move the pixel right if not at right edge and cell to the right is empty
    else if (direction == 1 && col < grid->cols - 1 && grid->IsEmpty(cellRight) && grid->InBounds(row + 1, col + 1))
    {
        newGrid[cellRight] = current;
        newGrid[index] = empty;
    }
    else if (direction == -1 && col > 0 && grid->IsEmpty(cellDirectLeft) && grid->InBounds(row, col - 1))
    {
        newGrid[cellDirectLeft] = current;
        newGrid[index] = empty;
    }
    else if (direction == 1 && col < grid->cols - 1 && grid->IsEmpty(cellDirectRight) && grid->InBounds(row, col + 1))
    {
        newGrid[cellDirectRight] = current;
        newGrid[index] = empty;
    }
}

void PhysicsSystem::HandlePlayerMovement() const
{
    for (EntityID ent : SceneView<TransformComponent, InputComponent, Box2DColliderComponent>(*m_scene))
    {
        TransformComponent *transformLocal = m_scene->Get<TransformComponent>(ent);
        InputComponent *inputLocal = m_scene->Get<InputComponent>(ent);
        Box2DColliderComponent *boxColliderLocal = m_scene->Get<Box2DColliderComponent>(ent);

        // Check if the player is on the ground
        if (inputLocal->spacePress)
        {
            // Apply a vertical impulse to simulate jumping
            boxColliderLocal->body->ApplyLinearImpulseToCenter(b2Vec2(0, inputLocal->jumpSpeed), true);
            inputLocal->spacePress = false;
        }

        // Update acceleration based on key presses
        if (inputLocal->leftPress)
        {
            boxColliderLocal->body->ApplyLinearImpulseToCenter(b2Vec2(-inputLocal->speed, 0), true);
        }
        else if (inputLocal->rightPress)
        {
            boxColliderLocal->body->ApplyLinearImpulseToCenter(b2Vec2(inputLocal->speed, 0), true);
        }

        // Reset player position if out of bounds
        if (boxColliderLocal->body->GetPosition().x < 0.0f)
        {
            boxColliderLocal->body->SetTransform(b2Vec2(0.0f, boxColliderLocal->body->GetPosition().y), 0.0f);
        }

        if (boxColliderLocal->body->GetPosition().x >= m_board->m_boardWidth - 1)
        {
            boxColliderLocal->body->SetTransform(b2Vec2(m_board->m_boardWidth - 1, boxColliderLocal->body->GetPosition().y), 0.0f);
        }

        if (boxColliderLocal->body->GetPosition().y < 0.0f)
        {
            boxColliderLocal->body->SetTransform(b2Vec2(boxColliderLocal->body->GetPosition().x, 0.0f), 0.0f);
        }

        if (boxColliderLocal->body->GetPosition().y > m_board->m_boardHeight - 1)
        {
            boxColliderLocal->body->SetTransform(b2Vec2(boxColliderLocal->body->GetPosition().x, m_board->m_boardHeight - 1), 0.0f);
        }

        // Update the position of the entity based on the physics simulation
        transformLocal->x = boxColliderLocal->body->GetPosition().x * m_board->m_tileSize;
        transformLocal->y = boxColliderLocal->body->GetPosition().y * m_board->m_tileSize;
    }
}

void PhysicsSystem::CheckTriggers() const
{
    for (EntityID ent : SceneView<TransformComponent, Box2DColliderComponent>(*m_scene))
    {
        Box2DColliderComponent *boxColliderLocal = m_scene->Get<Box2DColliderComponent>(ent);

        if (boxColliderLocal->isTrigger)
        {
            // Check for collisions with other objects
            for (b2ContactEdge *edge = boxColliderLocal->body->GetContactList(); edge; edge = edge->next)
            {
                b2Contact *contact = edge->contact;
                b2Fixture *fixtureA = contact->GetFixtureA();
                b2Fixture *fixtureB = contact->GetFixtureB();

                // Iterate over all fixtures attached to the body and check for collision
                for (b2Fixture *fixture = boxColliderLocal->body->GetFixtureList(); fixture; fixture = fixture->GetNext())
                {
                    if (fixture == fixtureA || fixture == fixtureB)
                    {
                        boxColliderLocal->OnCollisionEnter();
                        break;
                    }
                }
            }
        }
    }
}
