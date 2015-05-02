/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "texture_load.h"
#include "log.h"
#include <png.h>
#include <cstring>
namespace game
{
  void error_fn(void*) noexcept {}

  void load_png(std::string filename, Texture& t) noexcept
  {
    std::FILE* fp = std::fopen(filename.c_str(), "rb");
    if(!fp)
    {
      log_w("Failed to open file '%'", filename);
      return;
    }

    constexpr int HEADER_READ = 7;
    unsigned char* const header = new unsigned char[HEADER_READ];
    fread(header, 1, HEADER_READ, fp);
    bool is_png = !png_sig_cmp(header, 0, HEADER_READ);
    if(!is_png)
    {
      log_w("File '%' not a png", filename);
      return;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL, NULL, NULL);
    if(!png_ptr)
    {
      log_w("Failed to initialize png struct for '%'", filename);
      return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      log_w("Failed to initialize png info for '%'", filename);
      return;
    }

    png_infop end_info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      log_w("Failed to initialize png end info for '%'", filename);
      return;
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
      fclose(fp);

      log_w("libpng error for '%'", filename);
      return;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, HEADER_READ);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_GRAY_TO_RGB, NULL);

    Vec<int> image_extents;
    image_extents.x = png_get_image_width(png_ptr, info_ptr);
    image_extents.y = png_get_image_height(png_ptr, info_ptr);
    t.allocate(image_extents);

    auto png_data = png_get_rows(png_ptr, info_ptr);
    auto format = png_get_color_type(png_ptr, info_ptr);

    int png_bytes_per_pixel = 0;
    if(format == PNG_COLOR_TYPE_RGB)
    {
      png_bytes_per_pixel = 3;
    }
    else if(format == PNG_COLOR_TYPE_RGBA)
    {
      png_bytes_per_pixel = 4;
    }

    for(int i = 0; i < image_extents.y; ++i)
    {
      auto row = new Color[image_extents.x];
      for(int j = 0; j < image_extents.x; ++j)
      {
        // Find where we are in png's data.
        auto dst_ptr = *(png_data + i);
        dst_ptr += j * png_bytes_per_pixel;

        row[j].r = dst_ptr[0];
        row[j].g = dst_ptr[1];
        row[j].b = dst_ptr[2];
        if(format == PNG_COLOR_TYPE_RGB)
        {
          // We don't have an alpha value, so fill in full opacity.
          row[j].a = 0xff;
        }
        else if(format == PNG_COLOR_TYPE_RGBA)
        {
          row[j].a = dst_ptr[3];
        }
      }
      // Blit row-by-row;
      t.blit_data({{0, i}, image_extents.x, 1}, row);
      delete[] row;
    }

    // We copied the data, so just forget about png now.
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    // Close the file of course.
    fclose(fp);
  }
}