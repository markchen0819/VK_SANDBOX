#include "../../pch.h"
#include "BlendTree.h"
namespace IHCEngine::Graphics
{
	float BlendTree::GetBlendFactor()
	{
		blendFactor = (currentSpeed - lowerBoundSpeed) / (upperBoundSpeed - lowerBoundSpeed);
		blendFactor = std::clamp(blendFactor, 0.0f, 1.0f);
		return blendFactor;
	}
}
