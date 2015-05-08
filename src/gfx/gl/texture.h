/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
#include "../../common/texture.h"
namespace game { namespace gfx { namespace gl
{
  struct GL_Texture : public Texture
  {
    void allocate_(Vec<int> const& extents) noexcept override;
    void blit_data_(Volume<int> const& vol,
                    Color const* data) noexcept override;

    GLuint tex_id;
    GLenum texture_type;

    void bind(unsigned int loc) const noexcept;
  };
} } }