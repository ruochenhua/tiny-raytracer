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

#endif