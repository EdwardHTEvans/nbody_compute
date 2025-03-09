#version 430 core

layout(local_size_x = 256) in;

struct Particle {
  vec4 position; // xyz = position, w = mass
  vec4 velocity; // xyz = velocity, w = padding
};

layout(std430, binding = 0) buffer Particles {
  Particle particles[];
};

uniform float deltaTime;
uniform float G;

void main() {
  uint i = gl_GlobalInvocationID.x;
  if (i >= particles.length())
    return;

  vec3 acceleration = vec3(0.0);
  for (uint j = 0; j < particles.length(); j++) {
    if (i == j)
      continue;

    vec3 direction = particles[j].position.xyz - particles[i].position.xyz;
    float distanceSq = dot(direction, direction);

    // prevent division by zero
    if (distanceSq < 1e-6)
      continue;

    float distance = sqrt(distanceSq);
    float force = G * particles[i].position.w * particles[j].position.w / (distanceSq + 1e-6);

    acceleration += normalize(direction) * (force / particles[i].position.w);
  }

  particles[i].velocity.xyz += acceleration * deltaTime;
  particles[i].position.xyz += particles[i].velocity.xyz * deltaTime;
}
