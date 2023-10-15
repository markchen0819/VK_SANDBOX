#pragma once

namespace IHCEngine::Math
{
	struct Segment
	{
		float uStart;
		float uEnd;
		float length; // Linear approximation of the segment length
	};

	struct ArcLengthEntry
	{
		float u;
		float length;
	};

	class SubCurve
	{
	public:

		SubCurve(glm::vec3 p0, glm::vec3 a0, glm::vec3 b1, glm::vec3 p1);

	private:

		// Methods to build table with Adaptive approach
		void buildArcLengthTable(); 
		glm::vec3 computeBezier(float u);
		float approximateLengthOfSegment(float uStart, float uEnd);
		glm::vec3 computeBezierDerivative(float u);
		bool isSCurve(float uStart, float uMid, float uEnd);

		const float epsilon = 0.001f; // This value can be adjusted based on desired precision

		glm::vec3 p0; // Control point p0
		glm::vec3 a0; // Control point ai
		glm::vec3 b1; // Control point bi+1
		glm::vec3 p1; // Control point p1

		std::vector<Segment> sortedSegmentList;
		std::vector<ArcLengthEntry> arcLengthTable;

	};
}
