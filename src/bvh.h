#pragma once
#ifndef BVH_H
#define BVH_H

#include "rt_common.h"
#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>

class bvh_node : public hittable
{
public:
    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end)
    {
        //int axis = rand() % 3;
        // 节点的bounding box包含传入的所有object的boundingbox
        bbox = aabb::empty;
        for(size_t i = start; i < end; ++i)
        {
            bbox = aabb(bbox, objects[i]->bounding_box());
        }
        // 找到最长的轴
        int axis = bbox.longest_axis();
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                        : box_z_compare;

        auto object_span = end - start;
        if(object_span == 1)
        {
            // 如果区间只剩下一个节点，那左右节点都指向这个节点
            left = right = objects[start];
        }
        else if (object_span == 2)
        {
            // 如果剩下两个，则左右一边一个
            left = objects[start];
            right = objects[start + 1];
        }
        else
        {
            // 若剩下更多，则根据一个随机的轴将剩余的object排序并分为两部分，继续往下分
            std::sort(objects.begin()+start, objects.begin()+end, comparator);
            auto mid = start + object_span / 2;
            left = make_shared<bvh_node>(objects, start, mid);
            right = make_shared<bvh_node>(objects, mid, end);
        }

        // bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bvh_node(hittable_list& list)
        : bvh_node(list.hit_objects, 0, list.hit_objects.size())
    {
        
    }

    bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override
    {
        if(!bbox.hit(r, ray_t))
        {
            return false;
        }

        bool hit_left = left->hit(r, ray_t, rec);
        // 这里的处理思路是，若左节点没有被击中，则右侧按照正常射线检测去做。若左节点被击中，则右侧需要得到比更近的结果才更新rec的记录，否则不更新
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left?rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override {return bbox;}
private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

    // 这里简单用在某个轴上aabb的最小的点作为判断依据
    static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index)
    {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 2);
    }
};

#endif