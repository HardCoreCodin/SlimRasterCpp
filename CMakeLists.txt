cmake_minimum_required(VERSION 3.8)

project(SlimRaster_1_Clipping)
add_executable(SlimRaster_1_Clipping WIN32 src/examples/1_clipping.cpp)
project(SlimRaster_2_NormalMaps)
add_executable(SlimRaster_2_NormalMaps WIN32 src/examples/2_normal_maps.cpp)
project(SlimRaster_3_debug_shaders)
#add_executable(SlimRaster_3_debug_shaders WIN32 src/examples/1_clipping_interpolation.c)

project(obj2mesh)
add_executable(obj2mesh src/obj2mesh.cpp)

project(bmp2texture)
add_executable(bmp2texture src/bmp2texture.cpp)