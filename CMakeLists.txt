cmake_minimum_required(VERSION 3.20)

if(NOT WIN32 AND NOT CMAKE_SYSTEM_NAME)
    find_program(MINGW_GCC NAMES x86_64-w64-mingw32-gcc-win32 x86_64-w64-mingw32-gcc x86_64-w64-mingw32-gcc-posix)
    find_program(MINGW_GXX NAMES x86_64-w64-mingw32-g++-win32 x86_64-w64-mingw32-g++ x86_64-w64-mingw32-g++-posix)
    find_program(MINGW_WINDRES NAMES x86_64-w64-mingw32-windres)
    if(MINGW_GCC AND MINGW_GXX AND MINGW_WINDRES)
        set(CMAKE_SYSTEM_NAME Windows)
        set(CMAKE_SYSTEM_PROCESSOR x86_64)
        set(CMAKE_C_COMPILER "${MINGW_GCC}")
        set(CMAKE_CXX_COMPILER "${MINGW_GXX}")
        set(CMAKE_RC_COMPILER "${MINGW_WINDRES}")
    endif()
endif()

project(risoprint LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if(NOT WIN32)
    message(FATAL_ERROR "risoprint AEX build requires a Windows x64 target (MSVC or MinGW-w64).")
endif()

enable_language(RC)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_subdirectory(extern/AfterEffectsSDK)

# Keep this build layout intentionally parallel to the known-good
# Dithering_MAJEED_build_ready repository. The CMake target itself is the
# plugin target; there is NO custom target with the same name.
set(RISOPRINT_AE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/ae")
set(RISOPRINT_CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core")
set(PIPL_R             "${RISOPRINT_AE_DIR}/risoprint_PiPL.r")
set(PIPL_BIN           "${CMAKE_CURRENT_BINARY_DIR}/risoprint_PiPL.bin")
set(PIPL_RC            "${CMAKE_CURRENT_BINARY_DIR}/risoprint_PiPL.rc")
set(PIPL_PY            "${RISOPRINT_AE_DIR}/pipl_gen.py")
set(RISOPRINT_VERSION_RC "${RISOPRINT_AE_DIR}/risoprint_Version.rc")
set(RISOPRINT_DEF      "${RISOPRINT_AE_DIR}/risoprint.def")

if(NOT EXISTS "${PIPL_R}")
    message(FATAL_ERROR "PiPL source not found: ${PIPL_R}")
endif()

# Same direct PiPL generation model as the reference repository.
add_custom_command(
    OUTPUT "${PIPL_RC}" "${PIPL_BIN}"
    COMMAND "${Python3_EXECUTABLE}" "${PIPL_PY}" "${PIPL_R}" "${PIPL_RC}" "${PIPL_BIN}"
    DEPENDS "${PIPL_R}" "${PIPL_PY}"
    COMMENT "Generating risoprint_PiPL.rc and risoprint_PiPL.bin via pipl_gen.py"
    VERBATIM
)

file(GLOB RISOPRINT_AE_SOURCES CONFIGURE_DEPENDS "${RISOPRINT_AE_DIR}/*.cpp")
file(GLOB RISOPRINT_CORE_SOURCES CONFIGURE_DEPENDS "${RISOPRINT_CORE_DIR}/*.cpp")

# IMPORTANT: risoprint is the actual CMake library target, just as
# YMDithers is in the reference. Do not create another custom target named
# risoprint; doing so creates a circular ResolveProjectReferences graph in
# Visual Studio/MSBuild.
add_library(risoprint SHARED
    "${PIPL_RC}"
    ${RISOPRINT_AE_SOURCES}
    ${RISOPRINT_CORE_SOURCES}
)

target_include_directories(risoprint PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${RISOPRINT_AE_DIR}"
    "${RISOPRINT_CORE_DIR}"
    ${AESDK_INCLUDE}
)

target_link_libraries(risoprint PRIVATE AESDK)

target_compile_definitions(risoprint PRIVATE
    MSWindows
    _WINDOWS
    WIN32
    _WIN32
    _WIN64
    AE_OS_WIN
    _CRT_SECURE_NO_WARNINGS
)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_options(risoprint PRIVATE /W3 /EHsc /MT /O2 /fp:fast)
    target_link_options(risoprint PRIVATE
        /SUBSYSTEM:WINDOWS,6.00
        /OSVERSION:6.00
        /VERSION:6.00
        /MANIFEST:NO
        /DEF:${RISOPRINT_DEF}
    )
else()
    target_compile_options(risoprint PRIVATE -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-missing-field-initializers -Wno-unknown-pragmas -O3)
    target_link_options(risoprint PRIVATE
        "${RISOPRINT_DEF}"
        -static -static-libgcc -static-libstdc++
        -Wl,--exclude-all-symbols
        -Wl,--kill-at -Wl,-s
        -Wl,--subsystem,windows:6.0
        -Wl,--major-os-version,6 -Wl,--minor-os-version,0
        -Wl,--major-image-version,6 -Wl,--minor-image-version,0
        -Wl,--major-subsystem-version,6 -Wl,--minor-subsystem-version,0
        -Wl,--image-base,0x180000000 -Wl,--disable-auto-image-base
    )
endif()

set_target_properties(risoprint PROPERTIES
    PREFIX ""
    OUTPUT_NAME "risoprint"
    SUFFIX ".aex"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

add_custom_command(TARGET risoprint POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/public"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/public/dist"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/dist"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:risoprint>"
        "${CMAKE_BINARY_DIR}/risoprint.aex"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:risoprint>"
        "${CMAKE_CURRENT_SOURCE_DIR}/public/risoprint.aex"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:risoprint>"
        "${CMAKE_CURRENT_SOURCE_DIR}/public/dist/risoprint.aex"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:risoprint>"
        "${CMAKE_CURRENT_SOURCE_DIR}/dist/risoprint.aex"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:risoprint>"
        "${CMAKE_CURRENT_SOURCE_DIR}/risoprint.aex"
    COMMENT "Packaging risoprint.aex without post-build PE binary patching"
)

# Keep the same useful alias pattern as the reference, but with names that
# cannot collide with the actual risoprint target.
add_custom_target(RISOPRINT_BUILD DEPENDS risoprint)
add_custom_target(risoprint_AEX DEPENDS risoprint)

set(PLUGIN_FOLDER
    "C:/Program Files/Adobe/Adobe After Effects 2023/Support Files/Plug-ins/"
    CACHE STRING "After Effects plugin folder"
)
install(TARGETS risoprint DESTINATION "${PLUGIN_FOLDER}")
