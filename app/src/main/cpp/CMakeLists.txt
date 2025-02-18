# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)

project("cube_planets")

add_subdirectory(glm)

# Creates your game shared library. The name must be the same as the
# one used for loading in your Kotlin/Java or AndroidManifest.txt files.
add_library(cube_planets SHARED
        main.cpp
        AndroidOut.cpp
        Engine.cpp
        Shader.cpp
        Texture2D.cpp
        Camera.cpp
        DrawableBase.cpp
        DrawableTextured.cpp
        Entity.cpp
        Scene.cpp
        Input.cpp
        CubePlanet.cpp
        Geometry.cpp
        Utility.cpp)

# Searches for a package provided by the game activity dependency
find_package(game-activity REQUIRED CONFIG)

# Configure libraries CMake uses to link your target library.
target_link_libraries(cube_planets
        # The game activity
        game-activity::game-activity

        # EGL and other dependent libraries required for drawing
        # and interacting with Android system
        EGL
        GLESv3
        glm
        jnigraphics
        android
        log)