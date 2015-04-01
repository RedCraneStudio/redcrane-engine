/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include <cstdint>
namespace survive
{
  struct Color
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  inline bool operator==(Color const& c1, Color const& c2) noexcept
  {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
  }
  inline bool operator!=(Color const& c1, Color const& c2) noexcept
  {
    return !(c1 == c2);
  }
}
