#pragma once
namespace IHCEngine::Math
{
	// Distance-time function using Sine interpolation
	// input time will be normalized: 0-t1-t2-t
	// GetDistance returns 0~1 to feed in ArcLength global table

	class SpeedControl
	{
	public:

		void SetTimings(float easeInEndTime, float easeOutStartTime, float totalTime);
		float GetDistance(float accumulatedTime);

	private:
		float easeInEndTime;
		float easeOutStartTime;
		float totalTime=0;

		// Normalized time
		float t1;
		float t2;
		float L;

		float S1(float t); // Ease In
		float S2(float t); // Constant
		float S3(float t); // Ease out
	};

}