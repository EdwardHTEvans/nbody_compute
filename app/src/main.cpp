#define GLAD_GL_IMPLEMENTATION // necessary for headeronly version.
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

const GLuint window_w = 1024, window_h = 1024;
GLuint computeShader;
GLuint particleShader;
float deltaTime = 0.0;

static const std::filesystem::path computeShaderPath = "app/shaders/nbody_c.glsl";
static const std::filesystem::path vertexShaderPath = "app/shaders/particle_v.glsl";
static const std::filesystem::path fragmentShaderPath = "app/shaders/particle_f.glsl";

// padded for std430 layout
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
}

void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar *message, const void *userParam) {
  std::cerr << "debug: " << message << std::endl;
}

int main(void) {
  glfwInit();

  // initialise window
  GLFWwindow *window = glfwCreateWindow(window_w, window_h, "particles", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  // check version
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  std::cout << "OpenGL Version: " << major << "." << minor << std::endl;

  // initialise callbacks
  CallbackInitialise(window);
  RegisterKeyCallback(key_callback);

  // setup orbit camera
  RegisterCursorPosCallback(OCMouseCallback);
  RegisterMouseButtonCallback(OCMouseButtonCallback);
  RegisterScrollCallback(OCScrollCallback);
  RegisterKeyCallback(OCKeyCallback);
  {
    float window_ratio = (float) window_w / (float) window_h;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), window_ratio, 0.1f, 100.0f);
    OCSetProjection(projection);
  }

  // needed for vertex shader to change size of
  // vertex with point size
  glEnable(GL_PROGRAM_POINT_SIZE);

  // load compute shader
  computeShader = LoadComputeShader(computeShaderPath.c_str());
  if (computeShader == GL_INVALID_INDEX)
    exit(1);

  // load particle shader
  particleShader = LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
  if (particleShader == GL_INVALID_INDEX)
    exit(1);

  // initialise particles
  unsigned int n_particles = 50000;
  std::vector<Particle> particles(n_particles);
  for (auto &p : particles) {

    // sphere
    float theta = ((float) rand() / (float) RAND_MAX) * 2.0f * M_PI;
    float phi = acos((2.0f * ((float) rand() / (float) RAND_MAX)) - 1.0f);
    float r = cbrt((float) rand() / (float) RAND_MAX);

    float x = r * sin(phi) * cos(theta);
    float y = r * sin(phi) * sin(theta);
    float z = r * cos(phi);
    float w = (((double) rand() / (RAND_MAX)) * 100000);

    /* DEBUG: cube
    float x = (((double)rand() / (RAND_MAX)) * 2) - 1;
    float y = (((double)rand() / (RAND_MAX)) * 2) - 1;
    float z = (((double)rand() / (RAND_MAX)) * 2) - 1;
    float w = (((double)rand() / (RAND_MAX)) * 100000);
    */

    p.position = glm::vec4(x, y, z, w);
    p.velocity = glm::vec4(0.0f);
  }

  // Upload initial data to SSBO
  GLuint ssbo;
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, n_particles * sizeof(Particle), particles.data(),
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // bind SSBO as a VBO to use in rendering
  glBindBuffer(GL_ARRAY_BUFFER, ssbo);
  // enable position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) 0);
  // enable velocity attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) (sizeof(glm::vec4)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    // deadcode: for incase i need to remember
    // float t = (float)glfwGetTime();

    // compute bodies
    glUseProgram(computeShader);
    glUniform1f(glGetUniformLocation(computeShader, "deltaTime"), deltaTime);
    glUniform1f(glGetUniformLocation(computeShader, "G"), 6.67430e-11f);
    glDispatchCompute((n_particles + 255) / 256, 1, 1);

    // ensure compute is done before render
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw bodies
    glUseProgram(particleShader);

    glm::mat4 view = OCGetView();
    GLuint viewLoc = glGetUniformLocation(particleShader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, n_particles);

    glm::mat4 projection = OCGetProjection();
    GLuint projLoc = glGetUniformLocation(particleShader, "projection");
    glUseProgram(particleShader);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    /* DEBUG: read first particle data
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    Particle *first_particle = (Particle *) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    */

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // cleanup
  glDeleteProgram(computeShader);
  glDeleteBuffers(1, &ssbo);
  glfwTerminate();

  return 0;
}
