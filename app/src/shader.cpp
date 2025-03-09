#include <fstream>
#include <iostream>
#include <sstream>

#include "shader.h"

std::string ReadShaderFile(const char *filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Failed to open shader file: " << filename << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

// Function to compile a shader
GLuint CompileShader(const char *source, GLenum type) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  // Check for compilation errors
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(shader, 512, nullptr, info_log);
    std::cerr << "Shader Compilation Failed:\n" << info_log << std::endl;
    glDeleteShader(shader);
    return GL_INVALID_INDEX;
  }
  return shader;
}

GLuint LoadShader(const char *vertex_filename, const char *fragment_filename) {
  std::string vertex_code = ReadShaderFile(vertex_filename);
  std::string fragment_code = ReadShaderFile(fragment_filename);

  if (vertex_code.empty() || fragment_code.empty()) {
    return GL_INVALID_INDEX;
  }

  // compile
  GLuint vertex_shader = CompileShader(vertex_code.c_str(), GL_VERTEX_SHADER);
  GLuint fragment_shader = CompileShader(fragment_code.c_str(), GL_FRAGMENT_SHADER);

  if (vertex_shader == 0 || fragment_shader == 0) {
    return GL_INVALID_INDEX;
  }

  // link
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  // check errors
  GLint success;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
    std::cerr << "Shader Program Linking Failed:\n" << info_log << std::endl;
    glDeleteProgram(shader_program);
    shader_program = GL_INVALID_INDEX;
  }

  // cleanup
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
}

GLuint LoadComputeShader(const char *filename) {
  std::string computeCode = ReadShaderFile(filename);
  if (computeCode.empty()) {
    return GL_INVALID_INDEX;
  }

  // compile
  GLuint compute_shader = CompileShader(computeCode.c_str(), GL_COMPUTE_SHADER);

  if (compute_shader == 0) {
    return GL_INVALID_INDEX;
  }

  // link
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, compute_shader);
  glLinkProgram(shader_program);

  // check errors
  GLint success;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
    std::cerr << "Shader Program Linking Failed:\n" << info_log << std::endl;
    glDeleteProgram(shader_program);
    shader_program = GL_INVALID_INDEX;
  }

  // cleanup
  glDeleteShader(compute_shader);

  return shader_program;
}
