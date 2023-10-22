#pragma once
#include <chrono>

namespace IHCEngine::Core
{
	class Time
	{
	public:

		static constexpr int FIXED_UPDATE_TIME_STEP = 70;

		// no duplication
		Time(const  Time&) = delete;
		Time& operator=(const Time&) = delete;

		static void Init();
		static void Reset();
		static void Update();

		static float GetDeltaTime() { return  instance->deltaTime; }
		static float GetUnscaledDeltaTime() { return instance->unscaledDeltaTime;}

		static float GetFixedDeltaTime() { return instance->unscaledFixedDeltaTime * instance->timeScale; }
		static float GetUnscaledFixedDeltaTime() { return instance->unscaledFixedDeltaTime; }

		static void SetTimeScale(float timeScale) { instance->timeScale = timeScale; }
		static float GetTimeScale() { return instance->timeScale; }

		static int GetFrameRate() { return static_cast<int>(instance->calculatedFps); }
		static int GetAverageFrameRate() { return static_cast<int>(instance->averageFps); }
		static void UnlockFrameRate() { instance->minFrameTime = 0.0f; }
		static void LockFrameRate(int maxFrameRate)
		{
			instance->minFrameTime = 1.0f / maxFrameRate;
			instance->frameTimeCounter = 0.0f;
		}

		static float GetElapsedTime() { return  instance->totalTime; }
		static int GetFrameCount() { return instance->frameCount; }

		static bool ShouldExecuteUpdate();

		// Physics
		static void UpdateFixedTime();
		static void SetFixedTime(int maxUpdatePerFrame);
		static bool ShouldExecuteFixedUpdate();


	private:
		Time();
		static Time* GetInstance()
		{
			if (instance == nullptr)
			{
				instance = std::unique_ptr<Time>(new Time()); // Static local variable
			}
			return instance.get();
		}
		static inline std::unique_ptr<Time> instance = nullptr;

		// lock frameRate
		float minFrameTime{0.0};
		float frameTimeCounter{0}; // adds up to 0 ~ timediff for one frame 
		bool shouldExecuteUpdate = true;

		// calculations
		std::chrono::time_point<std::chrono::high_resolution_clock> previousTime;
		float timeScale{ 1.0f }; // slow-mo/ speedup
		float totalTime{ 0.0f };
		float totalFixedTime{ 0.0f };

		// deltaTime
		float unscaledDeltaTime{ 0.0 }; // actual timediff for one frame 
		float deltaTime{ 0.0f };
		
		// physics time
		float unscaledFixedDeltaTime{ 0.0f };

		// other info
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
		float calculatedFps{ 0.0f };
		float averageFps{ 0.0f };
		int frameCount{0};

		// fix for window interruption
		float maxFrameTime = 0.2; //250 ms

	};
}
