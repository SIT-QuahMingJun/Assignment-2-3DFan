# Removes generated output from every supported platform.

# STRAY BUILD OUTPUT DIRECTORIES
set(GENERATED_DIRECTORIES
    "${CMAKE_CURRENT_LIST_DIR}/build"
    "${CMAKE_CURRENT_LIST_DIR}/build_desktop"
    "${CMAKE_CURRENT_LIST_DIR}/out"
    "${CMAKE_CURRENT_LIST_DIR}/web"
    "${CMAKE_CURRENT_LIST_DIR}/external"
    "${CMAKE_CURRENT_LIST_DIR}/tmp"
    "${CMAKE_CURRENT_LIST_DIR}/android/.gradle"
    "${CMAKE_CURRENT_LIST_DIR}/android/.idea"
    "${CMAKE_CURRENT_LIST_DIR}/android/build"
    "${CMAKE_CURRENT_LIST_DIR}/android/captures"
    "${CMAKE_CURRENT_LIST_DIR}/android/.externalNativeBuild"
    "${CMAKE_CURRENT_LIST_DIR}/android/.cxx"
    "${CMAKE_CURRENT_LIST_DIR}/android/out"
    "${CMAKE_CURRENT_LIST_DIR}/android/app/build"
    "${CMAKE_CURRENT_LIST_DIR}/android/app/.cxx"
    "${CMAKE_CURRENT_LIST_DIR}/android/app/.externalNativeBuild"
    "${CMAKE_CURRENT_LIST_DIR}/android/app/out"
)

# LOOP AND DELETE IF EXISTS
foreach(DIRECTORY IN LISTS GENERATED_DIRECTORIES)
    if(EXISTS "${DIRECTORY}")
        file(REMOVE_RECURSE "${DIRECTORY}")

        if(EXISTS "${DIRECTORY}")
            message(WARNING "Could not fully remove: ${DIRECTORY}")
        else()
            message(STATUS "Removed: ${DIRECTORY}")
        endif()
    endif()
endforeach()

# LOCAL CONFIGURATION AND GENERATED ANDROID FILES
set(GENERATED_FILES
    "${CMAKE_CURRENT_LIST_DIR}/android/local.properties"
)

file(GLOB_RECURSE GENERATED_ANDROID_FILES
    LIST_DIRECTORIES FALSE
    "${CMAKE_CURRENT_LIST_DIR}/android/*.apk"
    "${CMAKE_CURRENT_LIST_DIR}/android/*.aab"
    "${CMAKE_CURRENT_LIST_DIR}/android/*.ap_"
    "${CMAKE_CURRENT_LIST_DIR}/android/*.dex"
    "${CMAKE_CURRENT_LIST_DIR}/android/*.class"
    "${CMAKE_CURRENT_LIST_DIR}/android/*.iml"
)
list(APPEND GENERATED_FILES ${GENERATED_ANDROID_FILES})

foreach(FILE_PATH IN LISTS GENERATED_FILES)
    if(EXISTS "${FILE_PATH}")
        file(REMOVE "${FILE_PATH}")

        if(EXISTS "${FILE_PATH}")
            message(WARNING "Could not remove: ${FILE_PATH}")
        else()
            message(STATUS "Removed: ${FILE_PATH}")
        endif()
    endif()
endforeach()

message(STATUS "Cross-platform cleanup complete")
