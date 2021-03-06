/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "sdl_helper.h"
#include "common/log.h"
#include "common/debugging.h"
namespace redc
{
  SDL_Init_Lock::SDL_Init_Lock(SDL_Init_Lock&& l)
          : window(l.window),
            gl_context(l.gl_context),
            properties(l.properties)
  {
    l.window = nullptr;
    l.gl_context = nullptr;
  }

  SDL_Init_Lock& SDL_Init_Lock::operator=(SDL_Init_Lock&& l)
  {
    window = l.window;
    gl_context = l.gl_context;
    l.properties = l.properties;

    l.window = nullptr;
    l.gl_context = nullptr;

    return *this;
  }

  SDL_Init_Lock::~SDL_Init_Lock()
  {
    // If we don't have both for any reason, don't bother deallocating either.
    if(window && gl_context)
    {
      // That way, if we accidentally partially move we won't do the aggressive
      // SDL_Quit call.

      SDL_GL_DeleteContext(gl_context);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }
  }

  SDL_Init_Lock init_sdl(std::string title, Vec<int> res, bool fullscreen,
                         bool vsync)
  {
    // Initialize SDL

    if(SDL_Init(SDL_INIT_VIDEO))
    {
      log_e("Failed to init SDL");
      return {};
    }

    SDL_version version;
    SDL_GetVersion(&version);

    log_i("Initialized SDL %.%.%", version.major, version.minor, version.patch);

    // Initialize window

    int flags = SDL_WINDOW_OPENGL;
    if(fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    SDL_Init_Lock ret;

    ret.window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, res.x, res.y,
                                  flags);
    if(!ret.window)
    {
      SDL_Quit();
      log_e("Failed to initialize SDL for resolution %x%", res.x, res.y);
      return ret;
    }

    // Initialize OpenGL context

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    ret.gl_context = SDL_GL_CreateContext(ret.window);

    bool nvidia_fix = false;
    if(ret.gl_context == NULL)
    {
      log_e("Failed to create OpenGL context: %", SDL_GetError());
      log_i("Trying a new configuration... (NVIDIA fix)");
      SDL_GL_ResetAttributes();

      SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

      SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
      SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

      // Don't request an sRGB context right away, just check later and do gamma
      // correction ourselves if necessary.

      ret.gl_context = SDL_GL_CreateContext(ret.window);
      nvidia_fix = true;

      if(ret.gl_context == NULL)
      {
        REDC_UNREACHABLE_MSG("Failed to make OpenGL context");
      }
    }

    SDL_GL_MakeCurrent(ret.window, ret.gl_context);

    // Set vsync setting
    int sync_interval = 0;
    if(vsync) sync_interval = 1;
    if(SDL_GL_SetSwapInterval(sync_interval) == -1)
    {
      // Shit, failed to set that
      log_w("Failed to set swap interval: %", SDL_GetError());
    }

    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);

    log_i("OpenGL: %, GLSL: %, Vendor: %, Renderer: %",
          glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION),
          glGetString(GL_VENDOR), glGetString(GL_RENDERER));

    GLint front_type = 0;
    glGetFramebufferAttachmentParameteriv(
      GL_DRAW_FRAMEBUFFER, GL_BACK_LEFT,
      GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &front_type
    );
    std::string framebuf_string("Unknown type");
    switch(front_type)
    {
    case GL_LINEAR:
      framebuf_string = "RGB";
      ret.properties.srgb = false;
      if(nvidia_fix)
      {
        framebuf_string = "sRGB (probably; NVIDIA bug)";
        ret.properties.srgb = true;
      }
      break;
    case GL_SRGB:
      framebuf_string = "sRGB";
      ret.properties.srgb = true;
      break;
    default:
      REDC_UNREACHABLE_MSG("Invalid framebuffer type");
    }
    log_d("Using % framebuffer", framebuf_string);

    return ret;
  }
}
