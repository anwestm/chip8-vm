cmake_minimum_required(VERSION 3.9)
project(chip8_vm C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_FLAGS “${CMAKE_FLAGS} -lmingw32”)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/lib/include)
link_directories(${PROJECT_SOURCE_DIR}/lib/lib)

file(GLOB CHIP8VM_SRC "src/*.c" "src/*.h")


add_executable(chip8_vm ${CHIP8VM_SRC} src/chip8.c src/chip8.h src/input.c src/input.h)
target_link_libraries(chip8_vm SDL2 SDL2main)
