#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "callback_handle.h"
#include "orbit_camera.h"
#include "shader.h"
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

GLuint window_w = 1024, window_h = 1024;
GLuint computeShader;
GLuint particleShader;
float deltaTime = 0.0;
bool space_pressed = false;

float G = 6.67430e-11f;
float centralMass = 1e9f;

static const std::filesystem::path computeShaderPath = "app/shaders/nbody_c.glsl";
static const std::filesystem::path vertexShaderPath = "app/shaders/particle_v.glsl";
static const std::filesystem::path fragmentShaderPath = "app/shaders/particle_f.glsl";

// Padded for std430 layout
struct Particle {
  glm::vec4 position; // (x, y, z, w=mass)
  glm::vec4 velocity; // (vx, vy, vz, w=padding)
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    // load compute shader
    GLuint newComputeShader = LoadComputeShader(computeShaderPath.c_str());
    if (computeShader != GL_INVALID_INDEX)
      computeShader = newComputeShader;

    GLuint newParticleShader = LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    if (newParticleShader != GL_INVALID_INDEX)
      particleShader = newParticleShader;
  }

  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    space_pressed = true;
  if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    space_pressed = false;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
    deltaTime += yoffset * 0.0016f;
    if (deltaTime < 0.0f)
      deltaTime = 0.0f;
  } else {
    OCScrollCallback(window, xoffset, yoffset);
  }
}

void window_callback(GLFWwindow *window, int width, int height) {
  window_w = width;
  window_h = height;
  glViewport(0, 0, window_w, window_h);
  float window_ratio = (float) window_w / (float) window_h;
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), window_ratio, 0.1f, 100.0f);
  OCSetProjection(projection);
}

void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar *message, const void *userParam) {
  std::cerr << "debug: " << message << std::endl;
}

int main(void) {
  glfwInit();

  // Initialise window
  GLFWwindow *window = glfwCreateWindow(window_w, window_h, "particles", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);
  glViewport(0, 0, window_w, window_h);
  glEnable(GL_DEBUG_OUTPUT);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glDebugMessageCallback(MessageCallback, 0);

  // Check version
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  std::cout << "OpenGL Version: " << major << "." << minor << std::endl;

  // Initialise callbacks
  CallbackInitialise(window);
  RegisterKeyCallback(key_callback);
  RegisterScrollCallback(scroll_callback);
  RegisterWindowSizeCallback(window_callback);

  // Setup orbit camera
  RegisterCursorPosCallback(OCMouseCallback);
  RegisterMouseButtonCallback(OCMouseButtonCallback);
  {
    float window_ratio = (float) window_w / (float) window_h;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), window_ratio, 0.1f, 100.0f);
    OCSetProjection(projection);
  }

  // Needed for vertex shader to change size of
  // Vertex with point size
  glEnable(GL_PROGRAM_POINT_SIZE);

  // Load compute shader
  computeShader = LoadComputeShader(computeShaderPath.c_str());
  if (computeShader == GL_INVALID_INDEX)
    exit(1);

  // Load particle shader
  particleShader = LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
  if (particleShader == GL_INVALID_INDEX)
    exit(1);

  // Initialise particles
  unsigned int n_particles = 256 * 20;
  std::vector<Particle> particles(n_particles);
  for (size_t i = 0; i < n_particles; i++) {
    glm::vec3 velocity;
    float x, y, z, w;
    float theta, phi, r;

    if (i == 0) {
      particles[i].position = glm::vec4(0.0f, 0.0f, 0.0f, centralMass);
      particles[i].velocity = glm::vec4(0.0f);
    } else {
      theta = ((float) rand() / (float) RAND_MAX) * 2.0f * M_PI;
      phi = acos((2.0f * ((float) rand() / (float) RAND_MAX)) - 1.0f);
      r = cbrt((float) rand() / (float) RAND_MAX);

      x = r * sin(phi) * cos(theta);
      y = ((r * 0.05) * sin(phi) * sin(theta));
      z = r * cos(phi);
      w = 2e3f;

      // Compute perpendicular velocity direction
      glm::vec3 radial_direction = glm::normalize(glm::vec3(x, y, z));
      glm::vec3 arbitrary_axis = glm::vec3(0, 1, 0);
      glm::vec3 tangent_velocity = glm::normalize(glm::cross(radial_direction, arbitrary_axis));

      // Compute orbital velocity
      float distance = glm::length(glm::vec2(x, z));
      float orbital_speed = sqrt((G * centralMass) / (distance + 1e-6f));

      velocity = tangent_velocity * orbital_speed;
      particles[i].position = glm::vec4(x, y, z, w);
      particles[i].velocity = glm::vec4(velocity, 0.0f);
    }
  }

  // Upload initial data to SSBO
  GLuint ssbo;
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, n_particles * sizeof(Particle), particles.data(),
               GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  size_t mem_width = 0;
  // Bind SSBO as a VBO to use in rendering
  glBindBuffer(GL_ARRAY_BUFFER, ssbo);

  // Enable position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) (mem_width));
  mem_width += sizeof(Particle::position);

  // Enable velocity attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) (mem_width));
  mem_width += sizeof(Particle::velocity);

  // Ensure vertex attributes are synchronized
  glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

  // Constants for fixed timestep
  const double fixedTimeStep = 1.0 / 120.0;
  double accumulator = 0.0;
  double lastTime = glfwGetTime();

  double frameTimeSum = 0.0;
  int frameCount = 0;

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // Get time since last frame
    double currentTime = glfwGetTime();
    double frameTime = currentTime - lastTime;
    lastTime = currentTime;
    accumulator += frameTime;

    auto start = std::chrono::high_resolution_clock::now();
    glm::mat4 view = OCGetView();
    glm::mat4 projection = OCGetProjection();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Run physics updates at a fixed step
    while (accumulator >= fixedTimeStep) {
      // Bind SSBO for compute shader
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

      // Dispatch compute shader
      glUseProgram(computeShader);
      glUniform1f(glGetUniformLocation(computeShader, "deltaTime"), deltaTime);
      glUniform1f(glGetUniformLocation(computeShader, "G"), G);
      glDispatchCompute((n_particles + 255) / 256, 1, 1);

      // Ensure all compute writes are visible before rendering
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      accumulator -= fixedTimeStep; // Remove processed time
    }

    // Now bind VAO for rendering
    glBindVertexArray(vao);
    glUseProgram(particleShader);
    GLuint viewLoc = glGetUniformLocation(particleShader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    GLuint projLoc = glGetUniformLocation(particleShader, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glDrawArrays(GL_POINTS, 0, n_particles);

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    Particle *particle = (Particle *) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    OCSetTarget(particle[0].position);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    auto end = std::chrono::high_resolution_clock::now();

    // Moving average FPS over 100 frames
    frameTimeSum += std::chrono::duration<double>(end - start).count();
    frameCount++;

    if (frameCount >= 100) {
      double avgFrameTime = frameTimeSum / 100.0;
      double avgFPS = 1.0 / avgFrameTime;
      std::cout << "Avg FPS: " << avgFPS << std::endl;
      frameTimeSum = 0.0;
      frameCount = 0;
    }
  }

  // Cleanup
  glDeleteProgram(computeShader);
  glDeleteBuffers(1, &ssbo);
  glfwTerminate();

  return 0;
}
