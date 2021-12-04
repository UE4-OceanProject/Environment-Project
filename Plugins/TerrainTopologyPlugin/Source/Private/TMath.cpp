#include "TMath.h"


float TMath::SafeAcos(float r)
{
	return static_cast<float>(FMath::Acos(FMath::Min(1.0f, FMath::Max(-1.0f, r))));
}

float TMath::SafeAsin(float r)
{
	return static_cast<float>(FMath::Asin(FMath::Min(1.0f, FMath::Max(-1.0f, r))));
}

float TMath::SafeSqrt(float v)
{
	if (v <= 0.0f)
	{
		return 0.0f;
	}
	return static_cast<float>(FMath::Sqrt(v));
}

float TMath::SafeLog(float v)
{
	if (v <= 0.0f)
	{
		return 0.0f;
	}
	return static_cast<float>(FMath::Loge(v));
}

float TMath::SafeLog10(float v)
{
	if (v <= 0.0)
	{
		return 0.0f;
	}
	return static_cast<float>(FMath::LogX(10, v));

}

float TMath::SafeInvSqrt(float n, float d, float eps)
{
	if (d <= 0.0f)
	{
		return 0.0f;
	}
	d = static_cast<float>(FMath::Sqrt(d));
	if (d < eps)
	{
		return 0.0f;
	}
	return n / d;
}

float TMath::SafeInv(float v, float eps)
{
	if (FMath::Abs(v) < eps)
	{
		return 0.0f;
	}
	return 1.0f / v;
}

float TMath::SafeDiv(float n, float d, float eps)
{
	if (FMath::Abs(d) < eps)
	{
		return 0.0f;
	}
	return n / d;
}

float TMath::SignOrZero(float v)
{
	if (v == 0)
	{
		return 0;
	}
	return FMath::Sign(v);
}