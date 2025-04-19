cmake_minimum_required(VERSION 3.00)  # CMake 最低版本要求
project(2DEngine LANGUAGES C CXX)       # 项目名称和语言（C++）

#设置 C 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/glad.c          # 包含 GLAD 的 C 文件
)

# 包含头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
  include             # GLFW 和 GLAD 头文件所在目录
  include/glad
  include/GLFW
  include/glm
  include/glm/gtc
)

# 链接 GLFW 库
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/libglfw3.a  # GLFW 静态库路径
  opengl32                            # Windows 的 OpenGL 库
  gdi32                               # Windows 的 GDI 库
)
