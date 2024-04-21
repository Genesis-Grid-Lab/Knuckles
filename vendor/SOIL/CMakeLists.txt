cmake_minimum_required(VERSION 3.6)
#just a fucking test
include_directories(inc)

ADD_LIBRARY( soil SHARED
  src/image_helper.c
  src/stb_image_aug.c
  src/image_DXT.c
  src/SOIL.c
  )


