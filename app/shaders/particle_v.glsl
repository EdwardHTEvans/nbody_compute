#version 330 core

layout(location = 0) in vec3 a_p; // position from SSBO
layout(location = 1) in vec3 a_v; // velocity from SSBO

// out to fragment shader
out vec3 f_v;

// camera
uniform mat4 projection;
uniform mat4 view;

void main() {
  gl_Position = projection * view * vec4(a_p, 1.0);
  gl_PointSize = 2;
  f_v = a_v;
}
