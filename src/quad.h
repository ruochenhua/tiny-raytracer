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
        auto n = cross(u, v);
        normal = unit_vector(n);
        // D=Ax+By+Cz，normal为(A,B,C)，且Q在平面上满足(x,y,z)的值，代入公式可算出D
        D = dot(normal, Q);

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
        // 平面的公式为 Ax+By+Cz=D
        // 可以看作为，存在法线为n=(A,B,C)的平面和空间上从原点出发，到任意一点(x,y,z)的向量v
        // 这两个的点乘dot(n, v)=D
        // 带入射线公式v=P+td,提出t,可得t=(D-dot(n,p))/dot(n,d)
        // 若dot(n,d)为0，则代表射线和平面平行
        // 对于n，可通过叉乘u,v得出
        auto denom = dot(normal, r.direction());
        // 点乘为0，不相交
        if(fabs(denom) < 1e-8)
        {
            return false;
        }
        // 超出射线范围
        auto t = (D - dot(normal, r.origin())) / denom;
        if(!ray_t.contains(t))
        {
            return false;
        }        

        auto intersectoin = r.at(t);
        auto from_q = intersectoin - Q;
        // 点乘代表from_q投影到u/v的方向上再乘以u/v的长度，这里已经可以直接判断了吧？
        // 教程上还进行了更加复杂的变换，没有理解其中的含义
        auto proj_u = dot(from_q, u) / u.length_squared();
        auto proj_v = dot(from_q, v) / v.length_squared();
        interval unit_interval(0, 1);
        // if(proj_u < 0 || proj_u > 1 || proj_v < 0 || proj_v > 1)
        if(!unit_interval.contains(proj_u) || !unit_interval.contains(proj_v))
        {
            return false;
        }

        rec.t = t;
        rec.u = proj_u;
        rec.v = proj_v;
        rec.p = intersectoin;
        rec.mat = mat;
        rec.set_face_normal(r, normal);
        return true;
    }

private:
    // 一个四边形(quad)的表示，是通过一个空间上的点Q，以及从这个点延伸出去的两条边uv表示。
    // 和Q对角的点是Q+u+v，另外还有两个对角点是Q+u和Q+v
    point3 Q;
    vec3 u, v, normal;
    shared_ptr<material> mat;
    aabb bbox;    
    double D;
};

#endif
