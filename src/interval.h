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

    interval(const interval& a, const interval& b)
    {
        min = fmin(a.min, b.min);
        max = fmax(a.max, b.max);
    }

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

    double clamp(double x) const 
    {
        if (x < min) return min;
        if (x > max) return max;

        return x;
    }

    // 
    interval expand(double delta) const 
    {
        auto padding = delta / 2;
        return interval(min-padding, max+padding);
    }

    static const interval empty, universe;
};

// 默认的两个区间，一个空区间，一个是无限区间
const interval interval::empty = interval();
const interval interval::universe = interval(-infinity, infinity);

interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}

#endif