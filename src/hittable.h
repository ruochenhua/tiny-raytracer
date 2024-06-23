#pragma once
#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
// 碰撞物体基类

// 碰撞记录
struct hit_record
{
    point3 p;
    vec3 normal;
    double t;
    bool b_front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        // outward_normal must be unit vector
        b_front_face = dot(r.direction(), outward_normal) < 0;
        // set normal always go against in ray
        normal = b_front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif