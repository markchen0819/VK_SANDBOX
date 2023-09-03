#pragma once


namespace IHCEngine::Graphics
{
    class Camera;
}
namespace IHCEngine::Input
{
	class CameraController
	{
    public:

        CameraController(IHCEngine::Graphics::Camera& camera);
        IHCEngine::Graphics::Camera& camera;
        float movementSpeed = 0.02;
        float rotationSpeed = 0.1;

        void HandleInput(GLFWwindow* window);
	};
}
