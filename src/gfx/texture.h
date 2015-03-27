/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <cstdint>
namespace survive
{
  enum class Texture_Format
  {
    RGB, RGBA
  };

  struct Texture
  {
    int w, h;
    uint8_t* data;

    int pitch;

    Texture_Format format;

    Texture() noexcept = default;
    Texture(Texture const&) noexcept = delete;
    Texture(Texture&&) noexcept;
    ~Texture() noexcept;

    static Texture from_png_file(std::string filename) noexcept;
  };
}
