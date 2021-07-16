#include "Topolgy.h"
#include "FMathTT.h"



namespace TerrainTopologyPlugin
{

    FVector4* const Topology::white = new FVector4(1);
    FVector2D* const Topology::one = new FVector2D(1);

    bool Topology::getColoredGradient() const
    {
        return m_coloredGradient;
    }

    void Topology::setColoredGradient(bool value)
    {
        m_coloredGradient = value;
    }

    int Topology::getWidth() const
    {
        return map.tex_width;
    }

    int Topology::getHeight() const
    {
        return map.tex_height;
    }

    std::vector<float> Topology::getHeights() const
    {
        return map.heights;
    }

    void Topology::SetUpdateMap(std::function<void()> updateMapTexture)
    {
        UpdateMap = updateMapTexture;
    }

    void Topology::Start(MapData newmap, std::function<void()> newUpdateMap)
    {
        this->map = newmap;
        this->UpdateMap = newUpdateMap;

        //Create color gradients to help visualize the maps.
        m_currentColorMode = m_coloredGradient;

        CreateGradients(m_coloredGradient);

        //If required smooth the heights.
        if (DoSmoothHeights())
        {
            SmoothHeightMap();
        }

        newUpdateMap();
    }

    void Topology::Update()
    {
        //If settings changed then recreate map.
        if (OnChange())
        {
            CreateGradients(m_coloredGradient);
            UpdateMap();

            m_currentColorMode = m_coloredGradient;
        }
    }

    bool Topology::OnChange()
    {
        return false;
    }

    bool Topology::DoSmoothHeights()
    {
        return false;
    }

    std::vector<float> Topology::Load16Bit(const std::wstring& fileName, bool bigendian)
    {
        std::vector<unsigned char> bytes = { 0,0,0 };//System::IO::File::ReadAllBytes(fileName);

        int size = bytes.size() / 2;
        std::vector<float> data(size);

        for (int x = 0, i = 0; x < size; x++)
        {
            data[x] = (bigendian) ? (bytes[i++] * 256.0f + bytes[i++]) : (bytes[i++] + bytes[i++] * 256.0f);
            data[x] /= std::numeric_limits<unsigned short>::max();
        }

        return data;
    }

    float Topology::GetNormalizedHeight(int x, int y)
    {
        x = FMath::Clamp(x, 0, map.tex_width - 1);
        y = FMath::Clamp(y, 0, map.tex_height - 1);

        return map.heights[x + y * map.tex_width];
    }

    float Topology::GetHeight(int x, int y)
    {
        return GetNormalizedHeight(x, y) * map.terrain_height;
    }

    FVector2D* Topology::GetFirstDerivative(int x, int y)
    {
        float w = map.cell_length;
        float z1 = GetHeight(x - 1, y + 1);
        float z2 = GetHeight(x + 0, y + 1);
        float z3 = GetHeight(x + 1, y + 1);
        float z4 = GetHeight(x - 1, y + 0);
        float z6 = GetHeight(x + 1, y + 0);
        float z7 = GetHeight(x - 1, y - 1);
        float z8 = GetHeight(x + 0, y - 1);
        float z9 = GetHeight(x + 1, y - 1);

        //p, q
        float zx = (z3 + z6 + z9 - z1 - z4 - z7) / (6.0f * w);
        float zy = (z1 + z2 + z3 - z7 - z8 - z9) / (6.0f * w);

        return new FVector2D(-zx, -zy);
    }

    void Topology::GetDerivatives(int x, int y, FVector2D*& d1, FVector*& d2)
    {
        float w = map.cell_length;
        float w2 = w * w;
        float z1 = GetHeight(x - 1, y + 1);
        float z2 = GetHeight(x + 0, y + 1);
        float z3 = GetHeight(x + 1, y + 1);
        float z4 = GetHeight(x - 1, y + 0);
        float z5 = GetHeight(x + 0, y + 0);
        float z6 = GetHeight(x + 1, y + 0);
        float z7 = GetHeight(x - 1, y - 1);
        float z8 = GetHeight(x + 0, y - 1);
        float z9 = GetHeight(x + 1, y - 1);

        //p, q
        float zx = (z3 + z6 + z9 - z1 - z4 - z7) / (6.0f * w);
        float zy = (z1 + z2 + z3 - z7 - z8 - z9) / (6.0f * w);

        //r, t, s
        float zxx = (z1 + z3 + z4 + z6 + z7 + z9 - 2.0f * (z2 + z5 + z8)) / (3.0f * w2);
        float zyy = (z1 + z2 + z3 + z7 + z8 + z9 - 2.0f * (z4 + z5 + z6)) / (3.0f * w2);
        float zxy = (z3 + z7 - z1 - z9) / (4.0f * w2);

        d1 = new FVector2D(-zx, -zy);
        d2 = new FVector(-zxx, -zyy, -zxy); //is zxy or -zxy?
    }

