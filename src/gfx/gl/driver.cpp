/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "prep_mesh.h"
#include "prep_tex.h"
#include "../../common/software_texture.h"
#include "../../common/software_mesh.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      Driver::Driver() noexcept
      {
        current_shader_ = &standard_shader_;
        current_shader_->use();
      }
      Driver::~Driver() noexcept
      {
        glUseProgram(0);
      }
      void Driver::set_shader(Shader shade) noexcept
      {
        switch(shade)
        {
          case Shader::Standard:
          {
            current_shader_ = &standard_shader_;
          }
          case Shader::Hud:
          {
            current_shader_ = &hud_shader_;
          }
        }
        current_shader_->use();
      }
      std::unique_ptr<Mesh> Driver::make_mesh_repr() noexcept
      {
        // This is obviously temporary.
        return std::make_unique<Software_Mesh>();
      }
      void Driver::render_mesh(Mesh& mesh) noexcept
      {
      }

      std::shared_ptr<Texture> Driver::make_texture_repr() noexcept
      {
        return std::make_shared<Software_Texture>();
      }
      void Driver::bind_texture(Texture&, unsigned int) noexcept
      {
        // Don't do a damn thing just yet.
      }

      void Driver::set_diffuse(Color const& c) noexcept
      {
        current_shader_->set_diffuse(c);
      }

      void Driver::set_projection(glm::mat4 const& p) noexcept
      {
        current_shader_->set_projection(p);
      }
      void Driver::set_view(glm::mat4 const& v) noexcept
      {
        current_shader_->set_view(v);
      }
      void Driver::set_model(glm::mat4 const& m) noexcept
      {
        current_shader_->set_model(m);
      }
      void Driver::clear_color_value(Color const& c) noexcept
      {
        glClearColor(c.r / (float) 0xff, c.g / (float) 0xff,
                     c.b / (float) 0xff, 1.0);
      }
      void Driver::clear_depth_value(float f) noexcept
      {
        glClearDepth(f);
      }

      void Driver::clear() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
      void Driver::clear_color() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT);
      }
      void Driver::clear_depth() noexcept
      {
        glClear(GL_DEPTH_BUFFER_BIT);
      }
    }
  }
}
