#pragma once
#include "Component.h"
#include "../../../Graphics/Particle/Particle.h"
#include "../../../Graphics/VKWraps/VKHelpers.h"
#include "../../../Graphics/Particle/ComputeParticleUniformBufferObject.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;
}

namespace IHCEngine::Component
{
	class ComputeParticleComponent : public Component
	{
	public:
		ComputeParticleComponent();
		~ComputeParticleComponent() = default;

		const int GetMaxParticleCount() { return maxParticleCount; }
		const int GetParticleCount() { return particleCount; }

		void Compute(Graphics::FrameInfo& frameInfo);
		void Draw(Graphics::FrameInfo& frameInfo);

		//// Operations
		// Advection
		void SetEnableAdvection(const bool b) { enableAdvection = b; }
		bool GetEnableAdvection() const { return enableAdvection; }
		void SetFlowDirection(const glm::vec4& vec) { flowDirection = glm::normalize(vec); }
		const glm::vec4& GetFlowDirection() const { return flowDirection; }
		void SetFlowCenter(const glm::vec4& vec) { flowCenter = vec; }
		const glm::vec4& GetFlowCenter() const { return flowCenter; }
		void SetFlowMaxVelocity(float f) { flowMaxVelocity = f; }
		float GetFlowMaxVelocity() const { return flowMaxVelocity; }
		void SetFlowWidth(float f) { flowWidth = f; }
		float GetFlowWidth() const { return flowWidth; }
		// Vortex
		void SetEnableVortex(const bool b) { enableVortex = b; }
		bool GetEnableVortex() const { return enableVortex; }
		void SetRotationRate(const float f) { kappa = f; }
		float GetRotationRate() const { return kappa; }
		void SetTightness(const float f) { tau = f; }
		float GetTightness() const { return tau; }
		void SetVortexCenter(const glm::vec4& vec) { vortexCenter = vec; }
		const glm::vec4& GetVortexCenter() const { return vortexCenter; }
		void SetVortexAxis(const glm::vec4& vec) { vortexAxis = glm::normalize(vec); }
		const glm::vec4& GetVortexAxis() const { return vortexAxis; }
		// Gravity
		void SetEnableGravity(const bool b) { enableGravity = b; }
		bool GetEnableGravity() const { return enableGravity; }
		void SetGravity(const glm::vec4& vec) { gravity = vec; }
		const glm::vec4& GetGravity() const { return gravity; }
		// Bounce
		void SetEnableBounce(const bool b) { enableBounce = b; }
		bool GetEnableBounce() const { return enableBounce; }
		void SetRestitution(const float f) { restitution = f; }
		float GetRestitution() const { return restitution; }

		////
		// Vulkan
		void SetDescriptorSets(std::vector<VkDescriptorSet> set) { computeDescriptorSets = set; }
		std::vector<VkDescriptorSet>& GetDescriptorSets() { return computeDescriptorSets; }
		void SetUniformBuffers(std::vector<Graphics::IHCBuffer*> buffers) { computeParticleUniformBuffers = buffers; }
		std::vector<Graphics::IHCBuffer*>& GetUnformBuffers() { return computeParticleUniformBuffers; }
		std::vector<Graphics::IHCBuffer*> GetSSBO()
		{
			std::vector<Graphics::IHCBuffer*> rawPointerVector;
			rawPointerVector.reserve(shaderStorageBuffers.size());
			for (const auto& uniquePtr : shaderStorageBuffers) 
			{
				rawPointerVector.push_back(uniquePtr.get());
			}
			return rawPointerVector;
		}

	private:
		std::vector<Graphics::Particle> particles;

		int maxParticleCount = 65536 * 10; // 256;
		int particleCount = 65536 * 10; //256;
		float lastFrameTime = 0.0f;

		bool enableAdvection = false;
		glm::vec4 flowDirection = glm::vec4(0, 1, 0, 0);
		glm::vec4 flowCenter = glm::vec4(0, 1, 0, 0);
		float flowMaxVelocity = 3.0f;
		float flowWidth = 10.0f;
		
		bool enableVortex = false;
		float kappa = 2.0f;  // rotation rate 
		float tau = 0.5f; //  tightness
		glm::vec4 vortexCenter = glm::vec4(0, 0, 0, 0);
		glm::vec4 vortexAxis = glm::vec4(0, 1, 0, 0);

		bool enableGravity = false;
		glm::vec4 gravity = glm::vec4(0, -9.8, 0, 0);

		bool enableBounce = false;
		float groundHeight = -4.8;
		float restitution = 0.7;


		void initParticles();

		// Vulkan
		Graphics::ComputeParticleUniformBufferObject ubo{};
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> shaderStorageBuffers; // created in component
		std::vector<Graphics::IHCBuffer*> computeParticleUniformBuffers; // retrieved from descriptor wrap
		std::vector<VkDescriptorSet> computeDescriptorSets; // retrieved from descriptor wrap
	
		void createVulkanResources();
		void createShaderStorageBuffers();
		void destroyVulkanResources();

		void Attach() override;
		void Remove() override;
	};
}
