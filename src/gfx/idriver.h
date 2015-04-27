/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../common/color.h"
#include "mesh.h"
#include "texture.h"
namespace game
{
  namespace gfx
  {
    enum class Shader
    {
      Standard, Hud
    };

    struct IDriver
    {
      virtual ~IDriver() noexcept {}

      virtual void set_shader(Shader shade) noexcept = 0;

      virtual void prepare_mesh(Mesh& mesh) noexcept = 0;
      virtual void remove_mesh(Mesh& mesh) noexcept = 0;
      virtual void render_mesh(Mesh const& mesh) noexcept = 0;

      virtual void prepare_texture(Texture& tex) noexcept = 0;
      virtual void remove_texture(Texture& tex) noexcept = 0;
      virtual void bind_texture(Texture const&, unsigned int) noexcept=0;

      virtual void set_diffuse(Color const&) noexcept = 0;

      virtual void set_projection(glm::mat4 const&) noexcept = 0;
      virtual void set_view(glm::mat4 const&) noexcept = 0;
      virtual void set_model(glm::mat4 const&) noexcept = 0;

      virtual void clear_color_value(Color const& color) noexcept = 0;
      virtual void clear_depth_value(float val) noexcept = 0;

      virtual void clear() noexcept = 0;
      virtual void clear_color() noexcept = 0;
      virtual void clear_depth() noexcept = 0;
    };
  }
}
