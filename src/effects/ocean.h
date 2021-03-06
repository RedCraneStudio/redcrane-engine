/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <chrono>
#include "../gfx/imesh.h"
#include "../use/effect.h"
#include "../common/plane.h"
namespace redc { namespace effects
{
  struct Ocean_Gen
  {
    int octaves;
    float amplitude;
    float frequency;
    float persistence;
    float lacunarity;
  };

  struct Ocean_Effect : public gfx::Effect
  {
    void init(gfx::IDriver& driver) noexcept;
    void render(gfx::IDriver& driver, gfx::Camera const& cam) noexcept;

    void set_ocean_gen_parameters(Ocean_Gen const& gen) noexcept;
    Ocean_Gen get_ocean_gen_parameters() const noexcept
    { return gen_params_; }

    float last_time_used_ = 0.0f;
    Plane water_base_;
  private:
    std::size_t elements_;

    std::unique_ptr<gfx::IBuffer> uv_buf_;
    std::unique_ptr<gfx::IMesh> grid_mesh_;
    std::unique_ptr<gfx::IShader> shader_;

    void update_ocean_gen_params() noexcept;

    float max_disp_ = 0.0f;
    bool needs_gen_params_update_ = false;
    Ocean_Gen gen_params_;

    std::chrono::high_resolution_clock::time_point start_;
  };
} }
