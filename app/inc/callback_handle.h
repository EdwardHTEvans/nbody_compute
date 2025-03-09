#pragma once
#define GLAD_GL_IMPLEMENTATION // necessary for headeronly version.
#include <glad/gl.h>

#include <GLFW/glfw3.h>

// initialise callbacks
void CallbackInitialise(GLFWwindow *window);

// input callbacks
void RegisterCursorPosCallback(GLFWcursorposfun callback);
void RegisterMouseButtonCallback(GLFWmousebuttonfun callback);
void RegisterScrollCallback(GLFWscrollfun callback);
void RegisterKeyCallback(GLFWkeyfun callback);
void RegisterCharCallback(GLFWcharfun callback);

// window callbacks
void RegisterFramebufferSizeCallback(GLFWframebuffersizefun callback);
void RegisterWindowSizeCallback(GLFWwindowsizefun callback);
void RegisterWindowCloseCallback(GLFWwindowclosefun callback);
void RegisterWindowRefreshCallback(GLFWwindowrefreshfun callback);
void RegisterWindowFocusCallback(GLFWwindowfocusfun callback);
void RegisterWindowIconifyCallback(GLFWwindowiconifyfun callback);
void RegisterWindowMaximizeCallback(GLFWwindowmaximizefun callback);
void RegisterWindowPosCallback(GLFWwindowposfun callback);

// monitor callback
void RegisterMonitorCallback(GLFWmonitorfun callback);

// joystick callback
void RegisterJoystickCallback(GLFWjoystickfun callback);
