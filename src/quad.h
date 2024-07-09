#pragma once
#ifndef QUAD_H
#define QUAD_H

#include "rt_common.h"
#include "hittable.h"

class quad : public hittable
{
public:
    quad(const point3& in_Q, const vec3& in_u, const vec3& in_v, shared_ptr<material> in_mat)
    : Q(in_Q), u(in_u), v(in_v), mat(in_mat)
    {
        set_bounding_box();
    }

    virtual void set_bounding_box()
    {
        auto bbox_diagnal1 = aabb(Q, Q+u+v);
        auto bbox_diagnal2 = aabb(Q+u, Q+v);
        bbox = aabb(bbox_diagnal1, bbox_diagnal2);
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    bool hit(const ray&r, const interval& ray_t, hit_record& rec) const override
    {
        return false;
    }

private:
    // 一个四边形(quad)的表示，是通过一个空间上的点Q，以及从这个点延伸出去的两条边uv表示。
    // 和Q对角的点是Q+u+v，另外还有两个对角点是Q+u和Q+v
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
};

#endif
