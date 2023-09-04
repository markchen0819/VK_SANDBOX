#pragma once
#include <GLFW/glfw3.h>
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"
namespace IHCEngine::Graphics
{
    class Camera;
}
namespace SampleApplication
{
	class CameraController : public IHCEngine::Component::CustomBehavior
	{
    public:
        CameraController();
        void Awake() override;
        void Start() override;
        void Update() override;
        void FixedUpdate() override;
        void OnEnable() override;
        void OnDisable() override; 

    private:

        GLFWwindow* window = nullptr;
        IHCEngine::Graphics::Camera* camera = nullptr;

        float movementSpeed = 1;
        float rotationSpeed = 1; // degree

        bool isRotating = false;
        double mouseX, mouseY;
        double lastX = 0.0, lastY = 0.0;
        void HandleInput();
	};
}
