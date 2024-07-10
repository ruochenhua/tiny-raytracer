#pragma once
#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

/*
 模拟烟雾类介质
 还有次表面散射（在皮肤、玉石等上也会出现）
 烟雾和硬表面是完全不一样的东西，在处理上可以看作，在体积内某处有一定概率出现的平面
 光线穿透介质时，有概率在介质体积内的光路上任意一点散射(scatter)，也有可能穿透整个介质。越密集的介质穿透的概率越小，同理体积越大的介质穿透的概率也越小。
*/

#include "rt_common.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable
{
public:
    constant_medium(shared_ptr<hittable> in_boundary, double in_density, shared_ptr<texture> tex)
    : boundary(in_boundary), neg_inv_density(-1/in_density),
      phase_function(make_shared<isotropic>(tex))
      {}

    constant_medium(shared_ptr<hittable> in_boundary, double in_density, const color& albedo)
    : boundary(in_boundary), neg_inv_density(-1/in_density),
      phase_function(make_shared<isotropic>(albedo))
      {}

    bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override
    {
        const bool enbale_debug = false;
        const bool debugging = enbale_debug && random_double() < 0.00001;   //点太多，debug信息随机筛选一部分显示

        hit_record rec1, rec2;

        // 光线不会击中边界，就直接返回
        if(!boundary->hit(r, interval::universe, rec1))
        {
            return false;
        }

        // 在击中点往后一段再测一次，如果不会击中也返回false
        // 这个做法应该是规避当射线起点处于介质中时，下面的算法出现光线乱反弹导致画面出问题的情况
        // todo 后面可以试一下相机处于介质中的情况，并且去掉这段逻辑看下什么表现
        if(!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))
        {
            return false;
        }

        if (debugging) std::clog << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        // 这里应该是超出射线范围了的情况
        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto ray_length = r.direction().length();   // r的direction是没有normalized的，和对焦距离相关。所以下面的值需要和这个长度相乘
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length; // 这里是射线穿过介质边界经过的距离
        auto hit_distance = neg_inv_density * log(random_double());

        // 这里取了一个随机数作为长度比较， 体现光线穿透介质和其密度负相关的含义
        // 随机的长度大，则代表光线穿透，没有碰撞
        if (hit_distance > distance_inside_boundary)
            return false;

        // 这里就是假设光线进入介质并在介质其中的某一个点进行散射了
        // 由于rec.t对应r，这里要除以ray_length，因为t是和direction的相乘的
        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::clog << "hit_distance = " <<  hit_distance << '\n'
                      << "rec.t = " <<  rec.t << '\n'
                      << "rec.p = " <<  rec.p << '\n';
        }

        // 写入击中的材质数据
        rec.normal = vec3(1,0,0);    // 无所谓填什么，不影响
        rec.b_front_face = true;     // 这个也是
        rec.mat = phase_function;


        return true;
    }

    virtual aabb bounding_box() const override
    {
        return boundary->bounding_box();
    }

private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function;
};

#endif