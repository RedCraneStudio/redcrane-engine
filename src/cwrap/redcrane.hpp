/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * The engine's C interface for the engine to use
 */
#include <cstdint>
#include <memory>

#include <boost/variant.hpp>

#include "../minilua.h"

#include "../common/timer.hpp"
#include "../common/id_map.hpp"
#include "../common/peer_ptr.hpp"

#include "../use/mesh_cache.h"
#include "../use/texture.h"

#include "../gfx/idriver.h"
#include "../gfx/camera.h"
#include "../gfx/mesh_chunk.h"

#include "../fps/camera_controller.h"

#include "../sdl_helper.h"

#include <boost/filesystem.hpp>

#ifndef REDC_REDCRANE_DECL_H
#define REDC_REDCRANE_DECL_H
extern "C"
{
  #include "../redcrane_decl.h"
}
#endif

namespace redc
{
  template <class T>
  inline Peer_Lock<T> lock_resource(void* ptr)
  {
    auto peer = (Peer_Ptr<T>*) ptr;
    return peer->lock();
  }

  struct Scene;

  struct Engine
  {
    redc::SDL_Init_Lock sdl_raii;

    // Make sure we put this at the top so it is uninitialized relatively after
    // we have to deallocate all the meshes, textures, etc.
    // in the game.
    std::unique_ptr<gfx::IDriver> driver;
    std::unique_ptr<gfx::Mesh_Cache> mesh_cache;

    std::vector<Peer_Ptr<Scene> > scenes;

    // Resources
    std::vector<Peer_Ptr<Texture> > textures;
    std::vector<Peer_Ptr<gfx::Mesh_Chunk> > meshs;
    std::vector<Peer_Ptr<gfx::Shader> > shaders;

    boost::filesystem::path share_path;

    bool running = true;

    // TODO: Maybe keep track of every scene so lua doesn't have to deal with it
    // We should reserve some amount of memory for each scene so that we can
    // pass around pointers and no they won't suddenly become invalid.
  };

  struct Mesh_Object
  {
    Peer_Lock<gfx::Mesh_Chunk> chunk;
    glm::mat4 model;
  };

  struct Cam_Object
  {
    Cam_Object(gfx::Camera const& c) : cam(c), control() {}

    gfx::Camera cam;
    fps::Camera_Controller control;
  };

  struct Object
  {
    // This must match the order in the boost variant!!!
    enum Obj_Type : unsigned int
    {
      Mesh = 0,
      Cam = 1
    };

    boost::variant<Mesh_Object, Cam_Object> obj;

    // Non-owned parent, they should all be in the vector.
    Object* parent;
  };

  glm::mat4 object_model(Object const& obj);

  struct Scene
  {
    // This is an unordered map that also keeps an active camera available to
    // us.
    Engine* engine;

    // Should always be a Obj_Type::Cam!! Can we enforce that with the type
    // system?
    obj_id active_camera;

    ID_Gen<obj_id> index_gen;
    std::array<Object, std::numeric_limits<obj_id>::max()> objs;

#ifdef REDC_LOG_FRAMES
    int16_t frame_count = 0;
    Timer<> frame_timer;
#endif
  };
}