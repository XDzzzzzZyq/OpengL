#pragma once

#include "CubicSplineHelpers.h"

typedef unsigned int uint32;

namespace CubicSplineTest
{
    WorldSpace operator* (float c, const WorldSpace& v)
    {
        WorldSpace cv = { c*v.x, c*v.y, c*v.z };
        return cv;
    }

    WorldSpace operator+ (const WorldSpace& lhs, const WorldSpace& rhs)
    {
        WorldSpace v = { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
        return v;
    }

    WorldSpace operator- (const WorldSpace& lhs, const WorldSpace& rhs)
    {
        WorldSpace v = { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
        return v;
    }

    float Dot(const WorldSpace& lhs, const WorldSpace& rhs)
    {
        return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
    }

    float LengthSquared(const WorldSpace& v)
    {
        return Dot(v, v);
    }
}
