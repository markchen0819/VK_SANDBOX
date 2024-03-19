#pragma once
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

#include "Frustum.h"
#include "AABB_BoundingVolume.h"

namespace SampleApplication
{

    class CPUFrustumCulling : public IHCEngine::Component::CustomBehavior
    {
    public:
        CPUFrustumCulling();

        void Start() override;
        void Update() override;
        void OnEnable() override;
        void OnDisable() override;

        void AddAABBWithGameObject(AABB_BoundingVolume aabb, IHCEngine::Core::GameObject* gobj);

    private:

        Frustum frustum;
        const glm::vec3 cameraPrevPos =glm::vec3(0,0,0);

        bool dropFrustum{ false }; // debug
        std::vector<AABB_BoundingVolume> aabbs;
        std::vector<IHCEngine::Core::GameObject*> gobjs;
        void UpdateFrustum();
        void Cull();


    };
}
