#pragma once
#ifndef AABB_H
#define AABB_H

#include "rt_common.h"

class aabb
{
public:
    interval x, y, z;
    aabb() {}

    aabb(const interval& ix, const interval& iy, const interval& iz)
        : x(ix), y(iy), z(iz) 
        {
            pad_to_minimums();
        }        

    // 基于两个点构成的aabb区间
    aabb(const point3& a, const point3& b)
    {
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);    
        pad_to_minimums();
    }

    aabb(const aabb& box0, const aabb& box1)
    {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
        pad_to_minimums();
    }
    
    const interval& axis_interval(int n) const 
    {
        if(n == 1) return y;
        if(n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const
    {
        const point3& ray_origin = r.origin();
        const vec3& ray_dir = r.direction();

        for(int axis = 0; axis < 3; ++axis)
        {
            const interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            // 根据计算公式，获取射线在指定轴上和对应aabb的interval的相交的两个点的值
            auto t0 = (ax.min - ray_origin[axis]) * adinv;            
            auto t1 = (ax.max - ray_origin[axis]) * adinv;

            // adinv可以是负数(射线方向为负)，因此有可能t0>t1，这里要分情况处理
            if(t0 < t1)
            {
                if(t0 > ray_t.min) ray_t.min = t0;
                if(t1 < ray_t.max) ray_t.max = t1;
            }
            else
            {
                if(t1 > ray_t.min) ray_t.min = t1;
                if(t0 < ray_t.max) ray_t.max = t0;
            }

            /* 
            这里第一次看有点迷糊
            其实这里主要是判断射线和AABB相交的情况，需要射线在AABB三个轴上相交的结果都是有重合的
            区间重合的判定条件，是两个区间中较小区间的最大值要大于较大区间的最小值。如下所示
            |-------|
                |--------|
            下面的判定就是当较小区间的最大值还没有较大区间的最小值大的时候，两个区间没有重叠。也就没有命中这个AABB
            */ 
            if(ray_t.max <= ray_t.min)
            {
                return false;
            }
        }
        return true;
    }

    int longest_axis() const
    {
        // 找到这个aabb最长的轴，返回索引
        if(x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }


    static const aabb empty, universe;

private:
    // aabb不能无限小，会有数学问题，这里保证一个最小值
    void pad_to_minimums()
    {
        double delta = 0.0001;
        if(x.size() < delta) x = x.expand(delta);
        if(y.size() < delta) y = y.expand(delta);
        if(z.size() < delta) z = z.expand(delta);
    }
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

#endif