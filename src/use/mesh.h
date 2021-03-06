/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <boost/optional.hpp>
#include "../gfx/idriver.h"
#include "../gfx/mesh_chunk.h"
#include "../gfx/mesh_data.h"
namespace redc { namespace gfx
{
  struct Mesh_Load_Params
  {
    std::string filename;

    bool retain_mesh = false;
  };

  struct Mesh_Result
  {
    Mesh_Chunk chunk;
    boost::optional<Indexed_Mesh_Data> data;
  };

  struct Mesh_Cache;

  Mesh_Result load_mesh(IDriver&, Mesh_Load_Params const&);
  Mesh_Result load_mesh(IDriver&, Mesh_Cache& mc, Mesh_Load_Params const&);

  Mesh_Chunk load_chunk(IDriver& driver, Mesh_Cache& mc, std::string const& f);
} }
