#include "../../pch.h"
#include "Time.h"


void IHCEngine::Core::Time::Init()
{
	GetInstance();
}

IHCEngine::Core::Time::Time()
{
	startTime = std::chrono::high_resolution_clock::now();
	previousTime = std::chrono::high_resolution_clock::now();
}

void IHCEngine::Core::Time::Reset()
{
	totalTime = 0;
	frameTimeCounter = 0;
	
	totalFixedTime = 0;

	timeScale = 1.0f;
	frameCount = 0;
	averageFps = 0.0;

	startTime = std::chrono::high_resolution_clock::now();
	previousTime = std::chrono::high_resolution_clock::now();
	shouldExecuteUpdate = false;
}

void IHCEngine::Core::Time::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0;  // Convert to seconds as a double
	unscaledDeltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()- previousTime).count()) * 0.000000001f;
	
	previousTime = currentTime;
	totalTime += unscaledDeltaTime;
	frameTimeCounter += unscaledDeltaTime;

	if (frameTimeCounter <= minFrameTime)
	{
		return; // framerate control
	}

	// window interrupt fix
	if(frameTimeCounter > maxFrameTime)
	{
		frameTimeCounter = maxFrameTime;
	}

	unscaledDeltaTime = frameTimeCounter;
	deltaTime = timeScale * frameTimeCounter;
	calculatedFps = 1.0f / frameTimeCounter;
	shouldExecuteUpdate = true;
	frameCount++;
	averageFps = frameCount / elapsedTime;

	if (minFrameTime == 0.0f)
	{
		frameTimeCounter = 0.0f;
	}
	else
	{
		frameTimeCounter = frameTimeCounter - minFrameTime;
	}
}

bool IHCEngine::Core::Time::ShouldExecuteUpdate()
{
	if (shouldExecuteUpdate)
	{
		shouldExecuteUpdate = false;
		return true;
	}
	return false;
}

void IHCEngine::Core::Time::UpdateFixedTime()
{
	totalFixedTime += unscaledFixedDeltaTime;
}
void IHCEngine::Core::Time::SetFixedTime(int maxUpdatePerFrame)
{
	unscaledFixedDeltaTime = 1.0f / static_cast<float>(maxUpdatePerFrame);
}
bool IHCEngine::Core::Time::ShouldExecuteFixedUpdate()
{
	// Should call fixed update when 
	// total fixed time is less than 1 fixed time step from total time
	return (totalTime - totalFixedTime > unscaledFixedDeltaTime);
}

