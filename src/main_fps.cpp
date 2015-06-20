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

#include <btBulletDynamicsCommon.h>

#include "common/log.h"

#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "gfx/mesh_chunk.h"
#include "gfx/support/load_wavefront.h"
#include "gfx/support/mesh_conversion.h"
#include "gfx/support/generate_aabb.h"
#include "gfx/support/write_data_to_mesh.h"
#include "gfx/support/texture_load.h"

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

struct House_Kinematic_Motion : public btMotionState
{
  void getWorldTransform(btTransform &trans) const override;
  void setWorldTransform(btTransform const& trans) override;

  void move_left();
  void move_right();
private:
  btVector3 pos_;
};

void House_Kinematic_Motion::getWorldTransform(btTransform &trans) const
{
  trans.setOrigin(pos_);
}
void House_Kinematic_Motion::setWorldTransform(btTransform const& trans)
{
  pos_ = trans.getOrigin();
}

void House_Kinematic_Motion::move_left()
{
  pos_.setX(pos_.getX() - .01);
}
void House_Kinematic_Motion::move_right()
{
  pos_.setX(pos_.getX() + .01);
}

struct Command_Options
{
};

Command_Options parse_command_line(int argc, char**)
{
  Command_Options opt;
  for(int i = 0; i < argc; ++i)
  {
    //auto option = argv[i];
  }
  return opt;
}

