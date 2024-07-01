#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "rt_common.h"
#include "hittable.h"

class sphere : public hittable
{
public:
    // 静止的球体
    sphere(const point3& c, double r, shared_ptr<material> in_mat)
        : center(c), radius(fmax(0, r)), mat(in_mat), is_moving(false)
    { 
        // 静态球体的AABB，就是基于球心，变长为直径的正方体
        auto rvec = vec3(radius, radius, radius);
        bbox = aabb(c - rvec, c + rvec);
    }

    // 平移的球体
    sphere(const point3& c, const point3& target_c, double r, shared_ptr<material> in_mat)
        : center(c), radius(fmax(0, r)), mat(in_mat), is_moving(true), movement_vec(target_c-c)
    {
        // 运动的球体的AABB包含运动前和运动后的区间
        auto rvec = vec3(radius, radius, radius);
        aabb bbox1(c - rvec, c + rvec);
        aabb bbox2(target_c - rvec, target_c + rvec);
        bbox = aabb(bbox1, bbox2);
    }

    // 计算射线和空间球体的相交结果，本质上是将射线的表达式带入球体表达式，看是否能得出结果
    // 若有一个或两个解，则对应射线和球体有一个或两个交点；若无解则不想交
    bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override
    {
        const vec3 oc = sphere_center(r.time()) - r.origin();
        // auto discriminant = b*b - 4*a*c;
        // 二次方程有无解就是看这个式子能否开根号，若小于0则开不出根号没有实数解
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;
        auto discriminant = h*h - a*c;

        if(discriminant < 0)
        {
            return false;
        }

        const auto sqrt_d = sqrt(discriminant);
        auto root = (h-sqrt_d) / a;
        // 这里检查两个可能的解，先检查离射线起点近的这一段
        if(!ray_t.surrounds(root))
        {
            // 再检查远的这一段，都不符合那就没有碰撞的结果
            root = (h + sqrt_d) / a;
            if(!ray_t.surrounds(root))
            {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(root);
        auto outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;
        
        return true;
    }

    point3 sphere_center(double time) const
    {
        // 这里也默认球体在做匀速运动，当然是不准确的
        // 目前仅仅是测试动态模糊效果，可以不必深究
        return center + time*movement_vec;
    }

    aabb bounding_box() const override {return bbox;}
private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
    bool is_moving;     
    vec3 movement_vec;  //运动方向
    aabb bbox;
};

#endif