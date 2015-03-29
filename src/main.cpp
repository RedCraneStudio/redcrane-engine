/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

#include "common/log.h"

#include "gfx/mesh.h"
#include "gfx/program.h"
#include "gfx/pipeline.h"
#include "gfx/texture.h"

#include "glad/glad.h"
#include "glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "uv.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

struct Command_Options
{
  bool test = false;
};

Command_Options parse_command_line(int argc, char** argv)
{
  Command_Options opt;
  for(int i = 0; i < argc; ++i)
  {
    auto option = argv[i];
    if(strcmp(option, "--test") == 0)
    {
      opt.test = true;
    }
  }
  return opt;
}

int main(int argc, char** argv)
{
  using namespace survive;

  uv_chdir("assets/");

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

  // Parse command line arguments.
  auto options = parse_command_line(argc - 1, argv+1);

  // Should be run the testing whatchamacallit?
  if(options.test)
  {
    int result = Catch::Session().run(1, argv);
    if(result)
    {
      return EXIT_FAILURE;
    }
  }

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

  // Initialize the graphics pipeline.
  auto pipeline = gfx::Pipeline{};

  // Load a shader program.
  auto shader_program = gfx::Program::from_files("shader/diffuse/vertex",
                                                 "shader/diffuse/fragment");
  // Prepare a mesh for rendering.
  auto mesh = pipeline.prepare_mesh(Mesh::from_file("obj/plane.obj"));

  auto tex =
      pipeline.prepare_texture(Texture::from_png_file("tex/cracked_soil.png"));

  int fps = 0;
  int time = glfwGetTime();

  // Set up some pre-rendering state.
  glClearColor(0.509, .694, .737, 1.0);
  glClearDepth(1);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Use our shader.
  shader_program.use();

  auto tex_loc = shader_program.get_uniform_location("tex");
  shader_program.set_uniform_int(tex_loc, 0);

  auto mvp_loc = shader_program.get_uniform_location("mvp");

  float deg = -5;
  while(!glfwWindowShouldClose(window))
  {
    ++fps;

    // Create our matrix.

    //glm::mat4 view = glm::lookAt(glm::vec3(0, 10, 0), glm::vec3(0, 0, 0),
                                 //glm::vec3(0, 0, 1));
    //glm::mat4 perspective = glm::frustum(-1, 1, -1, 1, -1, 1);
    //glm::mat4 perspective = glm::ortho(-1, 1, -1, 1, -1, 1);
    glm::mat4 view(1.0);
    view = glm::lookAt(glm::vec3(0, 0, 15), glm::vec3(0, 0, 0),
                       glm::vec3(0, 1, 0));
    glm::mat4 perspective(1.0);
    perspective = glm::perspective(45., 1., .01, 30.);

    auto model = glm::mat4(1.0);

    // rotate 90 degrees ccw
    //model = glm::translate(model, glm::vec3(0, cam_height, 0));
    model = glm::rotate(model, -3.14159f / 2.0f, glm::vec3(0, 1, 0));
    model = glm::rotate(model, deg, glm::vec3(0, 1, 0));
    //model = glm::translate(model, glm::vec3(0, 0, 0));
    //model = glm::translate(model, glm::vec3(0, 0, 7));
    model = glm::scale(model, glm::vec3(5));

    deg += .001;

    glm::mat4 mvp = perspective * view * model;

    // Set the matrix in the program.
    // TODO this seems bad, change this.
    shader_program.set_uniform_mat4(mvp_loc, mvp);

    // Clear the screen and render.
    pipeline.clear();
    pipeline.render_pipeline_mesh(mesh);

    // Show it on screen.
    glfwSwapBuffers(window);
    glfwPollEvents();

    if(int(glfwGetTime()) != time)
    {
      time = glfwGetTime();
      log_d("fps: %", fps);
      fps = 0;
    }
  }

  glfwTerminate();
  return 0;
}
