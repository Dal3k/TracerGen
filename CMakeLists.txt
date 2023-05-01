cmake_minimum_required(VERSION 3.21)

project(TracerGen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -mcpu=apple-m1 -mtune=native -DNDEBUG")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_executable(TracerGen main.cpp vec3.h color.h ray.h hittable.h sphere.h hittable_list.h utility.h camera.h material.h moving_sphere.h aabb.h bvh.h texture.h perlin.h external/stb_image.h rtw_stb_image.h aarect.h box.h constant_medium.h stb_image_write.h tetrahedron.h triangle.h menger_sponge.cpp menger_sponge.h fractal_tree_3d.h cylinder.h barnsley_fern.h sierpinski_tetrahedron.h scenes.h fractal_noise_texture.h worley_noise_texture.h reaction_diffusion_texture.h reaction_diffusion_texture_color.h turbulence_texture.h warped_texture.h)

find_package(TBB REQUIRED)
target_link_libraries(TracerGen PRIVATE TBB::tbb)
