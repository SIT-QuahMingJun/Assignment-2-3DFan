# ===========================================================================
# ImportDependencies.cmake
#
# Downloads GLM for CMake targets, plus GLFW and GLEW for Desktop.
# Android supplies its own OpenGL libraries, but still uses GLM.
# ===========================================================================

include(FetchContent)

function(importDependencies)


    # Shared dependency: Desktop + Android
    message(STATUS "[ImportDependencies] Fetching GLM ...")
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(glm)

    # Android already provides GLES, EGL and logging libraries.
    if(ANDROID)
        message(STATUS "[ImportDependencies] Android dependencies ready.")
        return()
    endif()

    # Desktop-only: GLFW
    message(STATUS "[ImportDependencies] Fetching GLFW ...")
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)

    message(STATUS "[ImportDependencies] Fetching GLEW ...")
    FetchContent_Declare(
        glew
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(glew-cmake_BUILD_STATIC ON  CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glew)

    message(STATUS "[ImportDependencies] All desktop dependencies ready.")

endfunction()
