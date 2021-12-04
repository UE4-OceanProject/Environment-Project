#pragma once

//#include <cmath>


class TMath
{
public:
	static constexpr float EPS = 1e-18f;

	static constexpr float SQRT2 = 1.414213562373095f;

	static constexpr float Rad2Deg = 180.0f / PI;

	static constexpr float Deg2Rad = PI / 180.0f;

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SafeAcos(float r)
	static float SafeAcos(float r);

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SafeAsin(float r)
	static float SafeAsin(float r);

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SafeSqrt(float v)
	static float SafeSqrt(float v);

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SafeLog(float v)
	static float SafeLog(float v);

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SafeLog10(float v)
	static float SafeLog10(float v);

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SafeInvSqrt(float n, float d, float eps = EPS)
	static float SafeInvSqrt(float n, float d, float eps = EPS);

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SafeInv(float v, float eps = EPS)
	static float SafeInv(float v, float eps = EPS);

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SafeDiv(float n, float d, float eps = EPS)
	static float SafeDiv(float n, float d, float eps = EPS);

	//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
	//ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static float SignOrZero(float v)
	static float SignOrZero(float v);

};
