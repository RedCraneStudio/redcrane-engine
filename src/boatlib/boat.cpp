/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "boat.h"
namespace redc
{
  Hull_Desc build_hull_desc(gfx::Mesh_Cache& m_cache, std::string name,
                            std::string model_name,
                            Hull_Attachment_Def attachments) noexcept
  {
    Hull_Desc ret;

    ret.name = name;
    ret.mesh = m_cache.load_owned_chunk(model_name);

    ret.attachments = attachments;

    return ret;
  }

  template <class T>
  inline T build_object_desc(gfx::Mesh_Cache& mc, std::string name,
                             std::string model_name) noexcept
  {
    return {name, mc.load_owned_chunk(model_name)};
  }
  Sail_Desc build_sail_desc(gfx::Mesh_Cache& mc, std::string name,
                            std::string model_name) noexcept
  { return build_object_desc<Sail_Desc>(mc, name, model_name); }

  Rudder_Desc build_rudder_desc(gfx::Mesh_Cache& mc, std::string name,
                                std::string model_name) noexcept
  { return build_object_desc<Rudder_Desc>(mc, name, model_name); }

  Gun_Desc build_gun_desc(gfx::Mesh_Cache& mc, std::string name,
                          std::string model_name) noexcept
  { return build_object_desc<Gun_Desc>(mc, name, model_name); }

  Boat_Render_Config build_boat_render_config(Boat_Config& boat) noexcept
  {
    Boat_Render_Config ret;

    ret.attachments = boat.hull->attachments;

    ret.hull = copy_mesh_chunk_share_mesh(boat.hull->mesh);
    ret.sail = copy_mesh_chunk_share_mesh(boat.sail->mesh);
    ret.rudder = copy_mesh_chunk_share_mesh(boat.rudder->mesh);
    ret.gun = copy_mesh_chunk_share_mesh(boat.gun->mesh);

    ret.model = glm::mat4(1.0f);

    return ret;
  }
}