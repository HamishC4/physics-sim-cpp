cmake_minimum_required(VERSION 3.14)
project(PhysicsSim)

# Mathplot utilizes modern C++ features (C++20 is recommended)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find system dependencies that mathplot uses
find_package(OpenGL REQUIRED)

# Use PkgConfig to find GLFW and Freetype (standard for Linux/Mac)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(FREETYPE REQUIRED freetype2)

# 2. Add your executable
add_executable(PhysicsSimApp src/mathplot-visualise.cpp src/utilities.cpp)

# 3. Provide the mathplot and maths header directories to your executable
target_include_directories(PhysicsSimApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mathplot
    ${CMAKE_CURRENT_SOURCE_DIR}/mathplot/maths
)

target_compile_definitions(PhysicsSimApp PRIVATE
    MPLOT_FONTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/mathplot/fonts\"
)

# Raise the compile-time math operations limit for GCC
target_compile_options(PhysicsSimApp PRIVATE "-fconstexpr-ops-limit=100000000")



# 4. Link the system libraries to your executable
target_link_libraries(PhysicsSimApp PRIVATE
    OpenGL::GL
    ${GLFW_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

# 5. Provide the include paths for GLFW and Freetype so headers can be found
target_include_directories(PhysicsSimApp PRIVATE
    ${GLFW_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)