#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "rt_common.h"

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
};

// 简单兰伯特材质
// 仅计算漫反射
class lambertian : public material
{
public:
    lambertian(const color& a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        // 随机选一个方向作为散射方向
        auto scatter_dir = rec.normal + random_unit_vector();
        scattered = ray(rec.p, scatter_dir);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
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
        scattered = ray(rec.p, reflect_dir);
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
        vec3 refracted = refract(unit_dir, rec.normal, ri);
        scattered = ray(rec.p, refracted);
        return true;
    }

private:
    double refraction_index;
};

#endif