#pragma once

#include <string>
#include <vector>
#include <limits>
#include <functional>
#include "CoreMinimal.h"

//C# TO C++ CONVERTER NOTE: Forward class declarations:
//namespace TerrainTopologyPlugin { class Texture2D; }



namespace TerrainTopologyPlugin
{
	enum class VISUALIZE_GRADIENT
	{
		WARM,
		COOL,
		COOL_WARM,
		GREY_WHITE,
		GREY_BLACK,
		BLACK_WHITE
	};

	class Topology
	{

	public:
		class Texture2D
		{
		private:
			std::vector<FVector4*> texture;

			FVector2D* size;

		public:
			virtual ~Texture2D()
			{
				delete size;
			}

			Texture2D(int width, int height);

			void SetPixel(int x, int y, FVector4* value, bool need_scale = true);

			FVector4* GetPixelBilinear(float u, float v);
		};

		static FVector4* const white;
		static FVector2D* const one;



		bool getColoredGradient() const;
		void setColoredGradient(bool value);
	protected:
		bool m_coloredGradient = false;

	private:
		Texture2D* m_posGradient, * m_negGradient, * m_gradient;

	protected:
		bool m_currentColorMode = false;

	public:

		virtual ~Topology()
		{
			delete m_posGradient;
			delete m_negGradient;
			delete m_gradient;
		}

		int getWidth() const;
		int getHeight() const;
		std::vector<float> getHeights() const;

	protected:
		std::function<void()> UpdateMap;

	public:
		void SetUpdateMap(std::function<void()> updateMapTexture);

	public:
		class MapData
		{
		public:
			float terrain_width = 0;
			float terrain_height = 0;
			float terrain_length = 0;

			int tex_width = 0;
			int tex_height = 0;

			float cell_length = 0;

			std::vector<float> heights;
		};

	protected:
		MapData map;

	public:
		void Start(MapData map, std::function<void()> UpdateMap);

		void Update();

		/// <summary>
		/// Default mode is nothing changes.
		/// </summary>
		/// <returns></returns>
	protected:
		virtual bool OnChange();

		/// <summary>
		/// Default mode is no smoothing.
		/// </summary>
		/// <returns></returns>
		virtual bool DoSmoothHeights();

		/// <summary>
		/// Create the map. Update to derivered class to implement.
		/// </summary>
	public:
		virtual std::vector<FVector4*> CreateMap() = 0;

		/// <summary>
		/// Load the provided height map.
		/// </summary>
		/// <param name="fileName"></param>
		/// <param name="bigendian"></param>
		/// <returns></returns>
		static std::vector<float> Load16Bit(const std::wstring& fileName, bool bigendian = false);

		/// <summary>
		/// Get a hight value ranging from 0 - 1.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <returns></returns>
	protected:
		float GetNormalizedHeight(int x, int y);

		/// <summary>
		/// Get a hight value ranging from 0 - actaul height in meters.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <returns></returns>
		float GetHeight(int x, int y);

		/// <summary>
		/// Get the heigts maps first derivative using Evans-Young method.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <returns></returns>
		FVector2D* GetFirstDerivative(int x, int y);

		/// <summary>
		/// Get the heigts maps first and second derivative using Evans-Young method.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="d1"></param>
		/// <param name="d2"></param>
		void GetDerivatives(int x, int y, FVector2D*& d1, FVector*& d2);

		/// <summary>
		/// Smooth heights using a 5X5 Gaussian kernel.
		/// </summary>
		void SmoothHeightMap();

		/// <summary>
		/// Take a parameter, rescale it and return as a 
		/// color using a gradient. Helps visualize some 
		/// parameters better especially if they have a 
		/// wide dynamic range and can be negative. 
		/// </summary>
		/// <param name="v">The parameter</param>
		/// <param name="exponent">Amount to rescale the dynamic range. 
		/// Will change if terrain cell length changes.</param>
		/// <param name="nonNegative">If the parameter is always positive</param>
		/// <returns></returns>
		FVector4* Colorize(float v, float exponent, bool nonNegative);



	private:
		void CreateGradients(bool colored);

		Texture2D* CreateGradient(VISUALIZE_GRADIENT g);

		Texture2D* CreateWarmGradient();

		Texture2D* CreateCoolGradient();

		Texture2D* CreateCoolToWarmGradient();

		Texture2D* CreateGreyToWhiteGradient();

		Texture2D* CreateGreyToBlackGradient();

		Texture2D* CreateBlackToWhiteGradient();

	};

}
