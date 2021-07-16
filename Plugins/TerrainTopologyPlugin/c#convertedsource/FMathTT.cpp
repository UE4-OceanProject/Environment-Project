#include "FMathTT.h"

namespace TerrainTopologyPlugin
{

	float FMathTT::SafeAcos(float r)
	{
		return static_cast<float>(std::acos(std::min(1.0f, std::max(-1.0f, r))));
	}

	float FMathTT::SafeAsin(float r)
	{
		return static_cast<float>(std::asin(std::min(1.0f, std::max(-1.0f, r))));
	}

	float FMathTT::SafeSqrt(float v)
	{
		if (v <= 0.0f)
		{
			return 0.0f;
		}
		return static_cast<float>(std::sqrt(v));
	}

	float FMathTT::SafeLog(float v)
	{
		if (v <= 0.0f)
		{
			return 0.0f;
		}
		return static_cast<float>(std::log(v));
	}

	float FMathTT::SafeLog10(float v)
	{
		if (v <= 0.0)
		{
			return 0.0f;
		}
		return static_cast<float>(std::log10(v));
	}

	float FMathTT::SafeInvSqrt(float n, float d, float eps)
	{
		if (d <= 0.0f)
		{
			return 0.0f;
		}
		d = static_cast<float>(std::sqrt(d));
		if (d < eps)
		{
			return 0.0f;
		}
		return n / d;
	}

	float FMathTT::SafeInv(float v, float eps)
	{
		if (std::abs(v) < eps)
		{
			return 0.0f;
		}
		return 1.0f / v;
	}

	float FMathTT::SafeDiv(float n, float d, float eps)
	{
		if (std::abs(d) < eps)
		{
			return 0.0f;
		}
		return n / d;
	}

	float FMathTT::SignOrZero(float v)
	{
		if (v == 0)
		{
			return 0;
		}
		return FMath::Sign(v);
	}
}
