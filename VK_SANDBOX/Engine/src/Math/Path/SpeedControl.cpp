#include "../../pch.h"
#include "SpeedControl.h"
#include "../MathConstants.h"

namespace IHCEngine::Math
{

	void SpeedControl::SetTimings(float easeInEndTime, float easeOutStartTime, float totalTime)
	{
		this->easeInEndTime = easeInEndTime;
		this->easeOutStartTime = easeOutStartTime;
		this->totalTime = totalTime;

		t1 = easeInEndTime / totalTime;
		t2 = easeOutStartTime / totalTime;
		L = (2 * t1 / PI) + (t2 - t1) + 2 * (1 - t2) / PI;
	}

	float SpeedControl::GetDistance(float accumulatedTime)
	{
		assert(!totalTime == 0.0, "Timings not set");
		assert(!(accumulatedTime < 0), "Passing inpossible accumulatedTime");

		if(accumulatedTime > totalTime)
		{
			return 1.0f; // already at end of path
		}
		float timeToFeedIn = accumulatedTime/totalTime;

		if(accumulatedTime < easeInEndTime) // Ease In S1(t)
		{
			return S1(timeToFeedIn);
		}
		else if(accumulatedTime>=easeInEndTime 
			&& accumulatedTime<=easeOutStartTime)  // Constant S2(t)
		{
			return S2(timeToFeedIn);
		}
		else if (accumulatedTime > easeOutStartTime)  // Ease Out S3(t)
		{
			return S3(timeToFeedIn);
		}

		assert(false, "SpeedControl::GetDistance impossible");
	}

	float SpeedControl::S1(float t)
	{
		return (2 * t1 / PI * sinf((t - t1) / t1 * PI / 2) + 2 * t1 / PI) / L;
	}

	float SpeedControl::S2(float t)
	{
		return ((t - t1) + 2 * t1 / PI) / L;
	}

	float SpeedControl::S3(float t)
	{
		return (2 * (1 - t2) / PI * sinf( (t - t2) / (1 - t2) * PI / 2) + 2 * t1 / PI + (t2 - t1)) / L;
	}
}
