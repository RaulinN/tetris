cmake_minimum_required(VERSION 3.27)
project(tetris C)

set(CMAKE_C_STANDARD 99)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
endif()

# https://stackoverflow.com/questions/74899325/sdl2-ld-symbols-not-found
# https://stackoverflow.com/questions/28395833/using-sdl2-with-cmake
# https://wolfgang-ziegler.com/blog/sdl-cmake-osx
# find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED)

message ([STATUS] " SDL2_INCLUDE_DIR => " ${SDL2_INCLUDE_DIRS})
message ([STATUS] " SDL2_LIBRARIES => " ${SDL2_LIBRARIES})

add_executable(tetris src/main.c
        src/log.h
        src/tetromino.c
        src/tetromino.h
        src/game.c
        src/game.h
        src/board.c
        src/board.h
        src/piece.c
        src/piece.h
)

# include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
