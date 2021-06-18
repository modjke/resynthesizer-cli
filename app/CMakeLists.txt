project(libresynth)
cmake_minimum_required(VERSION 3.13)

set(SRC  imageSynth.c engine.c glibProxy.c engineParams.c imageFormat.c progress.c)

add_library(libresynth SHARED ${SRC} )

add_executable(cli cli.c)

target_link_libraries(cli m)
target_link_libraries(cli pthread)
target_link_libraries(cli libresynth)