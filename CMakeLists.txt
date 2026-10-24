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

project(ResoPrint LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if(NOT WIN32)
    message(FATAL_ERROR "ResoPrint AEX build requires a Windows x64 target (MSVC or MinGW-w64).")
endif()

enable_language(RC)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_subdirectory(extern/AfterEffectsSDK)

set(AE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ae")
set(CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core")
set(PIPL_R "${AE_DIR}/ResoPrint_PiPL.r")
set(PIPL_BIN "${CMAKE_CURRENT_BINARY_DIR}/ResoPrint_PiPL.bin")
set(PIPL_RC "${CMAKE_CURRENT_BINARY_DIR}/ResoPrint_PiPL.rc")
set(PIPL_PY "${AE_DIR}/pipl_gen.py")
set(VERSION_RC "${AE_DIR}/ResoPrint_Version.rc")
set(DEF_FILE "${AE_DIR}/ResoPrint.def")

add_custom_command(OUTPUT "${PIPL_RC}" "${PIPL_BIN}"
    COMMAND "${Python3_EXECUTABLE}" "${PIPL_PY}" "${PIPL_R}" "${PIPL_RC}" "${PIPL_BIN}"
    DEPENDS "${PIPL_R}" "${PIPL_PY}"
    COMMENT "Generating ResoPrint PiPL resource"
    VERBATIM)

add_library(ResoPrint SHARED
    "${PIPL_RC}"
    "${AE_DIR}/resoprint_ae.cpp"
    "${CORE_DIR}/resoprint_core.cpp"
    "${VERSION_RC}")

target_include_directories(ResoPrint PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" "${AE_DIR}" "${CORE_DIR}" ${AESDK_INCLUDE})
target_link_libraries(ResoPrint PRIVATE AESDK)
target_compile_definitions(ResoPrint PRIVATE MSWindows _WINDOWS WIN32 _WIN32 _WIN64 AE_OS_WIN _CRT_SECURE_NO_WARNINGS)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_options(ResoPrint PRIVATE /W3 /EHsc /MT /O2 /fp:fast)
    target_link_options(ResoPrint PRIVATE /SUBSYSTEM:WINDOWS,6.00 /OSVERSION:6.00 /VERSION:6.00 /MANIFEST:NO /DEF:${DEF_FILE})
else()
    target_compile_options(ResoPrint PRIVATE -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-missing-field-initializers -Wno-unknown-pragmas -O3)
    target_link_options(ResoPrint PRIVATE "${DEF_FILE}" -static -static-libgcc -static-libstdc++ -Wl,--exclude-all-symbols -Wl,--kill-at -Wl,-s -Wl,--subsystem,windows:6.0 -Wl,--major-os-version,6 -Wl,--minor-os-version,0 -Wl,--major-image-version,6 -Wl,--minor-image-version,0 -Wl,--major-subsystem-version,6 -Wl,--minor-subsystem-version,0 -Wl,--image-base,0x180000000 -Wl,--disable-auto-image-base)
endif()

set_target_properties(ResoPrint PROPERTIES PREFIX "" OUTPUT_NAME "ResoPrint" SUFFIX ".aex" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug" RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release" LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug" LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release")

add_custom_command(TARGET ResoPrint POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/public"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/public/dist"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/dist"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:ResoPrint>" "${CMAKE_BINARY_DIR}/ResoPrint.aex"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:ResoPrint>" "${CMAKE_CURRENT_SOURCE_DIR}/public/ResoPrint.aex"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:ResoPrint>" "${CMAKE_CURRENT_SOURCE_DIR}/public/dist/ResoPrint.aex"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:ResoPrint>" "${CMAKE_CURRENT_SOURCE_DIR}/dist/ResoPrint.aex"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:ResoPrint>" "${CMAKE_CURRENT_SOURCE_DIR}/ResoPrint.aex"
    COMMENT "Packaging ResoPrint.aex")

add_custom_target(RESOPRINT DEPENDS ResoPrint)
add_custom_target(ResoPrint_AEX DEPENDS ResoPrint)
