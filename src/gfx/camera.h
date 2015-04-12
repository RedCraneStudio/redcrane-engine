/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace strat
{
  namespace gfx
  {
    enum class Camera_Type
    {
      Perspective, Orthographic
    };

    struct Perspective_Cam_Params
    {
      float fov, aspect, near, far;
    };

    struct Ortho_Cam_Params
    {
      float left, right, bottom, top, near, far;
    };

    struct Camera
    {
      // A camera also includes a perspective and orthographic mode.
      Camera_Type projection_mode;
      union
      {
        Perspective_Cam_Params perspective;
        Ortho_Cam_Params ortho;
      };

      // Camera position, orientation and lookpos.
      glm::vec3 eye;
      glm::vec3 look;
      glm::vec3 up;
    };

    glm::mat4 camera_view_matrix(Camera const& cam) noexcept;
    glm::mat4 camera_proj_matrix(Camera const& cam) noexcept;

    Camera make_isometric_camera() noexcept;
  }
}