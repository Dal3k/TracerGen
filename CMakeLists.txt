cmake_minimum_required(VERSION 3.21)
project(TracerGen)

set(CMAKE_CXX_STANDARD 17)

add_executable(TracerGen main.cpp vec3.h color.h ray.h hittable.h sphere.h hittable_list.h utility.h camera.h material.h moving_sphere.h aabb.h bvh.h texture.h perlin.h)
