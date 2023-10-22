#include "../../pch.h"
#include "SubCurve.h"

#include "PathStructs.h"

namespace IHCEngine::Math
{
	SubCurve::SubCurve(glm::vec3 p0, glm::vec3 a0, glm::vec3 b1, glm::vec3 p1)
		: p0(p0), a0(a0), b1(b1), p1(p1)
	{
		buildArcLengthTable();
		generatePointsForRendering();
	}

	glm::vec3 SubCurve::GetPointOnCurve(float u)
	{
		return computeBezier(u);
	}


	void SubCurve::buildArcLengthTable()
	{
		arcLengthTable.Clear();
		sortedSegmentList.clear();

		arcLengthTable.table.push_back({ 0, 0.0 });
        sortedSegmentList.push_back({ 0.0f, 1.0f, approximateLengthOfSegment(0.0f, 1.0f) });

        while (!sortedSegmentList.empty())
		{
			Segment current = sortedSegmentList.back();
			sortedSegmentList.pop_back();

            float uMid = (current.uStart + current.uEnd) / 2.0f;
            float A = approximateLengthOfSegment(current.uStart, uMid);
            float B = approximateLengthOfSegment(uMid, current.uEnd);

            if (std::abs(A + B - current.length) > epsilon || isSCurve(current.uStart, uMid, current.uEnd))
			{
                // Split the segment
				sortedSegmentList.push_back({ current.uStart, uMid, A });
				sortedSegmentList.push_back({ uMid, current.uEnd, B });
            }
            else 
			{
                // Record the arc length for both subdivisions (NOT ACCUMULATED ARC LENGTH)
				// Avoid adding duplicate u
				if (arcLengthTable.table.empty() || arcLengthTable.table.back().u != uMid)
				{
					arcLengthTable.table.push_back({ uMid, A });
				}
				// Avoid adding duplicate u
				if (arcLengthTable.table.empty() || arcLengthTable.table.back().u != current.uEnd)
				{
					arcLengthTable.table.push_back({ current.uEnd, B });
				}
            }
        }
		// Sort u & accumulate arclength
		std::sort(arcLengthTable.table.begin(), arcLengthTable.table.end(), 
			[](const ArcLengthEntry& a, const ArcLengthEntry& b) 
		{
			return a.u < b.u;
		});
		float accumulatedLength = 0.0f;
		for (auto& entry : arcLengthTable.table)
		{
			accumulatedLength += entry.arcLength; // individual segment length
			entry.arcLength = accumulatedLength; // change to accumulated length up to this u
		}
		// Sort segment list
		std::sort(sortedSegmentList.begin(), sortedSegmentList.end(),
			[](const Segment& a, const Segment& b)
		{
			return a.uStart < b.uStart;
		});

		arcLengthTable.Normalize();
		//arcLengthTable.PrintTable();
	}
	float SubCurve::approximateLengthOfSegment(float uStart, float uEnd)
	{
		glm::vec3 const startPoint = computeBezier(uStart);
		glm::vec3 const endPoint = computeBezier(uEnd);
		return glm::length(endPoint - startPoint); 
	}
	glm::vec3 SubCurve::computeBezier(float u)
	{
		// Geometric form
		// P(u) = (1-u)^3 * P0 + 3(1-u)^2 * u * A0 + 3(1-u) * u^2 * B1 + u^3 * P1
		float u2 = u * u;
		float u3 = u2 * u;
		float one_minus_u = 1.0f - u;
		float one_minus_u2 = one_minus_u * one_minus_u;
		float one_minus_u3 = one_minus_u2 * one_minus_u;

		return one_minus_u3 * p0
		       + 3 * one_minus_u2 * u * a0
			   + 3 * one_minus_u * u2 * b1
			   + u3 * p1;
	}
	glm::vec3 SubCurve::computeBezierDerivative(float u)
	{
		// First derivative for cubic Bezier:
		// P'(u) = 3(1-u)^2 (A0 - P0) + 6(1-u)u (B1 - A0) + 3u^2 (P1 - B1)
		float one_minus_u = 1.0f - u;
		return 3 * one_minus_u * one_minus_u * (a0 - p0)
			   + 6 * one_minus_u * u * (b1 - a0)
			   + 3 * u * u * (p1 - b1);
	}
	bool SubCurve::isSCurve(float uStart, float uMid, float uEnd)
	{
		glm::vec3 tangentStart = glm::normalize(computeBezierDerivative(uStart));
		glm::vec3 tangentMid = glm::normalize(computeBezierDerivative(uMid));
		glm::vec3 tangentEnd = glm::normalize(computeBezierDerivative(uEnd));

		float dotProductStartMid = glm::dot(tangentStart, tangentMid);
		float dotProductMidEnd = glm::dot(tangentMid, tangentEnd);

		// Check for near parallelism (dot product close to 1).
		bool isStraightLine = 
		(dotProductStartMid > 0.95f || dotProductStartMid < -0.95f)
		&& (dotProductMidEnd > 0.95f || dotProductMidEnd < -0.95f);

		// If dot product is close to -1, tangents are nearly opposite
		// indicating a potential S-curve
		bool isSCurve = dotProductStartMid < -0.8f || dotProductMidEnd < -0.8f;

		return (isSCurve && !isStraightLine);
	}

	void SubCurve::generatePointsForRendering()
	{
		pointsForRendering.clear();
		float offset = 1.0 / density;
		float u = 0.0;
		while(u <= 1.0)
		{
			glm::vec3 point = computeBezier(u);
			pointsForRendering.push_back(point);
			u += offset;
		}
	}
}
