cmake_minimum_required(VERSION 3.10)
project(ChladniPlateSimulation)

# Set C++ standard/compile features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If you have specific compiler flags you want to use, set them here
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Include directory for header files
include_directories(include)

# Automatically add all source files from the src directory
file(GLOB SOURCES "src/*.cpp")

# Define the executable with the project name and sources
add_executable(${PROJECT_NAME} ${SOURCES})

# Find OpenGL package
find_package(OpenGL REQUIRED)

# If using GLEW and GLFW, find those packages as well (uncomment as necessary)
# find_package(GLEW REQUIRED)
# find_package(glfw3 REQUIRED)

# Link libraries to the executable
# If using GLEW and/or GLFW, link those as well (uncomment as necessary)
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    # ${GLEW_LIBRARIES}
    # glfw
)

# For GLM, you might not need to link it if it's header-only, but make sure it's included if used
# include_directories(${GLM_INCLUDE_DIRS})

# Set the directory where the executable will be created
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Optionally, if you have shaders or assets, you can copy them to your build directory using custom commands
# This is useful for making sure your application runs from the build directory without manually copying files
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${PROJECT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
# )
