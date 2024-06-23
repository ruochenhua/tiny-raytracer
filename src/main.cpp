#include <iostream>
#include "rt_common.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

// double hit_sphere(const point3& center, double radius, const ray& r)
// {
//     vec3 oc = center - r.origin();
//     // auto a = dot(r.direction(), r.direction());
//     // auto b = -2.0*dot(r.direction(), oc);
//     // auto c = dot(oc, oc) - radius*radius;
//     // auto discriminant = b*b - 4*a*c;    // 二次方程有无解就是看这个式子能否开根号，若小于0则开不出根号没有实数解
//     auto a = r.direction().length_squared();
//     auto h = dot(r.direction(), oc);
//     auto c = oc.length_squared() - radius*radius;
//     auto discriminant = h*h - a*c;
//
//     if(discriminant < 0)
//     {
//         return -1.0;
//     }
//     else
//     {
//         // 这里返回 -b-discriminant的这个解，因为这里是离射线更近的相交点，更远的相交点被挡住了看不见
//         // return (-b-sqrt(discriminant)) / (2.0*a);
//         return (h - sqrt(discriminant)) / a;
//     }
// }

int main() 
{
    // float aspectRatio = 16.0f / 9.0f;
    // // image size
    // int imageWidth = 480;
    // int imageHeight = int(imageWidth / aspectRatio);
    // // must bigger than 1
    // imageHeight = (imageHeight < 1) ? 1 : imageHeight;

    // world hit object
    hittable_list world;

    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    // camera
    camera cam;
    cam.render(world);
    return 0;
}