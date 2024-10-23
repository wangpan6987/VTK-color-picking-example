cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(ColorPick)

set(CMAKE_CXX_STANDARD 17) # 设置C++标准为C++17
set(CMAKE_CXX_STANDARD_REQUIRED True) # 如果不支持C++17则编译失败
set(CMAKE_CXX_EXTENSIONS False) # 不使用非标准的扩展


find_package(VTK COMPONENTS 
  CommonColor
  CommonCore
  CommonDataModel
  CommonTransforms
  FiltersCore
  FiltersGeneral
  FiltersModeling
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  RenderingAnnotation
  IOGeometry
  IOLegacy
  IOPLY
  IOXML
)


if (NOT VTK_FOUND)
  message(FATAL_ERROR "Test: Unable to find the VTK build folder.")
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
add_executable(Test MACOSX_BUNDLE ColorPick.cpp  "MouseInteractorStyle.cpp"MouseInteractorStyle.h MouseInteractorStyle.cpp)
  target_link_libraries(Test PRIVATE ${VTK_LIBRARIES}
)
# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS Test
  MODULES ${VTK_LIBRARIES}
)