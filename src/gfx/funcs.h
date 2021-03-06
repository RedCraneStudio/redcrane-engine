/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_FUNCS_H
#define REDC_GFX_FUNCS_H
#include "common.h"
#include "gltf/tiny_gltf_loader.h"
namespace redc
{

  // Static abstraction of OpenGL

  std::vector<Buf_Repr> make_buffers(std::size_t num);
  Buf_Repr make_buffer();
  void upload_data(Buf_Repr buf, Buffer_Target target, uint8_t* data,
                   std::size_t length);

  void make_textures(std::size_t num, Texture_Repr* reprs);

  void set_pixel_store_unpack_alignment(int align);
  void upload_image(Texture_Repr tex, std::vector<uint8_t> const& data,
                    Texture_Target target, Texture_Format dformat,
                    Texture_Format iformat, std::size_t width,
                    std::size_t height, Data_Type type);
  void allocate_texture(Texture_Repr tex, Texture_Target target,
                        Texture_Format iformat, std::size_t width,
                        std::size_t height);

  void set_sampler(Texture_Repr tex, Texture_Target target,
                   tinygltf::Sampler const& sampler);
  void set_mipmap_level(Texture_Repr tex, Texture_Target target,
                        unsigned int level);

  std::vector<Mesh_Repr> make_mesh_reprs(std::size_t num);

  Shader make_shader(Shader_Type type);
  Program_Repr make_program();
  void use_program(Program_Repr repr);
  void upload_shader_source(Shader shade, char const * source,
                            int source_length);
  bool compile_shader(Shader shader, std::vector<char>* info_log);
  void attach_shader(Program_Repr program, Shader_Repr shader);
  bool link_program(Program_Repr program, std::vector<char>* link_log);

  Attrib_Bind get_attrib_bind(Program_Repr program, std::string const& name);
  Param_Bind get_param_bind(Program_Repr program, std::string const& name);

  void destroy_shader(Shader_Repr shader);
  void destroy_program(Program_Repr program);
  void destroy_bufs(std::size_t num, Buf_Repr* bufs);
  void destroy_textures(std::size_t num, Texture_Repr* textures);
  void destroy_meshes(std::size_t num, Mesh_Repr* reprs);

  void use_mesh(Mesh_Repr mesh);
  void use_array_accessor(Attrib_Bind bind, Buf_Repr buf, Accessor const& acc);
  void use_element_array_accessor(Buf_Repr buf, Accessor const&);

  Attrib_Bind bad_attrib_bind();
  bool is_good_attrib_bind(Attrib_Bind bind);
  Param_Bind bad_param_bind();
  bool is_good_param_bind(Param_Bind bind);

  void enable_vertex_attrib_arrays(Mesh_Repr mesh, unsigned int start,
                                   unsigned int end);
  void draw_elements(std::size_t count, Data_Type type, Primitive_Type mode,
                     std::size_t offset);
  void draw_arrays(std::size_t count, Primitive_Type mode);

  void set_parameter(Param_Bind bind, Parameter const& param, int& texture_slot,
                     std::vector<Texture_Repr> const& textures);

  inline void set_parameter(Param_Bind bind, Param_Type type,
                            Param_Value const& val, int& texture_slot,
                            std::vector<Texture_Repr> const& textures)
  {
    Parameter param;
    param.type = type;
    param.value = val;
    set_parameter(bind, param, texture_slot, textures);
  }

  Texture_Format get_attachment_internal_format(Attachment attachment);
  Draw_Buffer to_draw_buffer(Attachment attachment);

  void make_framebuffers(std::size_t num, Framebuffer_Repr* reprs);
  void destroy_framebuffers(std::size_t num, Framebuffer_Repr* reprs);
  void bind_framebuffer(Fbo_Binding binding, Framebuffer_Repr fbo);
  void unbind_framebuffer();
  void framebuffer_attach_texture(Fbo_Binding binding, Attachment attachment,
                                  Texture_Target textarget, Texture_Repr tex,
                                  int level = 0);
  Fbo_Status check_framebuffer_status(Fbo_Binding target);
  std::string fbo_status_string(Fbo_Status status);
  void make_renderbuffers(std::size_t num, Renderbuffer_Repr* repr);
  void destroy_renderbuffers(std::size_t num, Renderbuffer_Repr* repr);
  void bind_renderbuffer(Renderbuffer_Repr repr);

