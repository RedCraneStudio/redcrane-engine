/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <thread>
#include <chrono>

#include "common/log.h"

#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "gfx/mesh_chunk.h"
#include "gfx/support/load_wavefront.h"
#include "gfx/support/mesh_conversion.h"
#include "gfx/support/generate_aabb.h"
#include "gfx/support/write_data_to_mesh.h"
#include "gfx/support/texture_load.h"
#include "gfx/support/format.h"
#include "gfx/support/allocate.h"
#include "gfx/support/json.h"

#include "collisionlib/triangle_conversion.h"
#include "collisionlib/triangle.h"

#include "common/json.h"

#include "fps/camera_controller.h"

#include "glad/glad.h"
#include "glfw3.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "uv.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

#define PI 3.141592653589793238463

int main(int argc, char** argv)
{
  using namespace game;

  set_log_level(Log_Severity::Debug);

  uv_chdir("assets/");

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

  // Init glfw.
  if(!glfwInit())
    return EXIT_FAILURE;

  auto window = glfwCreateWindow(1000, 1000, "Hello World", NULL, NULL);
  if(!window)
  {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Init context + load gl functions.
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  // Log glfw version.
  log_i("Initialized GLFW %", glfwGetVersionString());

  int maj = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
  int min = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
  int rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);

  // Log GL profile.
  log_i("OpenGL core profile %.%.%", maj, min, rev);

  // Hide the mouse and capture it
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  auto scene = load_json("scene/fps.json");

  auto player_position = vec3_from_js(scene["player_pos"]);

  {
    // Make an OpenGL driver.
    gfx::gl::Driver driver{Vec<int>{1000, 1000}};

    auto shader = driver.make_shader_repr();
    shader->load_vertex_part("shader/basic/v");
    shader->load_fragment_part("shader/basic/f");

    // We need to get rid of dependency on these in the future.
    shader->set_diffuse_name("dif");
    shader->set_projection_name("proj");
    shader->set_view_name("view");
    shader->set_model_name("model");
    shader->set_sampler_name("tex");

    driver.use_shader(*shader);

    shader->set_sampler(0);

    // Get all our textures
    auto brick = driver.make_texture_repr();
    load_png("tex/cracked_soil.png", *brick);
    auto grass = driver.make_texture_repr();
    load_png("tex/grass.png", *grass);

    auto terrain_tex = driver.make_texture_repr();
    load_png("tex/topdown_terrain.png", *terrain_tex);

    // Make an fps camera.
    auto cam = gfx::make_fps_camera();
    cam.fp.pos = player_position;

    auto cam_controller = fps::Camera_Controller{};
    cam_controller.camera(cam);

    cam_controller.set_pitch_limit(PI / 2);

    // TODO put the house on the scene at a fixed location.
    Maybe_Owned<Mesh> terrain = driver.make_mesh_repr();
    auto terrain_data =
      gfx::to_indexed_mesh_data(gfx::load_wavefront("obj/fps_terrain.obj"));

    gfx::allocate_standard_mesh_buffers(terrain_data.vertices.size(),
                                        terrain_data.elements.size(),
                                        *terrain, Usage_Hint::Draw,
                                        Upload_Hint::Static);
    auto ter_chunk = gfx::write_data_to_mesh(terrain_data, std::move(terrain));
    gfx::format_standard_mesh_buffers(*terrain);

    auto triangles = triangles_from_mesh_data(terrain_data);

    auto terrain_model = glm::mat4(1.0f);

    int fps = 0;
    int time = glfwGetTime();

    // Set up some pre-rendering state.
    driver.clear_color_value(Color{0x55, 0x66, 0x77});
    driver.clear_depth_value(1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    double prev_x, prev_y;
    glfwPollEvents();
    glfwGetCursorPos(window, &prev_x, &prev_y);

    shader->set_diffuse(colors::white);
    driver.bind_texture(*terrain_tex, 0);
    shader->set_model(terrain_model);

    while(!glfwWindowShouldClose(window))
    {
      ++fps;

      glfwPollEvents();

      if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      {
        glfwSetWindowShouldClose(window, true);
      }

      glm::vec4 delta_movement;
      if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      {
        delta_movement.z -= 1.0f;
      }
      if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      {
        delta_movement.x -= 1.0f;
      }
      if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      {
        delta_movement.z += 1.0f;
      }
      if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      {
        delta_movement.x += 1.0f;
      }

      delta_movement *= .005;

      delta_movement = delta_movement *
        glm::rotate(glm::mat4(1.0f), cam.fp.yaw, glm::vec3(0.0f, 1.0f, 0.0f));

      double x, y;
      glfwGetCursorPos(window, &x, &y);
      cam_controller.apply_delta_pitch(y / 250.0 - prev_y / 250.0);
      cam_controller.apply_delta_yaw(x / 250.0 - prev_x / 250.0);
      prev_x = x, prev_y = y;

      cam.fp.pos.x += delta_movement.x;
      cam.fp.pos.z += delta_movement.z;

      // Find the height at cam.fp.pos.
      for(auto const& triangle : triangles)
      {
        if(is_contained_xz(glm::vec2(cam.fp.pos.x, cam.fp.pos.z),
                           triangle))
        {
          auto bary = to_barycentric_coord(triangle, cam.fp.pos);
          cam.fp.pos.y = triangle.positions[0].y * bary.x +
                         triangle.positions[1].y * bary.y +
                         triangle.positions[2].y * bary.z + 1.0f;
        }
      }

      use_camera(driver, cam);

      // Clear the screen
      driver.clear();

      gfx::render_chunk(ter_chunk);

      glfwSwapBuffers(window);

      if(int(glfwGetTime()) != time)
      {
        time = glfwGetTime();
        log_d("fps: %", fps);
        fps = 0;
      }
    }
  }
  glfwTerminate();
  return 0;
}
