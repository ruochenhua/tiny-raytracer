#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "color.h"

// return the color for a given scene ray
color ray_color(const ray& r)
{
    // lerp between two color
    vec3 unit_dir = unit_vector(r.direction());
    auto a = 0.5*(unit_dir.y() + 1.0);  // 从范围-1至1映射到范围0至1
    return (1.0-a)*color(1.0,1.0,1.0) + a*color(0.5, 0.7, 1.0);
    // return color(0,0,0);
}

int main() 
{
    FILE* imageFile;
    float aspectRatio = 16.0f / 9.0f;
    // image size
    int imageWidth = 480;
    int imageHeight = int(imageWidth / aspectRatio);
    // must bigger than 1
    imageHeight = (imageHeight < 1) ? 1 : imageHeight;

    auto viewportHeight = 2.f;
    // aspect_ratio may vary depends on the real image height
    auto viewportWidth = viewportHeight * double((imageWidth)/imageHeight);
    

    // camera
    auto focal_length = 1.0;
    auto camera_center = point3(0,0,0);
    // 3D场景中的坐标轴是Y朝上，X朝右，Z朝向平面里面
    // 屏幕坐标轴是X朝右(u)，Y朝下(v)
    auto viewport_u = vec3(viewportWidth, 0, 0);
    auto viewport_v = vec3(0, -viewportHeight, 0);
    // 每个pixel的大小
    auto pixel_delta_u = viewport_u / imageWidth;
    auto pixel_delta_v = viewport_v / imageHeight;
    // viewport的左上角是以相机为原点，偏移视口大小一半以及焦距的距离
    auto viewport_upper_left = camera_center - vec3(0,0,focal_length) - viewport_u/2 - viewport_v/2;
    // 第一个pixel的位置(中心点)是viewport左上角偏移半个像素大小的位置
    auto pixel_origin_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);


    // error_t file_err;
    imageFile = fopen("test_image.ppm", "wb");

    if (imageFile == NULL) {
        perror("ERROR: Cannot open output file");
        exit(EXIT_FAILURE);
    }

    fprintf(imageFile, "P3\n");           // P3 filetype
    fprintf(imageFile, "%d %d\n", imageWidth, imageHeight);   // dimensions
    fprintf(imageFile, "255\n");          // Max pixel

    // Image
    for (int j = 0; j < imageHeight; j++) {
        std::clog << "\r Scanlines remaining: " << (imageHeight - j) << ' ' << std::flush;

        for (int i = 0; i < imageWidth; i++) {
            // auto pixelColor = color(double(i) / (imageWidth - 1), double(j) / (imageHeight - 1), 0.0);
            // 定位每个像素位置
            auto pixel_center = pixel_origin_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
            // 计算像素的方向，构建射线
            auto ray_dir = pixel_center - camera_center;
            ray r(camera_center, ray_dir);

            // 核心
            color pixel_color = ray_color(r);
            WriteColorToFile(imageFile, pixel_color);            
        }
    }

    std::clog << "\r Done.                                                   \n";

    fclose(imageFile);
}