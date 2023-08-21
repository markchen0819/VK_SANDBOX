#pragma once
#include <chrono>
#include <mutex>

namespace IHCEngine::Core
{
	
	class Time
	{
	public:

		static constexpr int FIXED_UPDATE_TIME_STEP = 70;
		static void Init();


		// no duplication
		Time(const  Time&) = delete;
		Time& operator=(const Time&) = delete;


		static void Reset();
		static void Update();

		static float GetDeltaTime() { return  timeInstance->deltaTime; }
		static float GetUnscaledDeltaTime() { return timeInstance->unscaledDeltaTime;}

		static float GetFixedDeltaTime() { return timeInstance->unscaledFixedDeltaTime * timeInstance->timeScale; }
		static float GetUnscaledFixedDeltaTime() { return timeInstance->unscaledFixedDeltaTime; }

		static void SetTimeScale(float timeScale) { timeInstance->timeScale = timeScale; }
		static float GetTimeScale() { return timeInstance->timeScale; }

		static int GetFrameRate() { return static_cast<int>(timeInstance->calculatedFps); }
		static void UnlockFrameRate() { timeInstance->minFrameTime = 0.0f; }
		static void LockFrameRate(int maxFrameRate)
		{
			timeInstance->minFrameTime = 1.0f / maxFrameRate;
			timeInstance->frameTimeCounter = 0.0f;
		}

		static float GetElapsedTime() { return  timeInstance->totalTime; }
		static int GetFrameCount() { return timeInstance->frameCount; }
	

		// Physics
		static void UpdateFixedTime();
		static void SetFixedTime(int maxUpdatePerFrame);
		static bool ShouldExecuteFixedUpdate();


	private:
		Time();
		static Time* GetInstance();

		static std::unique_ptr<Time> timeInstance;
		static std::mutex mutexLock; // incase of future multi-threading 

		// lock frameRate
		float minFrameTime{0.0};
		float frameTimeCounter{0}; // adds up to 0 ~ timediff for one frame 
		
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
		float calculatedFps{ 0.0f };
		int frameCount{0};

	};
}
