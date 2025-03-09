#include "orbit_camera.h"

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool      spaceBarPressed  = false;
bool      leftMousePressed = false;
double    lastX, lastY;
float     yaw    = 0.0f;
float     pitch  = 0.0f;
float     radius = 1.0f;
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

void oc_key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    spaceBarPressed = true;
  if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    spaceBarPressed = false;
}

void oc_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      leftMousePressed = true;
      glfwGetCursorPos(window, &lastX, &lastY);// Store initial position
    } else if (action == GLFW_RELEASE) {
      leftMousePressed = false;
    }
  }
}

void oc_mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (!leftMousePressed)
    return;

  float xoffset = xpos - lastX;
  float yoffset = ypos - lastY;// Normalized coordinates (Y is inverted)

  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.2f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  // Constrain pitch to prevent flipping
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;
}

void oc_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  radius -= yoffset * 0.2f;// Adjust zoom speed
  if (radius < 0.2f)
    radius = 0.2f;// Prevent too close zoom
}
