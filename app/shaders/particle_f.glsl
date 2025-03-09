#version 330 core

// in from fragment shader
in vec3 f_v;

// out to render
out vec4 colour;

void main() {
  float speed = clamp(length(f_v), 0.0, 1.0);
  colour = vec4(speed, 0.0, 1.0 - (speed / 2), 1.0);
}
