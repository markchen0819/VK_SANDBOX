#pragma once
#include <chrono>

namespace IHCEngine::Core
{
	class Time
	{
	public:

		static constexpr int FIXED_UPDATE_TIME_STEP = 70;
		static Time& GetInstance()
		{
			static Time instance;  // Static local variable
			return instance;
		}
		static void Init();


		// no duplication
		Time(const  Time&) = delete;
		Time& operator=(const Time&) = delete;


		void Reset();
		void Update();

		float GetDeltaTime() { return  deltaTime; }
		float GetUnscaledDeltaTime() { return unscaledDeltaTime;}

		float GetFixedDeltaTime() { return unscaledFixedDeltaTime * timeScale; }
		float GetUnscaledFixedDeltaTime() { return unscaledFixedDeltaTime; }

		void SetTimeScale(float timeScale) { timeScale = timeScale; }
		float GetTimeScale() { return timeScale; }

		int GetFrameRate() { return static_cast<int>(calculatedFps); }
		void UnlockFrameRate() { minFrameTime = 0.0f; }
		void LockFrameRate(int maxFrameRate)
		{
			minFrameTime = 1.0f / maxFrameRate;
			frameTimeCounter = 0.0f;
		}

		float GetElapsedTime() { return  totalTime; }
		int GetFrameCount() { return frameCount; }

		bool ShouldExecuteUpdate();

		// Physics
		void UpdateFixedTime();
		void SetFixedTime(int maxUpdatePerFrame);
		bool ShouldExecuteFixedUpdate();


	private:
		Time();
		~Time() {};

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
		float calculatedFps{ 0.0f };
		int frameCount{0};

		// fix for window interruption
		float maxFrameTime = 0.2; //250 ms

	};
}
