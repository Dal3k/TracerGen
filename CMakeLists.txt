cmake_minimum_required(VERSION 3.21)
project(TracerGen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -march=native -mtune=native -DNDEBUG")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
add_executable(TracerGen main.cpp vec3.h color.h ray.h hittable.h sphere.h hittable_list.h utility.h camera.h material.h moving_sphere.h aabb.h bvh.h texture.h perlin.h external/stb_image.h rtw_stb_image.h aarect.h box.h constant_medium.h)
