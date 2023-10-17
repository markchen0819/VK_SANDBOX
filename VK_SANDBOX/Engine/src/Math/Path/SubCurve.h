#pragma once
#include "PathStructs.h"

namespace IHCEngine::Math
{
	class SubCurve
	{
	public:

		SubCurve(glm::vec3 p0, glm::vec3 a0, glm::vec3 b1, glm::vec3 p1);

		ArcLengthTable& GetArcLengthTable() { return arcLengthTable; }
		std::vector<Segment>& GetSortedSegmentList() {return sortedSegmentList; }
		std::vector<glm::vec3> GetPointsForRendering() { return pointsForRendering; };


	private:

		const float epsilon = 0.001f; // Adjust base on desired precision

		glm::vec3 p0; // Control point p0
		glm::vec3 a0; // Control point ai
		glm::vec3 b1; // Control point bi+1
		glm::vec3 p1; // Control point p1

		ArcLengthTable arcLengthTable;
		std::vector<Segment> sortedSegmentList;


		// Methods to build table with Adaptive approach
		void buildArcLengthTable(); 
		glm::vec3 computeBezier(float u);
		float approximateLengthOfSegment(float uStart, float uEnd);
		glm::vec3 computeBezierDerivative(float u);
		bool isSCurve(float uStart, float uMid, float uEnd);

		// Rendering the subcurve
		float density = 50.0;
		std::vector<glm::vec3> pointsForRendering;
		void generatePointsForRendering();

	};
}
