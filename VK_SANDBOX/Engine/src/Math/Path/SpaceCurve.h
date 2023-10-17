#pragma once
#include "SubCurve.h"

namespace IHCEngine::Math
{
	// SpaceCurve is the entire path
	// SubCurves are Bezier polynomials contributing to part of the path

	class SpaceCurve
	{
	public:
		void SetControlPoints(const std::vector<glm::vec3> controlPoints);
		std::vector<glm::vec3> GetPointsForRendering();
		glm::vec3 GetPositionOnCurve(float u);

	private:

		std::vector<glm::vec3> controlPoints;
		std::vector<std::unique_ptr<SubCurve>> subCurves;
		void buildSubCurves();


		ArcLengthTable globalArcLengthTable;
		void buildGlobalArcLengthTable();

	};

}