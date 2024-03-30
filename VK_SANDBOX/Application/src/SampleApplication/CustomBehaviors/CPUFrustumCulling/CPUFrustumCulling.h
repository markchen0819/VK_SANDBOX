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

        void DropFrustum(bool b) { dropFrustum = b; }
        bool GetIsFrustumDropped() const { return dropFrustum; }

    private:
        Frustum frustum;
        glm::vec3 cameraPrevPos = glm::vec3(0,0,0);
        glm::vec3 cameraDroppedPos = glm::vec3(0, 0, 0);
        bool dropFrustum{ false }; // debug
        std::vector<AABB_BoundingVolume> aabbs;
        std::vector<IHCEngine::Core::GameObject*> gobjs;
        void UpdateFrustum();
        void Cull();

        // LOD
        float LODDistance = 60.0f;
        void UpdateLOD();
    };
}
