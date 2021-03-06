/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * Mesh stuff
 */

#include "redcrane.hpp"

#include "../use/mesh.h"

using namespace redc;

extern "C"
{
  // See mesh_pool.lua
  void *redc_load_mesh(void *engine, const char *str)
  {
    // TODO: Load many static objects into the same mesh.
    auto rce = (Engine *) engine;
    REDC_ASSERT_HAS_CLIENT(rce);

    gfx::Mesh_Load_Params load_params;
    load_params.filename = std::string{str};
    load_params.retain_mesh = false;
    gfx::Mesh_Result mesh = gfx::load_mesh(*rce->client->driver,
                                           *rce->mesh_cache, load_params);

    // Lua is one peer
    auto peer = new Peer_Ptr<gfx::Mesh_Chunk>(new gfx::Mesh_Chunk);

    // Move our loaded mesh into it
    *peer->get() = copy_mesh_chunk_move_mesh(mesh.chunk);

    // The engine is the other.
    rce->client->peers.push_back(peer->peer());

    return peer;
  }
  void redc_unload_mesh(void *mesh)
  {
    // This will deallocate the mesh in the engine if it hasn't been deallocated
    // already.
    auto peer = (Peer_Ptr<gfx::Mesh_Chunk>*) mesh;
    delete peer;
  }
}
