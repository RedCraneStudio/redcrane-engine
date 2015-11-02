#version 330

layout(location = 0) out vec4 diffuse_out;

in vec2 uv;

uniform vec4 dif;
uniform sampler2D heightmap;

void main()
{
  diffuse_out = texture(heightmap, uv);
  diffuse_out.r *= dif.r;
  diffuse_out.g *= dif.g;
  diffuse_out.b *= dif.b;
  diffuse_out.a *= 1.0;
}
