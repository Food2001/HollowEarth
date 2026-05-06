cmake_minimum_required(VERSION 3.10)
project(HollowEarth)

set(CMAKE_CXX_STANDARD 17)

add_executable(HollowEarth
    main.cpp
    recursos.rc
    src/glad.c
    src/shader.cpp
    src/camara.cpp
    src/terreno.cpp
    src/jugador.cpp
    src/mundo.cpp
    src/textura.cpp
    src/hud.cpp
    src/agua.cpp
    src/skybox.cpp
    src/zombie.cpp
    src/particula.cpp
    src/inventario.cpp
    src/Gestor.cpp
)

target_include_directories(HollowEarth PRIVATE
    include
    C:/SDL3/include
    C:/SDL3_image/include
)

target_link_directories(HollowEarth PRIVATE
    C:/SDL3/lib
    C:/SDL3_image/lib
)

target_link_libraries(HollowEarth PRIVATE
    SDL3
    SDL3_image
    opengl32
)

add_custom_command(TARGET HollowEarth POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        C:/SDL3/bin/SDL3.dll
        C:/SDL3_image/bin/SDL3_image.dll
        $<TARGET_FILE_DIR:HollowEarth>
)

add_custom_command(TARGET HollowEarth POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:HollowEarth>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.glsl
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.glsl
        $<TARGET_FILE_DIR:HollowEarth>/shaders/
)

add_custom_command(TARGET HollowEarth POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:HollowEarth>/assets/textures
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/pasto.png
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/arena.png
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/roca.png
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/nieve.png
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/agua.png
        $<TARGET_FILE_DIR:HollowEarth>/assets/textures/
)

add_custom_command(TARGET HollowEarth POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:HollowEarth>/assets/textures/skybox
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/skybox/clouds1_east.bmp
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/skybox/clouds1_west.bmp
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/skybox/clouds1_up.bmp
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/skybox/clouds1_down.bmp
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/skybox/clouds1_south.bmp
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/skybox/clouds1_north.bmp
        $<TARGET_FILE_DIR:HollowEarth>/assets/textures/skybox/
)