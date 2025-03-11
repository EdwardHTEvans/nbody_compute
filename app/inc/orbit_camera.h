#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void OCMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void OCMouseCallback(GLFWwindow *window, double xpos, double ypos);
void OCScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

glm::mat4 OCGetView();
void OCSetTarget(glm::vec3 new_target);

glm::mat4 OCGetProjection();
void OCSetProjection(glm::mat4 projection);

