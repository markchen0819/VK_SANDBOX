#pragma once
#include "SubCurve.h"

namespace IHCEngine::Math
{
	class SpaceCurve
	{
	public:

		void SetControlPoints(const std::vector<glm::vec3> controlPoints);

		//test
		std::vector<glm::vec3> GetPoints();

	private:

		std::vector<glm::vec3> controlPoints;
		std::vector<std::unique_ptr<SubCurve>> subCurves;
		void buildSegments();

	};

}