/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * General engine stuff
 */

#include "redcrane.hpp"

#include "../assets/load_dir.h"
#include "../gfx/gl/driver.h"

using namespace redc;

namespace
{
  void redc_lua_log(Log_Severity s, const char *msg)
  {
    redc::log(s, "(Mod) %", msg);
  }
}

extern "C"
{
  // See redcrane.lua

  void redc_log_d(const char* str)
  {
    redc_lua_log(Log_Severity::Debug, str);
  }
  void redc_log_i(const char* str)
  {
    redc_lua_log(Log_Severity::Info, str);
  }
  void redc_log_w(const char* str)
  {
    redc_lua_log(Log_Severity::Warning, str);
  }
  void redc_log_e(const char* str)
  {
    redc_lua_log(Log_Severity::Error, str);
  }

  void* redc_init_engine(Redc_Config cfg)
  {
    // Switch into the current working directory requested by the mod
    if(!exists(boost::filesystem::path(cfg.cwd)))
    {
      log_e("Failed to enter requested '%' directory", cfg.cwd);
      return nullptr;
    }

    auto eng = new Engine{cfg, assets::share_path(), true, nullptr};
    log_i("Initialized the Red Crane Engine alpha version 0.0 (Mod: %)",
          cfg.mod_name);

    return eng;
  }
  void redc_init_client(void* eng)
  {
    auto rce = (Engine*) eng;

    auto sdl_init_raii_lock =
            redc::init_sdl(rce->config.mod_name, {1000,1000}, false, false);
    auto sdl_window = sdl_init_raii_lock.window;

    rce->client = std::make_unique<Client>(std::move(sdl_init_raii_lock));

    SDL_SetRelativeMouseMode(SDL_TRUE);

    int x, y;
    SDL_GetWindowSize(sdl_window, &x, &y);

    rce->client->driver = std::make_unique<gfx::gl::Driver>(Vec<int>{x,y});

    rce->client->mesh_cache =
            std::make_unique<gfx::Mesh_Cache>(rce->share_path / "obj",
                                              rce->share_path / "obj_cache");

    // Load default shader, etc.
    auto df_shade = rce->client->driver->make_shader_repr();

    // TODO: Load shaders like we load mesh. Right now is bad
    auto basic_shade_path = rce->share_path / "shader" / "basic";
    df_shade->load_vertex_part((basic_shade_path / "vs.glsl").native());
    df_shade->load_fragment_part((basic_shade_path / "fs.glsl").native());

    df_shade->set_model_name("model");
    df_shade->set_view_name("view");
    df_shade->set_projection_name("proj");

    // Make it the default
    rce->client->driver->use_shader(*df_shade);

    // Make sure we don't delete it later by linking its lifetime with that of
    // the engines.
    rce->client->shaders.push_back(std::move(df_shade));
  }
  void redc_uninit_engine(void* eng)
  {
    auto rce = (Engine*) eng;
    delete rce;
  }
  const char* redc_get_asset_path(void* eng)
  {
    // Will this ever change mid-execution?
    auto rce = (Engine*) eng;
    return rce->share_path.native().c_str();
  }
  void redc_window_swap(void* eng)
  {
    auto engine = (Engine*) eng;
    if(engine->client)
    {
      SDL_GL_SwapWindow(engine->client->sdl_raii.window);
    }
  }

  void redc_gc(void* eng)
  {
    auto rce = (Engine*) eng;

    auto is_null = [](auto const& peer)
    {
      return peer.get() == nullptr;
    };

    // Go through the vector of peer pointers and removed deallocated ones.
    using std::begin; using std::end;

    if(rce->client)
    {
      auto& client = *rce->client;

      auto mesh_end = std::remove_if(begin(client.meshs), end(client.meshs),
                                     is_null);
      client.meshs.erase(mesh_end, end(client.meshs));

      auto textures_end = std::remove_if(begin(client.textures),
                                         end(client.textures), is_null);
      client.textures.erase(textures_end, end(client.textures));

      auto shaders_end = std::remove_if(begin(client.shaders),
                                        end(client.shaders), is_null);
      client.shaders.erase(shaders_end, end(client.shaders));
    }
  }
}
