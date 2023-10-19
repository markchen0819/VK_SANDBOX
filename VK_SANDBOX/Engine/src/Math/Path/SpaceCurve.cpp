#include "../../pch.h"
#include "SpaceCurve.h"

namespace IHCEngine::Math
{

	std::vector<glm::vec3> SpaceCurve::GetPointsForRendering()
	{
		std::vector<glm::vec3> points;
		for (auto& subcurve : subCurves)
		{
			auto pts = subcurve->GetPointsForRendering();
			for (auto& pt : pts)
			{
				points.push_back(pt);
			}
		}
		return points;
	}

	glm::vec3 SpaceCurve::GetPositionOnCurve(float traveledDistance)
	{
		// assume traveledDistance is 0 to 1
		// i = G^-1(s) * k
		// u = G^-1(s) * k - i

		float normalizedU = globalArcLengthTable.GetUParameterFromNormalizedTable(traveledDistance);
		int targetSubCurveIndex = std::ceil(normalizedU * globalArcLengthTable.totalU)-1;
		float localU = normalizedU * globalArcLengthTable.totalU - targetSubCurveIndex;
		//std::cout << localU << std::endl;
		glm::vec3 position = subCurves[targetSubCurveIndex]->GetPointOnCurve(localU);

		return position;
	}

	void SpaceCurve::SetControlPoints(const std::vector<glm::vec3> controlPoints)
	{
		this->controlPoints.clear();
		subCurves.clear();
		this->controlPoints = controlPoints;
		buildSubCurves();
		buildGlobalArcLengthTable();
	}

	void SpaceCurve::buildSubCurves()
	{
		if(controlPoints.size() < 4)
		{
			assert("Not enough control points!");
		}
		// p0 - p1
		glm::vec3 p0 = controlPoints[0];
		glm::vec3 a0 = controlPoints[0];
		glm::vec3 b1 = controlPoints[1] - (controlPoints[2] - controlPoints[0]) / 2.0f;
		glm::vec3 p1 = controlPoints[1];
		subCurves.push_back(std::make_unique<SubCurve>(p0, a0, b1, p1));
		// p1 - p2 - ... - pn-1
		for(size_t i = 1; i < controlPoints.size() - 2; ++i) // start from p1 not p0
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
			glm::vec3 bi_plus1 = pi_plus1 - (pi_plus2 - pi) / 2.0f;

			subCurves.push_back(std::make_unique<SubCurve>(pi, ai, bi_plus1, pi_plus1));
		}
		// pn-1 - pn
		glm::vec3 pn = controlPoints[controlPoints.size() - 1];
		glm::vec3 bn = controlPoints[controlPoints.size() - 1];
		glm::vec3 pn_minus1 = controlPoints[controlPoints.size() - 2];
		glm::vec3 an_minus1 = pn_minus1 + (pn - controlPoints[controlPoints.size() - 3])/2.0f;
		subCurves.push_back(std::make_unique<SubCurve>(pn_minus1, an_minus1, bn, pn));
	}

	void SpaceCurve::buildGlobalArcLengthTable()
	{
		globalArcLengthTable.Clear();
		float subCurveStart = 0.0f;
		float previousTableEndingLength = 0.0f;
		for (auto& subcurve : subCurves)
		{
			auto subCurveTable = subcurve->GetArcLengthTable();
			for (int i=0; i<subCurveTable.table.size(); ++i)
			{
				auto& entry = subCurveTable.table[i];

				if(subCurveStart!=0.0 && i==0)
				{
					// skip duplicates
					continue;
				}
				ArcLengthEntry e;
				e.u = entry.u + subCurveStart;
				e.arcLength = entry.arcLength + previousTableEndingLength;
				globalArcLengthTable.table.push_back(e);
			}
			subCurveStart += 1.0f;
			previousTableEndingLength = globalArcLengthTable.table.back().arcLength;
		}
		globalArcLengthTable.Normalize();
		//globalArcLengthTable.PrintTable();
	}
}
