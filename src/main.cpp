#include "Application.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

namespace py = pybind11;

int main(int, char **)
{
    Board *board = new Board(64, 20, 12);
    Application *app = new Application(board, true);

    app->Loop(120.f);

    return 0;
}

PYBIND11_MODULE(blockbyte, m)
{
    // Export the Board class
    py::class_<Board>(m, "Board")
        .def(py::init<int, int, int>());

    py::class_<Application>(m, "Application")
        .def(py::init<Board *, bool>())
        .def("Loop", &Application::Loop)
        .def("ImportSpritesheetLevel", &Application::ImportSpritesheetLevel)
        .def("AddBox2D", &Application::AddBox2D)
        .def("AddSprite", &Application::AddSprite)
        .def("GetScene", &Application::GetScene, py::return_value_policy::reference)
        .def_readwrite("m_isRunning", &Application::m_isRunning);

    py::class_<EntityID>(m, "EntityID")
        .def(py::init<EntityID>());

    py::class_<Scene>(m, "Scene")
        .def(py::init<>())
        .def("NewEntity", &Scene::NewEntity)
        .def("AddBox2DCollider", &Scene::AddBox2DCollider)

        // Assign Components
        .def("AssignTransformComponent", &Scene::Assign<TransformComponent>)
        .def("GetTransformComponent", &Scene::Get<TransformComponent>, py::return_value_policy::reference)

        .def("AssignSpriteComponent", &Scene::Assign<SpriteComponent>)
        .def("GetSpriteComponent", &Scene::Get<SpriteComponent>, py::return_value_policy::reference)

        .def("AssignInputComponent", &Scene::Assign<InputComponent>)
        .def("GetInputComponent", &Scene::Get<InputComponent>, py::return_value_policy::reference)

        .def("AssignSpriteSheetComponent", &Scene::Assign<SpriteSheetComponent>)
        .def("GetSpriteSheetComponent", &Scene::Get<SpriteSheetComponent>, py::return_value_policy::reference)

        .def("AssignBox2DColliderComponent", &Scene::Assign<Box2DColliderComponent>)
        .def("GetBox2DColliderComponent", &Scene::Get<Box2DColliderComponent>, py::return_value_policy::reference)

        .def("AssignGridSimulationComponent", &Scene::Assign<GridSimulationComponent>)
        .def("GetGridSimulationComponent", &Scene::Get<GridSimulationComponent>, py::return_value_policy::reference);

    // Define component classes
    py::class_<TransformComponent>(m, "TransformComponent")
        .def(py::init<>())
        .def_readwrite("x", &TransformComponent::x)
        .def_readwrite("y", &TransformComponent::y);

    py::class_<SpriteComponent>(m, "SpriteComponent")
        .def(py::init<>())
        .def_readwrite("filePath", &SpriteComponent::filePath)
        .def_readwrite("width", &SpriteComponent::width)
        .def_readwrite("height", &SpriteComponent::height);

    py::class_<Box2DColliderComponent>(m, "Box2DColliderComponent")
        .def(py::init<>())
        .def_readwrite("onCollisionEnter", &Box2DColliderComponent::onCollisionEnter);

    py::class_<InputComponent>(m, "InputComponent")
        .def(py::init<>())
        .def_readwrite("speed", &InputComponent::speed)
        .def_readwrite("jumpSpeed", &InputComponent::jumpSpeed);

    py::class_<SpriteSheetComponent>(m, "SpriteSheetComponent")
        .def(py::init<>());

    py::class_<GridSimulationComponent>(m, "GridSimulationComponent")
        .def(py::init<>());
}