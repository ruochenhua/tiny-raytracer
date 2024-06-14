#include <iostream>
#include "vec3.h"
#include "color.h"

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
            auto pixelColor = color(double(i) / (imageWidth - 1), double(j) / (imageHeight - 1), 0.0);
            WriteColorToFile(imageFile, pixelColor);            
        }
    }

    std::clog << "\r Done.                                                   \n";

    fclose(imageFile);
}