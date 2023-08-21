#pragma once

namespace IHCEngine::Graphics
{
	class Camera
	{
	public:
		void SetOrthographicProjection
		( 
			float left, float right, 
			float top, float bottom,
			float near, float far
		);
		void SetPerspectiveProjection
		(
			float fovy, float aspect, float near, float far
		);

		void SetViewDirection
		(
			glm::vec3 position, 
			glm::vec3 direction, 
			glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f }
		);
		void SetViewTarget
		(
			glm::vec3 position, glm::vec3 target, 
			glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f }
		);
		void SetViewYXZ(glm::vec3 position, glm::vec3 rotation); // EulerAngles

		const glm::mat4& GetProjection() const { return projectionMatrix; }
		const glm::mat4& GetView() const { return viewMatrix; }
		const glm::mat4& GetInverseView() const { return inverseViewMatrix; }
		const glm::vec3 GetPosition() const { return glm::vec3(inverseViewMatrix[3]); }

	private:
		glm::mat4 projectionMatrix{ 1.f }; // camera to screen (NDC)

		glm::mat4 viewMatrix{1.f};  // world to camera
		// transforms objects in the world space to the camera's view space.
		// represents the camera's position and orientation in the world.
		// moving and rotating everything in the world so that the camera is at the origin (0, 0, 0) and looking down the negative z-axi
		
		glm::mat4 inverseViewMatrix{1.f};
		// transforms from camera space back to world space.
		// common scenario (deferred rendering), wants the pixel pos in world


		// Flow:
		// Local Space -> (modelMatrix) 
		// ->  World Space -> (viewMatrix)
		// -> Camera Space -> (projectionMatrix) 
		// -> Clip Space -> (Perspective Division) -> 
		// -> Normalized Device Coordinates -> (Viewport Transformation)
		// -> Screen Space 
	};
}

