#include "../../pch.h"
#include "SpaceCurve.h"

namespace IHCEngine::Math
{
	void SpaceCurve::SetControlPoints(const std::vector<glm::vec3> controlPoints)
	{
		this->controlPoints = controlPoints;
		buildSegments();
	}

	std::vector<glm::vec3> SpaceCurve::GetPoints()
	{
		std::vector<glm::vec3> points;

		for(auto& subcurve : subCurves)
		{
			// test to see contents
			auto& list = subcurve->GetSortedSegmentList();
			auto& table = subcurve->GetArcLengthTable();

			auto pts = subcurve->GetControlPoints();
			for (auto& pt : pts)
			{
				points.push_back(pt);
			}
		}
		return points;

	}

	void SpaceCurve::buildSegments()
	{
		if(controlPoints.size() < 4)
		{
			assert("Not enough control points!");
		}
		// p0 - p1
		glm::vec3 p0 = controlPoints[0];
		glm::vec3 p1 = controlPoints[1];
		glm::vec3 a0 = p0 + (p1 - p0) / 3.0f;
		glm::vec3 b0 = p0 + 2.0f * (p1 - p0) / 3.0f;
		subCurves.push_back(std::make_unique<SubCurve>(p0, a0, b0, p1));
		// p1 - p2 - ... - pn-1
		for(size_t i = 1; i < controlPoints.size() - 3; ++i) // start from p1 not p0
		{
			// p = pi+(pi-pi-1) 
			// ai = (p+pi+1)/2 = pi+(pi+1-pi-1)/2
			// bi = pi+(pi-ai) = pi-(pi+1-pi-1)/2
			// use <pi, ai, bi+1, pi+1>

			glm::vec3 pi = controlPoints[i];
			glm::vec3 pi_plus1 = controlPoints[i + 1];
			glm::vec3 pi_minus1 = controlPoints[i - 1];
			glm::vec3 pi_plus2 = controlPoints[i + 2];
			glm::vec3 ai = pi + (pi_plus1 - pi_minus1) / 2.0f;
			glm::vec3 bi_plus1 = pi_plus1 + (pi_plus2 - pi) / 2.0f;

			subCurves.push_back(std::make_unique<SubCurve>(pi, ai, bi_plus1, pi_plus1));
		}
		// pn-1 - pn
		glm::vec3 p6 = controlPoints[controlPoints.size() - 2];
		glm::vec3 p7 = controlPoints[controlPoints.size() - 1];
		glm::vec3 b6 = p7 - (p7 - p6) / 3.0f;
		glm::vec3 a6 = p7 - 2.0f * (p7 - p6) / 3.0f;
		subCurves.push_back(std::make_unique<SubCurve>(p6, a6, b6, p7));
	} 

}
