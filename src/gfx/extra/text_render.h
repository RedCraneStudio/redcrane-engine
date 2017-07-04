/*
 * Copyright (c) 2017 Luke San Antonio
 * All rights reserved.
 */

#ifndef RED_CRANE_ENGINE_TEXT_RENDER_H
#define RED_CRANE_ENGINE_TEXT_RENDER_H

#include <string>

#include <glm/glm.hpp>

#include "../idriver.h"

#include "freetype-gl/texture-atlas.h"
#include "freetype-gl/texture-font.h"
#include "freetype-gl/text-buffer.h"

namespace redc
{
  enum class Reference_Point
  {
    Top_Center,
    Bottom_Center,
    Left_Center,
    Right_Center,
    Center,
    Top_Left,
    Top_Right,
    Bottom_Left,
    Bottom_Right
  };

  struct Text_Render_Ctx
  {
    explicit Text_Render_Ctx(std::string font_path);
    ~Text_Render_Ctx();

    void render_text(gfx::IDriver& driver, std::string text, glm::vec2 pt,
                     Reference_Point ref_pt);
  private:
    std::unique_ptr<gfx::IShader> shader_;

    ftgl::texture_atlas_t *atlas_;
    std::unique_ptr<gfx::ITexture> atlas_tex_;

    ftgl::texture_font_t *font_;
  };
}

#endif
