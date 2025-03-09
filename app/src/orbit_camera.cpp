#include "orbit_camera.h"

glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

bool space_pressed = false;
bool left_mouse_pressed = false;
double last_x, last_y;
float yaw = 0.0f;
float pitch = 0.0f;
float radius = 1.0f;

glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 projection = glm::ortho(0.0f, 800.0f, 800.0f, 0.0f);

void OCKeyCallback(GLFWwindow *window, int key, int scancode, int action,

                   int mode) {
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    space_pressed = true;
  if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    space_pressed = false;
}

void OCMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      left_mouse_pressed = true;
      glfwGetCursorPos(window, &last_x, &last_y);
    } else if (action == GLFW_RELEASE) {
      left_mouse_pressed = false;
    }
  }
}

void OCMouseCallback(GLFWwindow *window, double xpos, double ypos) {
  if (!left_mouse_pressed)
    return;

  float xoffset = xpos - last_x;
  float yoffset = ypos - last_y;

  last_x = xpos;
  last_y = ypos;

  float sensitivity = 0.2f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  // prevent flipping
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;
}

void OCScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  radius -= yoffset * 0.2f;
  if (radius < 0.1f)
    radius = 0.1f;
}

glm::mat4 OCGetView() {
  float cam_x = radius * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  float cam_y = radius * sin(glm::radians(pitch));
  float cam_z = radius * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
  camera_pos = glm::vec3(cam_x, cam_y, cam_z) + target;

  return glm::lookAt(camera_pos, target, camera_up);
}
void OCSetTarget(glm::vec3 new_target) {
  target = new_target;
}

glm::mat4 OCGetProjection() {
  return projection;
}
void OCSetProjection(glm::mat4 new_projection) {
  projection = new_projection;
}
