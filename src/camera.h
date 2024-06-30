#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "rt_common.h"
#include "hittable.h"
#include "material.h"

class camera
{
public:
    float aspect_ratio = 16.0f / 9.0f;
    int image_width = 480;
    int samples_per_pixel = 10;     //每个像素做多次采样
    int max_depth = 10;     // 射线检测递归深度

    double vfov = 75;       // 垂直fov的大小
    point3 lookfrom = point3(0,0,0);    // 相机位置
    point3 lookat = point3(0,0,-1);         // 看向什么地方
    vec3 vup = vec3(0,1,0);                 // up向量

    double defocus_angle = 0;   //虚化角度
    double focus_dist = 10;     //对焦距离
    void render(const hittable& world)
    {
        initialize();
        FILE* imageFile = fopen("test_image.ppm", "wb");

        if (imageFile == NULL) {
            perror("ERROR: Cannot open output file");
            exit(EXIT_FAILURE);
        }

        fprintf(imageFile, "P3\n");           // P3 filetype
        fprintf(imageFile, "%d %d\n", image_width, image_height);   // dimensions
        fprintf(imageFile, "255\n");          // Max pixel

        // Image
        for (int j = 0; j < image_height; j++) {
            std::clog << "\r Scanlines remaining: " << (image_height - j) << ' ' << std::flush;

            for (int i = 0; i < image_width; i++) {
                // auto pixelColor = color(double(i) / (imageWidth - 1), double(j) / (imageHeight - 1), 0.0);
                // 定位每个像素位置
                color pixel_color(0,0,0);
                for(int sample = 0; sample <samples_per_pixel; ++sample)
                {
                    ray r = get_ray(i, j);
                
                    pixel_color += ray_color(r, max_depth, world);
                }
                // auto pixel_center = pixel_origin_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
                // // 计算像素的方向，构建射线
                // auto ray_dir = pixel_center - camera_center;
                // ray r(camera_center, ray_dir);

                // // 核心
                // color pixel_color = ray_color(r, world);
                WriteColorToFile(imageFile, pixel_samples_scale * pixel_color);            
            }
        }
        
        std::clog << "\r Done.                                                   \n";

        fclose(imageFile);
    }
    
private:
    int image_height;
    FILE* image_file;
    point3 camera_center;
    point3 pixel_origin_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    //要做多次采样，所以每次采样会乘以一个系数，使得总采样数加起来的颜色是在正常范围
    double pixel_samples_scale;
    vec3 u,v,w;
    vec3 defocus_disk_u;        // 失焦盘的水平半径
    vec3 defocus_disk_v;        // 失焦盘垂直半径
    void initialize()
    {
        image_height = int(image_width) / aspect_ratio;
        // must bigger than 1
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;
        camera_center = lookfrom;
        
        // auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewportHeight = 2.0 * h * focus_dist;
        // aspect_ratio may vary depends on the real image height
        auto viewportWidth = viewportHeight * (double(image_width)/image_height);

        w = unit_vector(lookfrom - lookat);  // w是向前方向
        u = unit_vector(cross(vup, w));         // u是向右方向
        v = cross(w, u);                          // v是向上方向
        
        // 3D场景中的坐标轴是Y朝上，X朝右，Z朝向平面里面
        // 屏幕坐标轴是X朝右(u)，Y朝下(v)
        // auto viewport_u = vec3(viewportWidth, 0, 0);
        // auto viewport_v = vec3(0, -viewportHeight, 0);
        auto viewport_u = viewportWidth * u;
        auto viewport_v = -viewportHeight * v;
        
        // 每个pixel的大小
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        // viewport的左上角是以相机为原点，偏移视口大小一半以及焦距的距离
        auto viewport_upper_left = camera_center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        // 第一个pixel的位置(中心点)是viewport左上角偏移半个像素大小的位置
        pixel_origin_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle/2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    vec3 sample_square() const
    {
        // 范围在[-0.5, 0.5)
        return vec3(random_double()-0.5, random_double()-0.5, 0);
    }

    ray get_ray(int i, int j) const
    {
        auto offset = sample_square();
        auto pixel_sample = pixel_origin_loc + ((i+offset.x()) * pixel_delta_u) + ((j+offset.y()) * pixel_delta_v);
        // auto ray_origin = camera_center;
        auto ray_origin = (defocus_angle <=0 ) ? camera_center : defocus_disk_sample();
        auto ray_dir = pixel_sample - ray_origin;

        return ray(ray_origin, ray_dir);
    }


    color ray_color(const ray& r, int depth, const hittable& world)
    {
        if(depth < 0)
        {
            return color(0,0,0);
        }
        hit_record rec;
        // interval从0.001开始，避免ray反弹的时候起始点取值取到表面内部
        if(world.hit(r, interval(0.001, infinity), rec))
        {
            ray scattered;
            color attenuation;
            // 物体反射的颜色将会根据材质不同而不同
            if(rec.mat->scatter(r, rec, attenuation, scattered))
            {
                return attenuation*ray_color(scattered, depth-1, world);
            }
            // vec3 direction = random_on_hemisphere(rec.normal);
            // vec3 direction = rec.normal + random_unit_vector(); 
            return color(0,0,0);
        }

        // lerp between two color
        vec3 unit_dir = unit_vector(r.direction());
        auto a = 0.5*(unit_dir.y() + 1.0);  // 从范围-1至1映射到范围0至1
        return (1.0-a)*color(1.0,1.0,1.0) + a*color(0.5, 0.7, 1.0);    
    }

    point3 defocus_disk_sample() const
    {
        auto p = random_in_unit_disk();
        return camera_center + (p[0]*defocus_disk_u) + p[1]*defocus_disk_v;
    }
};

#endif