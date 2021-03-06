#version 330
layout(location = 0) in vec3 vertex;

uniform mat4 proj;
uniform mat4 view;

void main()
{
  gl_Position = proj * view * vec4(vertex, 1.0);
}