  void renderbuffer_storage(Attachment attachment, std::size_t width,
                            std::size_t height);
  void framebuffer_attach_renderbuffer(Fbo_Binding binding,
                                       Attachment attachment,
                                       Renderbuffer_Repr rb);

  void set_draw_buffers(std::size_t num, Draw_Buffer* bufs);

#ifdef REDC_USE_OPENGL
  // = Uniform functions!

  // == Int functions
  inline void set_uint_parameter(Param_Bind bind, unsigned int const* val)
  {
    glUniform1uiv(bind.loc, 1, val);
  }
  inline void set_int_parameter(Param_Bind bind, int const* val)
  {
    glUniform1iv(bind.loc, 1, val);
  }
  inline void set_ivec2_parameter(Param_Bind bind, int const* val)
  {
    glUniform2iv(bind.loc, 1, val);
  }
  inline void set_ivec3_parameter(Param_Bind bind, int const* val)
  {
    glUniform3iv(bind.loc, 1, val);
  }
  inline void set_ivec4_parameter(Param_Bind bind, int const* val)
  {
    glUniform4iv(bind.loc, 1, val);
  }

  // == Bool functions
  inline void set_bool_parameter(Param_Bind bind, bool const* val)
  {
    glUniform1ui(bind.loc, *val);
  }
  inline void set_bvec2_parameter(Param_Bind bind, bool const* val)
  {
    glUniform2ui(bind.loc, val[0], val[1]);
  }
  inline void set_bvec3_parameter(Param_Bind bind, bool const* val)
  {
    glUniform3ui(bind.loc, val[0], val[1], val[2]);
  }
  inline void set_bvec4_parameter(Param_Bind bind, bool const* val)
  {
    glUniform4ui(bind.loc, val[0], val[1], val[2], val[3]);
  }

  // == Float functions
  inline void set_float_parameter(Param_Bind bind, float const* vec)
  {
    glUniform1fv(bind.loc, 1, vec);
  }
  inline void set_vec2_parameter(Param_Bind bind, float const* vec)
  {
    glUniform2fv(bind.loc, 1, vec);
  }
  inline void set_vec3_parameter(Param_Bind bind, float const* vec)
  {
    glUniform3fv(bind.loc, 1, vec);
  }
  inline void set_vec4_parameter(Param_Bind bind, float const* vec)
  {
    glUniform4fv(bind.loc, 1, vec);
  }
  inline void set_mat2_parameter(Param_Bind bind, float const* vec)
  {
    glUniformMatrix2fv(bind.loc, 1, GL_FALSE, vec);
  }
  inline void set_mat3_parameter(Param_Bind bind, float const* vec)
  {
    glUniformMatrix3fv(bind.loc, 1, GL_FALSE, vec);
  }
  inline void set_mat4_parameter(Param_Bind bind, float const* vec)
  {
    glUniformMatrix4fv(bind.loc, 1, GL_FALSE, vec);
  }
#else
  // == Int functions
  void set_uint_parameter(Param_Bind bind, unsigned int const* val);
  void set_int_parameter(Param_Bind bind, int const* val);
  void set_ivec2_parameter(Param_Bind bind, int const* val);
  void set_ivec3_parameter(Param_Bind bind, int const* val);
  void set_ivec4_parameter(Param_Bind bind, int const* val);

  // == Bool functions
  void set_bool_parameter(Param_Bind bind, bool const* val);
  void set_bvec2_parameter(Param_Bind bind, bool const* val);
  void set_bvec3_parameter(Param_Bind bind, bool const* val);
  void set_bvec4_parameter(Param_Bind bind, bool const* val);

  // == Float functions
  void set_float_parameter(Param_Bind bind, float const* vec);
  void set_vec2_parameter(Param_Bind bind, float const* vec);
  void set_vec3_parameter(Param_Bind bind, float const* vec);
  void set_vec4_parameter(Param_Bind bind, float const* vec);
  void set_mat2_parameter(Param_Bind bind, float const* vec);
  void set_mat3_parameter(Param_Bind bind, float const* vec);
  void set_mat4_parameter(Param_Bind bind, float const* vec);
#endif

}
#endif // Header block
