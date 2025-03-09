
# app

include_directories("inc")
set(SOURCE_DIR "src")

set(SOURCES
    src/main.cpp
    src/shader.cpp
    src/callback_handle.cpp
    src/orbit_camera.cpp
)

# Add the executable target
add_executable(app ${SOURCES})
target_link_libraries(app glfw)
target_link_libraries(app glad)
target_link_libraries(app glm)