int main(int argc, char** argv)
{
  using namespace game;

  set_log_level(Log_Severity::Debug);

  uv_chdir("assets/");

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

  // Parse command line arguments.
  auto options = parse_command_line(argc - 1, argv+1);

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

  // Initialize bullet

  btDbvtBroadphase bt_broadphase;

  btDefaultCollisionConfiguration bt_configuration;
  btCollisionDispatcher bt_collision_dispatcher(&bt_configuration);

  btSequentialImpulseConstraintSolver bt_constraint_solver;
  btDiscreteDynamicsWorld bt_world(&bt_collision_dispatcher,
                                   &bt_broadphase,
                                   &bt_constraint_solver,
                                   &bt_configuration);

  bt_world.setGravity(btVector3(0, -9.81, 0));

  btStaticPlaneShape bt_plane_shape(btVector3(0, 1, 0), -1);
  btDefaultMotionState bt_motion_state;
  btRigidBody bt_plane(0, &bt_motion_state, &bt_plane_shape);

  bt_world.addCollisionObject(&bt_plane);

  // Hide the mouse and capture it
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // Make an fps camera.
    auto cam = gfx::make_fps_camera();
    cam.fp.pos = glm::vec3(0.0f, 0.0f, 15.0f);

    auto cam_controller = fps::Camera_Controller{};
    cam_controller.camera(cam);

    cam_controller.set_yaw_limit(PI / 2);
    cam_controller.set_pitch_limit(PI / 2);

    Maybe_Owned<Mesh> house = driver.make_mesh_repr();
    auto house_data =
               gfx::to_indexed_mesh_data(gfx::load_wavefront("obj/house.obj"));
    gfx::allocate_mesh_buffers(house_data, *house);
    // TODO: We should be moving here, but we can't because that would require
    // allocating a mesh, but maybe_owned doesn't know the implementation.
    // Moving here would ideally change our house to point to the ptr in the
    // mesh chunk, which now owns the mesh.
    auto house_chunk = gfx::write_data_to_mesh(house_data, std::move(house));
    auto house_model = glm::mat4(1.0f);
    gfx::format_mesh_buffers(*house);

    auto house_aabb = gfx::generate_aabb(house_data);

    btBoxShape bt_house_shape(btVector3(house_aabb.width / 2.f,
                                        house_aabb.height / 2.f,
                                        house_aabb.depth / 2.f));
    House_Kinematic_Motion bt_house_motion_state;
    btRigidBody bt_house_rigidbody(100,&bt_house_motion_state,&bt_house_shape);
    bt_house_rigidbody.setCollisionFlags(
                       btCollisionObject::CF_KINEMATIC_OBJECT);

    btTransform house_transform(btMatrix3x3::getIdentity(),
                                btVector3(0, 0, 0));
    bt_house_motion_state.setWorldTransform(house_transform);

    bt_world.addRigidBody(&bt_house_rigidbody);

    Maybe_Owned<Mesh> plane = driver.make_mesh_repr();
    auto plane_data =
      gfx::to_indexed_mesh_data(gfx::load_wavefront("obj/plane.obj"));

    gfx::allocate_mesh_buffers(plane_data, *plane);
    auto plane_chunk = gfx::write_data_to_mesh(plane_data, std::move(plane));
    gfx::format_mesh_buffers(*plane);

    auto plane_model = glm::mat4(1.0f);
    plane_model = glm::scale(plane_model, glm::vec3(5.0f, 1.0f, 7.0f));
    plane_model = glm::translate(plane_model, glm::vec3(0.0f,-1.0f,0.0f));
    plane_model = glm::translate(plane_model, glm::vec3(-0.5f, 0.0f, -0.5f));

    Maybe_Owned<Mesh> sphere = driver.make_mesh_repr();
    auto sphere_data =
              gfx::to_indexed_mesh_data(gfx::load_wavefront("obj/sphere.obj"));

    gfx::allocate_mesh_buffers(sphere_data, *sphere);
    auto sphere_chunk =gfx::write_data_to_mesh(sphere_data,std::move(sphere));
    auto sphere_model = glm::mat4(1.0f);
    gfx::format_mesh_buffers(*sphere);

    btDefaultMotionState default_motion;
    btCapsuleShape sphere_shape(1.0f, 1.0f);
    btRigidBody sphere_body(50, &default_motion, &sphere_shape);

    btTransform sphere_initial;
    sphere_initial.setOrigin(btVector3(0.0f, 5.0f, 0.0f));
    sphere_body.setWorldTransform(sphere_initial);

    bt_world.addRigidBody(&sphere_body);

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

    using time_point_t =
                   std::chrono::time_point<std::chrono::high_resolution_clock>;
    time_point_t before = time_point_t::clock::now();

    while(!glfwWindowShouldClose(window))
    {
      ++fps;

      if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      {
        bt_house_motion_state.move_left();
      }
      if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      {
        bt_house_motion_state.move_right();
      }

      auto now = time_point_t::clock::now();
      auto diff = now - before;
      before = now;

      using std::chrono::duration_cast;
      auto ms = duration_cast<std::chrono::milliseconds>(diff).count();

      bt_world.stepSimulation(ms / 1000.0f, 6);

      glfwPollEvents();

      if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      {
        glfwSetWindowShouldClose(window, true);
      }

      double x, y;
      glfwGetCursorPos(window, &x, &y);
      cam_controller.apply_delta_pitch(y / 250.0 - prev_y / 250.0);
      cam_controller.apply_delta_yaw(x / 250.0 - prev_x / 250.0);
      prev_x = x, prev_y = y;

      use_camera(driver, cam);

      // Clear the screen
      driver.clear();

      btTransform transform;
      bt_house_motion_state.getWorldTransform(transform);

      auto orig = transform.getOrigin();
      house_model = glm::translate(glm::mat4(1.0f),
                                   glm::vec3(orig.x(), orig.y(), orig.z()));

      auto y_delta_for_now = house_aabb.min.y - house_aabb.height / 2;

      house_model = glm::translate(house_model,
              glm::vec3(0.f, -house_aabb.height / 2.f - house_aabb.min.y,0.f));

      shader->set_diffuse(colors::white);

      driver.bind_texture(*grass, 0);

      shader->set_model(plane_model);
      gfx::render_chunk(plane_chunk);

      driver.bind_texture(*brick, 0);

      shader->set_model(house_model);
      gfx::render_chunk(house_chunk);

      btTransform sphere_trans;
      default_motion.getWorldTransform(sphere_trans);
      sphere_trans.getOpenGLMatrix(&sphere_model[0][0]);

      shader->set_model(sphere_model);
      shader->set_diffuse(colors::red);
      gfx::render_chunk(sphere_chunk);

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
