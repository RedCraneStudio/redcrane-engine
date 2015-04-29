/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "software_texture.h"
namespace game
{
  Software_Texture::~Software_Texture() noexcept
  {
    delete[] data_;
  }
  Color Software_Texture::get_pt(Vec<int> pt) const noexcept
  {
    return data_[pt.y * allocated_extents().x + pt.x];
  }
  Color* Software_Texture::get_row(int row) const noexcept
  {
    return data_ + row * allocated_extents().x;
  }
  Color* Software_Texture::get_data() const noexcept
  {
    return data_;
  }
  void Software_Texture::allocate_(Vec<int> const& extents) noexcept
  {
    data_ = new Color[extents.x * extents.y];
  }
  void Software_Texture::blit_data_(Volume<int> const& vol,
                                    Color const* cd) noexcept
  {
    auto extents = vol_extents(vol);
    for(int i = 0; i < extents.y; ++i)
    {
      for(int j = 0; j < extents.x; ++j)
      {
        data_[(vol.pos.y + i) * allocated_extents().x + vol.pos.x + j] =
          cd[i * extents.x + j];
      }
    }
  }
}
