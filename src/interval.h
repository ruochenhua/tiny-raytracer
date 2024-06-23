#pragma once
#ifndef INTERVAL_H
#define INTERVAL_H
#include "rt_common.h"
#include <limits>

const double infinity = std::numeric_limits<double>::infinity();
class interval
{
public:
    double min, max;
    interval():min(infinity), max(-infinity) {}

    interval(double in_min, double in_max) : min(in_min), max(in_max) {}

    double size() const
    {
        return max - min;
    }

    bool contains(double x) const
    {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const
    {
        return min < x && x < max;
    }

    static const interval empty, universe;
};

// 默认的两个区间，一个空区间，一个是无限区间
const interval interval::empty = interval();
const interval interval::universe = interval(-infinity, infinity);

#endif