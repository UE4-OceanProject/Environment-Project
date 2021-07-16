#include "CurvatureMap.h"
#include "FMathTT.h"



namespace TerrainTopologyPlugin
{

	bool CurvatureMap::OnChange()
	{
		return m_currentType != m_curvatureType || m_currentColorMode != m_coloredGradient;
	}

	bool CurvatureMap::DoSmoothHeights()
	{
		return true;
	}

	std::vector<FVector4*> CurvatureMap::CreateMap()
	{
		m_currentType = m_curvatureType;

		std::vector<FVector4*> newmap(getWidth() * getHeight());

		for (int y = 0; y < getHeight(); y++)
		{
			for (int x = 0; x < getWidth(); x++)
			{
				FVector2D* d1;
				FVector* d2;
				GetDerivatives(x, y, d1, d2);

				float curvature = 0;
				FVector4* color = white;

				switch (m_curvatureType)
				{
				case CURVATURE_TYPE::PLAN:
					curvature = PlanCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 1.5f, false);
					break;

				case CURVATURE_TYPE::HORIZONTAL:
					curvature = HorizontalCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.0f, false);
					break;

				case CURVATURE_TYPE::VERTICAL:
					curvature = VerticalCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.0f, false);
					break;

				case CURVATURE_TYPE::MEAN:
					curvature = MeanCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.4f, false);
					break;

				case CURVATURE_TYPE::GAUSSIAN:
					curvature = GaussianCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 5.0f, false);
					break;

				case CURVATURE_TYPE::MINIMAL:
					curvature = MinimalCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.5f, false);
					break;

				case CURVATURE_TYPE::MAXIMAL:
					curvature = MaximalCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.5f, false);
					break;

				case CURVATURE_TYPE::UNSPHERICITY:
					curvature = UnsphericityCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.0f, true);
					break;

				case CURVATURE_TYPE::ROTOR:
					curvature = RotorCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.5f, false);
					break;

				case CURVATURE_TYPE::DIFFERENCE:
					curvature = DifferenceCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.0f, false);
					break;

				case CURVATURE_TYPE::HORIZONTAL_EXCESS:
					curvature = HorizontalExcessCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.0f, true);
					break;

				case CURVATURE_TYPE::VERTICAL_EXCESS:
					curvature = VerticalExcessCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 2.0f, true);
					break;

				case CURVATURE_TYPE::RING:
					curvature = RingCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 5.0f, true);
					break;

				case CURVATURE_TYPE::ACCUMULATION:
					curvature = AccumulationCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
					color = Colorize(curvature, 5.0f, false);
					break;
				};

				newmap[x + y * getWidth()] = color;
			}

		}

		return newmap;
	}

	float CurvatureMap::PlanCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float zx2 = zx * zx;
		float zy2 = zy * zy;
		float p = zx2 + zy2;

		float n = zy2 * zxx - 2.0f * zxy * zx * zy + zx2 * zyy;
		float d = FMath::Pow(p, 1.5f);

		return FMathTT::SafeDiv(n, d);
	}

	float CurvatureMap::HorizontalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float zx2 = zx * zx;
		float zy2 = zy * zy;
		float p = zx2 + zy2;

		float n = zy2 * zxx - 2.0f * zxy * zx * zy + zx2 * zyy;
		float d = p * FMath::Pow(p + 1, 0.5f);

		return FMathTT::SafeDiv(n, d);
	}

	float CurvatureMap::VerticalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float zx2 = zx * zx;
		float zy2 = zy * zy;
		float p = zx2 + zy2;

		float n = zx2 * zxx + 2.0f * zxy * zx * zy + zy2 * zyy;
		float d = p * FMath::Pow(p + 1, 1.5f);

		return FMathTT::SafeDiv(n, d);
	}

	float CurvatureMap::MeanCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float zx2 = zx * zx;
		float zy2 = zy * zy;
		float p = zx2 + zy2;

		float n = (1 + zy2) * zxx - 2.0f * zxy * zx * zy + (1 + zx2) * zyy;
		float d = 2 * FMath::Pow(p + 1, 1.5f);

		return FMathTT::SafeDiv(n, d);
	}

	float CurvatureMap::GaussianCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float zx2 = zx * zx;
		float zy2 = zy * zy;
		float p = zx2 + zy2;

		float n = zxx * zyy - zxy * zxy;
		float d = FMath::Pow(p + 1, 2.f);

		return FMathTT::SafeDiv(n, d);
	}

	float CurvatureMap::MinimalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float H = MeanCurvature(zx, zy, zxx, zyy, zxy);
		float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);

		return H - FMathTT::SafeSqrt(H * H - K);
	}

	float CurvatureMap::UnsphericityCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float H = MeanCurvature(zx, zy, zxx, zyy, zxy);
		float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);

		return FMathTT::SafeSqrt(H * H - K);
	}

	float CurvatureMap::MaximalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float H = MeanCurvature(zx, zy, zxx, zyy, zxy);
		float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);

		return H + FMathTT::SafeSqrt(H * H - K);
	}

	float CurvatureMap::RotorCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float zx2 = zx * zx;
		float zy2 = zy * zy;
		float p = zx2 + zy2;

		float n = (zx2 - zy2) * zxy - zx * zy * (zxx - zyy);
		float d = FMath::Pow(p, 1.5f);

		return FMathTT::SafeDiv(n, d);
	}

	float CurvatureMap::DifferenceCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float Kv = VerticalCurvature(zx, zy, zxx, zyy, zxy);
		float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);

		return 0.5f * (Kv - Kh);
	}

	float CurvatureMap::HorizontalExcessCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);
		float Kmin = MinimalCurvature(zx, zy, zxx, zyy, zxy);

		return Kh - Kmin;
	}

	float CurvatureMap::VerticalExcessCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float Kv = VerticalCurvature(zx, zy, zxx, zyy, zxy);
		float Kmin = MinimalCurvature(zx, zy, zxx, zyy, zxy);

		return Kv - Kmin;
	}

	float CurvatureMap::RingCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float H = MeanCurvature(zx, zy, zxx, zyy, zxy);
		float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);
		float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);

		return 2 * H * Kh - Kh * Kh - K;
	}

	float CurvatureMap::AccumulationCurvature(float zx, float zy, float zxx, float zyy, float zxy)
	{
		float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);
		float Kv = VerticalCurvature(zx, zy, zxx, zyy, zxy);

		return Kh * Kv;
	}

}