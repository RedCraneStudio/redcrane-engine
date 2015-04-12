/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <vector>
#include "../common/aabb.h"
#include "../gfx/material.h"
#include "../gfx/mesh.h"
#include "../gfx/object.h"
#include "../gfx/idriver.h"
namespace strat
{ struct Terrain
  {
    // Let's use a tile based system, for now. The integer represents altitude.
    std::vector< std::vector<int> > altitude;

    int w;
    int h;
  };

  Terrain make_flat_terrain(int alt, int w, int h);
  Mesh make_terrain_mesh(Terrain const& t, double scale_fac = .01,
                         double flat_fac = 1) noexcept;

  enum class Orient
  {
    N,
    E,
    S,
    W
  };

  struct IStructure
  {
    virtual ~IStructure() noexcept {}

    virtual AABB aabb() const noexcept = 0;

    virtual gfx::Object const& obj() const noexcept = 0;
    virtual void prepare(gfx::IDriver&) noexcept = 0;

    virtual void set_model(glm::mat4 const&) noexcept = 0;
  };

  struct Structure_Instance
  {
    Structure_Instance(IStructure*, Orient) noexcept;
    Structure_Instance(Structure_Instance const&) noexcept;

    ~Structure_Instance() noexcept = default;

    const IStructure* structure_type;
    const Orient orientation;
  };
}
