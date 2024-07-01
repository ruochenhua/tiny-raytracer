#pragma once
#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "aabb.h"
#include <vector>


class hittable_list : public hittable
{
public:
    std::vector<shared_ptr<hittable>> hit_objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) {add(object);}

    void clear() {hit_objects.clear();}

    void add(shared_ptr<hittable> object)
    {
        hit_objects.push_back(object);
        // 更新整个hitable list的aabb
        bbox = aabb(bbox , object->bounding_box());
    }

    bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override
    {
        // hittable list的检测是需要检测所有object
        hit_record tmp_rec;
        bool hit_anything = false;
        auto closest_distance = ray_t.max;

        for(const auto& object : hit_objects)
        {
            if(object->hit(r, interval(ray_t.min, closest_distance), tmp_rec))
            {
                hit_anything = true;
                closest_distance = tmp_rec.t;
                // maybe check closest distance and material(etc.) later
                rec = tmp_rec;
            }
        }

        return hit_anything;
    }

    aabb bounding_box() const override {return bbox;}
private:
    aabb bbox;
};

#endif