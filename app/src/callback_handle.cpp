#include "callback_handle.h"

#include <algorithm>
#include <mutex>
#include <vector>

std::vector<GLFWcursorposfun> cursorPosCallbacks;
std::vector<GLFWmousebuttonfun> mouseButtonCallbacks;
std::vector<GLFWscrollfun> scrollCallbacks;
std::vector<GLFWkeyfun> keyCallbacks;
std::vector<GLFWcharfun> charCallbacks;

std::vector<GLFWframebuffersizefun> framebufferSizeCallbacks;
std::vector<GLFWwindowsizefun> windowSizeCallbacks;
std::vector<GLFWwindowclosefun> windowCloseCallbacks;
std::vector<GLFWwindowrefreshfun> windowRefreshCallbacks;
std::vector<GLFWwindowfocusfun> windowFocusCallbacks;
std::vector<GLFWwindowiconifyfun> windowIconifyCallbacks;
std::vector<GLFWwindowmaximizefun> windowMaximizeCallbacks;
std::vector<GLFWwindowposfun> windowPosCallbacks;

std::vector<GLFWmonitorfun> monitorCallbacks;
std::vector<GLFWjoystickfun> joystickCallbacks;

std::mutex callbackMutex;

void CleanupCallbacks() {
  std::lock_guard<std::mutex> lock(callbackMutex);
  cursorPosCallbacks.clear();
  mouseButtonCallbacks.clear();
  scrollCallbacks.clear();
  keyCallbacks.clear();
  charCallbacks.clear();
  framebufferSizeCallbacks.clear();
  windowSizeCallbacks.clear();
  windowCloseCallbacks.clear();
  windowRefreshCallbacks.clear();
  windowFocusCallbacks.clear();
  windowIconifyCallbacks.clear();
  windowMaximizeCallbacks.clear();
  windowPosCallbacks.clear();
  monitorCallbacks.clear();
  joystickCallbacks.clear();
}

template <typename T> void AddCallback(std::vector<T> &callbackList, T callback) {
  std::lock_guard<std::mutex> lock(callbackMutex);
  // duplicate protection
  if (std::find(callbackList.begin(), callbackList.end(), callback) == callbackList.end()) {
    callbackList.push_back(callback);
  }
}

template <typename T, typename... Args>
void ExecuteCallbacks(const std::vector<T> &callbackList, Args... args) {
  std::lock_guard<std::mutex> lock(callbackMutex);
  for (auto &callback : callbackList) {
    callback(args...);
  }
}

// registering callback functions
void RegisterCursorPosCallback(GLFWcursorposfun callback) {
  AddCallback(cursorPosCallbacks, callback);
}

void RegisterMouseButtonCallback(GLFWmousebuttonfun callback) {
  AddCallback(mouseButtonCallbacks, callback);
}

void RegisterScrollCallback(GLFWscrollfun callback) {
  AddCallback(scrollCallbacks, callback);
}

void RegisterKeyCallback(GLFWkeyfun callback) {
  AddCallback(keyCallbacks, callback);
}

void RegisterCharCallback(GLFWcharfun callback) {
  AddCallback(charCallbacks, callback);
}

void RegisterFramebufferSizeCallback(GLFWframebuffersizefun callback) {
  AddCallback(framebufferSizeCallbacks, callback);
}

void RegisterWindowSizeCallback(GLFWwindowsizefun callback) {
  AddCallback(windowSizeCallbacks, callback);
}

void RegisterWindowCloseCallback(GLFWwindowclosefun callback) {
  AddCallback(windowCloseCallbacks, callback);
}

void RegisterWindowRefreshCallback(GLFWwindowrefreshfun callback) {
  AddCallback(windowRefreshCallbacks, callback);
}

void RegisterWindowFocusCallback(GLFWwindowfocusfun callback) {
  AddCallback(windowFocusCallbacks, callback);
}

void RegisterWindowIconifyCallback(GLFWwindowiconifyfun callback) {
  AddCallback(windowIconifyCallbacks, callback);
}

void RegisterWindowMaximizeCallback(GLFWwindowmaximizefun callback) {
  AddCallback(windowMaximizeCallbacks, callback);
}

void RegisterWindowPosCallback(GLFWwindowposfun callback) {
  AddCallback(windowPosCallbacks, callback);
}

void RegisterMonitorCallback(GLFWmonitorfun callback) {
  AddCallback(monitorCallbacks, callback);
}

void RegisterJoystickCallback(GLFWjoystickfun callback) {
  AddCallback(joystickCallbacks, callback);
}

// callback functions
void CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  ExecuteCallbacks(cursorPosCallbacks, window, xpos, ypos);
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  ExecuteCallbacks(mouseButtonCallbacks, window, button, action, mods);
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  ExecuteCallbacks(scrollCallbacks, window, xoffset, yoffset);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  ExecuteCallbacks(keyCallbacks, window, key, scancode, action, mods);
}

void CharCallback(GLFWwindow *window, unsigned int codepoint) {
  ExecuteCallbacks(charCallbacks, window, codepoint);
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
  ExecuteCallbacks(framebufferSizeCallbacks, window, width, height);
}

void WindowSizeCallback(GLFWwindow *window, int width, int height) {
  ExecuteCallbacks(windowSizeCallbacks, window, width, height);
}

void WindowCloseCallback(GLFWwindow *window) {
  ExecuteCallbacks(windowCloseCallbacks, window);

  // cleanup, the windows closing
  CleanupCallbacks();
}

void WindowRefreshCallback(GLFWwindow *window) {
  ExecuteCallbacks(windowRefreshCallbacks, window);
}

void WindowFocusCallback(GLFWwindow *window, int focused) {
  ExecuteCallbacks(windowFocusCallbacks, window, focused);
}

void WindowIconifyCallback(GLFWwindow *window, int iconified) {
  ExecuteCallbacks(windowIconifyCallbacks, window, iconified);
}

void WindowMaximizeCallback(GLFWwindow *window, int maximized) {
  ExecuteCallbacks(windowMaximizeCallbacks, window, maximized);
}

void WindowPosCallback(GLFWwindow *window, int xpos, int ypos) {
  ExecuteCallbacks(windowPosCallbacks, window, xpos, ypos);
}

void MonitorCallback(GLFWmonitor *monitor, int event) {
  ExecuteCallbacks(monitorCallbacks, monitor, event);
}

void JoystickCallback(int jid, int event) {
  ExecuteCallbacks(joystickCallbacks, jid, event);
}

void CallbackInitialise(GLFWwindow *window) {
  glfwSetCursorPosCallback(window, CursorPosCallback);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSetScrollCallback(window, ScrollCallback);
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetCharCallback(window, CharCallback);

  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetWindowSizeCallback(window, WindowSizeCallback);
  glfwSetWindowCloseCallback(window, WindowCloseCallback);
  glfwSetWindowRefreshCallback(window, WindowRefreshCallback);
  glfwSetWindowFocusCallback(window, WindowFocusCallback);
  glfwSetWindowIconifyCallback(window, WindowIconifyCallback);
  glfwSetWindowMaximizeCallback(window, WindowMaximizeCallback);
  glfwSetWindowPosCallback(window, WindowPosCallback);

  glfwSetMonitorCallback(MonitorCallback);
  glfwSetJoystickCallback(JoystickCallback);
}
