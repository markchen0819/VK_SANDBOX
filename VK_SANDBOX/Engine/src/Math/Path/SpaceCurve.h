#pragma once


namespace IHCEngine::Math
{
	class SubCurve;

	class SpaceCurve
	{
	public:

		void SetControlPoints(const std::vector<glm::vec3> controlPoints);

	private:

		std::vector<glm::vec3> controlPoints;
		std::vector<std::unique_ptr<SubCurve>> segments;
		void buildSegments();

	};

}