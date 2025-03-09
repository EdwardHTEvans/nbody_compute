#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>

#include <GLFW/glfw3.h>

GLuint LoadShader(const char *vertex_filename, const char *fragment_filename);
GLuint LoadComputeShader(const char *filename);

