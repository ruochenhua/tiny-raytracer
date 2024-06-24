#pragma once
#ifndef RT_COMMON_H
#define RT_COMMON_H

#include <cmath>
#include <iostream>
#include <memory>

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

inline double random_double()
{
    // range [0, 1)
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
    // range [min, max)
    return min + (max-min)*random_double();
}

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif