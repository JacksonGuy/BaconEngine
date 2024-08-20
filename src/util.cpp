#include "util.h"
#include "stb_image.h"

#include <iostream>

/**
 * @brief Loads file data
 * 
 * @param path path to file
 * @param dataSize size of the data buffer loaded 
 * @return u8* pointer to data buffer
 */
u8* B_LoadFileData(const char* path, i32* dataSize) {
    u8* data = NULL;
    *dataSize = 0;

    if (path != NULL) {
        FILE* file = fopen(path, "rb");
        if (file != NULL) {
            // Get file size
            fseek(file, 0, SEEK_END);
            i32 size = ftell(file);
            fseek(file, 0, SEEK_SET);

            // Allocate memory
            if (size > 0) {
                data = (u8*)malloc(size * sizeof(u8));
                if (data != NULL) {
                    size_t count = fread(data, sizeof(u8), size, file);
                    *dataSize = (i32)count;
                }
            }
        }
        fclose(file);
    }

    return data;
}

/**
 * @brief Loads an image without the raylib bullshit.
 * 
 * @param path the path to the file
 * @return Image the image
 */
Image B_LoadImage(std::string path) {
    Image image = {0};
    
    i32 dataSize = 0;
    u8* fileData = B_LoadFileData(path.c_str(), &dataSize);

    std::cout << "Image Size: " << dataSize << "\n";

    if (fileData != NULL) {
        i32 comp = 0;
        i32 width, height;
        image.data = stbi_load_from_memory(fileData, dataSize, &width, &height, &comp, 0);

        std::cout << "Image Size: " << width << "," << height << std::endl;
        std::cout << stbi_failure_reason() << std::endl;

        if (image.data != NULL) {
            image.mipmaps = 1;

            if (comp == 1) image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
            else if (comp == 2) image.format = PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
            else if (comp == 3) image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
            else if (comp == 4) image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        }

        UnloadFileData(fileData);
    }

    return image;
}