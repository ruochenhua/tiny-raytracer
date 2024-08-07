#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "rt_common.h"
#include "texture.h"

class hit_record;

// 材质基类
class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const
    {
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const 
    {
        return color(0,0,0);
    }
};

// 简单兰伯特材质
// 仅计算漫反射
class lambertian : public material
{
public:
    lambertian(const color& a) : tex(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> in_tex) : tex(in_tex) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        // 随机选一个方向作为散射方向
        // 优化为从法线方向延伸出去再各个方向均匀概率的随机，这种随机分布效果比在集中点在法线正向半球随机分布要好
        auto scatter_dir = rec.normal + random_unit_vector();
        scattered = ray(rec.p, scatter_dir, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    shared_ptr<texture> tex;
};

// 金属材质
// 仅计算镜面反射和一些粗糙度
class metal : public material
{
public:
    metal(const color& a, double r) : albedo(a), roughness(r < 1 ? r : 1) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        vec3 reflect_dir = reflect(r_in.direction(), rec.normal);
        reflect_dir = unit_vector(reflect_dir) + (roughness*random_unit_vector());
        scattered = ray(rec.p, reflect_dir, r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
private:
    color albedo;
    double roughness;
};


// 透明(电介质)材质
// 仅计算折射
class dielectric : public material
{
public:
    dielectric(double r) : refraction_index(r) {}
     virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        // 默认空气是1，从空气射入和射出的情况要考虑
        double ri = rec.b_front_face ? (1.0 / refraction_index) : refraction_index;
        vec3 unit_dir = unit_vector(r_in.direction());

        // 在某种折射率的比值和入射角的情况下，光线入射是无法发生折射的，而是会发生反射
        // A*sin(A_angle) = B*sin(B_angle)
        // A / B * sin(A_angle) = sin(B_angle), A/B*sin(A_angle)范围在[-1,1]之间
        double cos_theta = fmin(dot(-unit_dir, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool not_refract = ri * sin_theta > 1.0 || ri * sin_theta < -1;
        vec3 new_dir;
        if(not_refract || reflectance(cos_theta, ri) > random_double())     // 为什么这里是个[0,1)的随机数作为判断？
        {
            new_dir = reflect(unit_dir, rec.normal);
        }
        else
        {
            new_dir = refract(unit_dir, rec.normal, ri);
        }
        scattered = ray(rec.p, new_dir, r_in.time());
        return true;
    }

private:
    double refraction_index;

    // Schlick近似：真实玻璃的反射是和入射角度相关的，可以用Schlick近似方法计算反射比
    static double reflectance(double cosine, double refraction_index)
    {
        auto r0 = (1-refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1-cosine), 5);
    }

};

class diffuse_light : public material
{
public:
    diffuse_light(shared_ptr<texture> in_tex) : tex(in_tex) {}
    diffuse_light(const color& emit) : tex(make_shared<solid_color>(emit)) {}

    color emitted(double u, double v, const point3& p) const override
    {
        return tex->value(u, v, p);
    }


private:
    shared_ptr<texture> tex;
};

// 各向同性材质，可用于烟雾等散射
class isotropic : public material
{
public:
    isotropic(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    isotropic(shared_ptr<texture> in_tex) : tex(in_tex) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        // 向各个方向散射，而不只是法线正向的那个面的方向
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);

        return true;
    }
private:
    shared_ptr<texture> tex;
};

#endif