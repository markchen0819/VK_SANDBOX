#pragma once
#include <GLFW/glfw3.h>
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"
#include "../../../../Engine/src/Math/Path/SpaceCurve.h"

namespace IHCEngine::Component
{
	class LineRendererComponent;
}

namespace IHCEngine::Graphics
{
	class Camera;
}

namespace SampleApplication
{
    class MotionAlongPathViewer : public IHCEngine::Component::CustomBehavior
    {
    public:
        MotionAlongPathViewer();
        void Awake() override;
        void Start() override;
        void Update() override;
        void FixedUpdate() override;
        void OnEnable() override;
        void OnDisable() override;


    private:

        // GLFWwindow* window = nullptr;
        //IHCEngine::Graphics::Camera* camera = nullptr;

        IHCEngine::Math::SpaceCurve spaceCurve;
        IHCEngine::Component::LineRendererComponent* lineRenderer;
    };
}