    void Topology::SmoothHeightMap()
    {
        auto heights = std::vector<float>(map.tex_width * map.tex_height);

        auto gaussianKernel5 = std::vector<std::vector<float>>
        {
            {1, 4, 6, 4, 1},
            {4, 16, 24, 16, 4},
            {6, 24, 36, 24, 6},
            {4, 16, 24, 16, 4},
            {1, 4, 6, 4, 1}
        };

        float gaussScale = 1.0f / 256.0f;

        for (int y = 0; y < map.tex_height; y++)
        {
            for (int x = 0; x < map.tex_width; x++)
            {
                float sum = 0;

                for (int i = 0; i < 5; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        int xi = x - 2 + i;
                        int yi = y - 2 + j;

                        sum += GetNormalizedHeight(xi, yi) * gaussianKernel5[i][j] * gaussScale;
                    }
                }

                heights[x + y * map.tex_width] = sum;
            }
        }

        map.heights = heights;
    }

    FVector4* Topology::Colorize(float v, float exponent, bool nonNegative)
    {
        if (exponent > 0)
        {
            float sign = FMath::Sign(v); //Was SignOrZero c#
            float pow = FMath::Pow(10, exponent);
            float log = FMath::Loge(1.0f + pow * FMath::Abs(v));
            //float log = FMath::Log(1.0f + pow * FMath::Abs(v));

            v = sign * log;
        }

        if (nonNegative)
            return this->m_gradient->GetPixelBilinear(v, 0);
        else
        {
            if (v > 0)
                return this->m_posGradient->GetPixelBilinear(v, 0);
            else
                return this->m_negGradient->GetPixelBilinear(-v, 0);
        }
    }

    Topology::Texture2D::Texture2D(int width, int height)
    {
        size = new FVector2D(width, height);
        texture = std::vector<FVector4*>(width * height);
    }

    void Topology::Texture2D::SetPixel(int x, int y, FVector4* value, bool need_scale)
    {
        if (need_scale)
        {
            value->operator*= (1.0f / 255.0f);
            //value *= (1.0f / 255.0f);
        }

        texture[y * size->X + x] = value;
    }

    FVector4* Topology::Texture2D::GetPixelBilinear(float u, float v)
    {
        // Pixel centers
        auto pcu = u * size->X - 0.5f;
        auto pcv = v * size->Y - 0.5f;

        FVector2D* pixel = new FVector2D(pcu, pcv);

        FVector2D bound = size - one;
        
        FVector2D p0_floor = FVector2D(FMath::FloorToFloat(pixel->X), FMath::FloorToFloat(pixel->Y));
        // Offset to get 4 closest to pixel
        FVector2D p0 = FMath::Clamp(p0_floor, FVector2D::ZeroVector, bound);
        FVector2D p1 = FMath::Clamp(p0 + FVector2D(0, 1), FVector2D::ZeroVector, bound);
        FVector2D p2 = FMath::Clamp(p0 + FVector2D(1, 0), FVector2D::ZeroVector, bound);
        FVector2D p3 = FMath::Clamp(p0 + FVector2D(1, 1), FVector2D::ZeroVector, bound);

        // get the values at each pixel
        FVector4* v0 = texture[static_cast<int>(p0.Y * size->X) + static_cast<int>(p0.X)];
        FVector4* v1 = texture[static_cast<int>(p1.Y * size->X) + static_cast<int>(p1.X)];
        FVector4* v2 = texture[static_cast<int>(p2.Y * size->X) + static_cast<int>(p2.X)];
        FVector4* v3 = texture[static_cast<int>(p3.Y * size->X) + static_cast<int>(p3.X)];

        // Calculate x
        auto R1 = FMath::Lerp(v0, v2, (pcu - p0.X));
        auto R2 = FMath::Lerp(v1, v3, (pcu - p0.X));

        // Calculate y
        FVector4* P = FMath::Lerp(R1, R2, (pcv - p0.Y));

        // Clamp between 0 and 1

//C# TO C++ CONVERTER TODO TASK: A 'delete pixel' statement was not added since pixel was passed to a method or constructor. Handle memory management manually.
        FVector4* zerovec = new FVector4(0);
        FVector4* onevec = new FVector4(0);
        return FMath::Clamp(P, zerovec, onevec);
    }

    void Topology::CreateGradients(bool colored)
    {
        if (colored)
        {
            m_gradient = CreateGradient(VISUALIZE_GRADIENT::COOL_WARM);
            m_posGradient = CreateGradient(VISUALIZE_GRADIENT::WARM);
            m_negGradient = CreateGradient(VISUALIZE_GRADIENT::COOL);
        }
        else
        {
            m_gradient = CreateGradient(VISUALIZE_GRADIENT::BLACK_WHITE);
            m_posGradient = CreateGradient(VISUALIZE_GRADIENT::GREY_WHITE);
            m_negGradient = CreateGradient(VISUALIZE_GRADIENT::GREY_BLACK);
        }
    }

    Topology::Texture2D* Topology::CreateGradient(VISUALIZE_GRADIENT g)
    {
        switch (g)
        {
        case VISUALIZE_GRADIENT::WARM:
            return CreateWarmGradient();

        case VISUALIZE_GRADIENT::COOL:
            return CreateCoolGradient();

        case VISUALIZE_GRADIENT::COOL_WARM:
            return CreateCoolToWarmGradient();

        case VISUALIZE_GRADIENT::GREY_WHITE:
            return CreateGreyToWhiteGradient();

        case VISUALIZE_GRADIENT::GREY_BLACK:
            return CreateGreyToBlackGradient();

        case VISUALIZE_GRADIENT::BLACK_WHITE:
            return CreateBlackToWhiteGradient();
        }

        return NULL;
    }

    Topology::Texture2D* Topology::CreateWarmGradient()
    {
        Texture2D* gradient = new Texture2D(5, 1);
        gradient->SetPixel(0, 0, new FVector4(80, 230, 80, 255));
        gradient->SetPixel(1, 0, new FVector4(180, 230, 80, 255));
        gradient->SetPixel(2, 0, new FVector4(230, 230, 80, 255));
        gradient->SetPixel(3, 0, new FVector4(230, 180, 80, 255));
        gradient->SetPixel(4, 0, new FVector4(230, 80, 80, 255));

        return gradient;
    }

    Topology::Texture2D* Topology::CreateCoolGradient()
    {
        Texture2D* gradient = new Texture2D(5, 1);
        gradient->SetPixel(0, 0, new FVector4(80, 230, 80, 255));
        gradient->SetPixel(1, 0, new FVector4(80, 230, 180, 255));
        gradient->SetPixel(2, 0, new FVector4(80, 230, 230, 255));
        gradient->SetPixel(3, 0, new FVector4(80, 180, 230, 255));
        gradient->SetPixel(4, 0, new FVector4(80, 80, 230, 255));

        return gradient;
    }

    Topology::Texture2D* Topology::CreateCoolToWarmGradient()
    {
        Texture2D* gradient = new Texture2D(9, 1);
        gradient->SetPixel(0, 0, new FVector4(80, 80, 230, 255));
        gradient->SetPixel(1, 0, new FVector4(80, 180, 230, 255));
        gradient->SetPixel(2, 0, new FVector4(80, 230, 230, 255));
        gradient->SetPixel(3, 0, new FVector4(80, 230, 180, 255));
        gradient->SetPixel(4, 0, new FVector4(80, 230, 80, 255));
        gradient->SetPixel(5, 0, new FVector4(180, 230, 80, 255));
        gradient->SetPixel(6, 0, new FVector4(230, 230, 80, 255));
        gradient->SetPixel(7, 0, new FVector4(230, 180, 80, 255));
        gradient->SetPixel(8, 0, new FVector4(230, 80, 80, 255));

        return gradient;
    }

    Topology::Texture2D* Topology::CreateGreyToWhiteGradient()
    {
        Texture2D* gradient = new Texture2D(3, 1);
        gradient->SetPixel(0, 0, new FVector4(128, 128, 128, 255));
        gradient->SetPixel(1, 0, new FVector4(192, 192, 192, 255));
        gradient->SetPixel(2, 0, new FVector4(255, 255, 255, 255));

        return gradient;
    }

    Topology::Texture2D* Topology::CreateGreyToBlackGradient()
    {
        Texture2D* gradient = new Texture2D(3, 1);
        gradient->SetPixel(0, 0, new FVector4(128, 128, 128, 255));
        gradient->SetPixel(1, 0, new FVector4(64, 64, 64, 255));
        gradient->SetPixel(2, 0, new FVector4(0, 0, 0, 255));

        return gradient;
    }

    Topology::Texture2D* Topology::CreateBlackToWhiteGradient()
    {
        Texture2D* gradient = new Texture2D(5, 1);
        gradient->SetPixel(0, 0, new FVector4(0, 0, 0, 255));
        gradient->SetPixel(1, 0, new FVector4(64, 64, 64, 255));
        gradient->SetPixel(2, 0, new FVector4(128, 128, 128, 255));
        gradient->SetPixel(3, 0, new FVector4(192, 192, 192, 255));
        gradient->SetPixel(4, 0, new FVector4(255, 255, 255, 255));

        return gradient;
    }

}