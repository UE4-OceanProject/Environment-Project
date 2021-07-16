#pragma once

//----------------------------------------------------------------------------------------
//	Copyright © 2004 - 2021 Tangible Software Solutions, Inc.
//	This class can be used by anyone provided that the copyright notice remains intact.
//
//	This class includes methods to convert multidimensional arrays to C++ vectors.
//----------------------------------------------------------------------------------------
#include <vector>

class RectangularVectors
{
public:
    static std::vector<std::vector<float>> RectangularFloatVector(int size1, int size2)
    {
        std::vector<std::vector<float>> newVector(size1);
        for (int vector1 = 0; vector1 < size1; vector1++)
        {
            newVector[vector1] = std::vector<float>(size2);
        }

        return newVector;
    }

    static std::vector<std::vector<std::vector<float>>> RectangularFloatVector(int size1, int size2, int size3)
    {
        std::vector<std::vector<std::vector<float>>> newVector(size1);
        for (int vector1 = 0; vector1 < size1; vector1++)
        {
            newVector[vector1] = std::vector<std::vector<float>>(size2);
            for (int vector2 = 0; vector2 < size2; vector2++)
            {
                newVector[vector1][vector2] = std::vector<float>(size3);
            }
        }

        return newVector;
    }
};
