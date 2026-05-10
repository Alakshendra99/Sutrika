cmake_minimum_required(VERSION 3.15)
project(sutrika)
# Remove MSVC-only linker flags injected by cmake-js
if(MINGW)
    string(REPLACE "/DELAYLOAD:NODE.EXE" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ELECTRON_EXECUTABLE)
    file(GLOB ELECTRON_EXE "${CMAKE_SOURCE_DIR}/node_modules/electron/dist/electron.exe")
    if(ELECTRON_EXE)
        set(ELECTRON_EXECUTABLE ${ELECTRON_EXE})
    else()
        message(FATAL_ERROR "Cannot find electron.exe")
    endif()
endif()

set(NODE_IMPLIB "${CMAKE_BINARY_DIR}/libelectron.a")
if(MINGW AND NOT EXISTS ${NODE_IMPLIB})
    find_program(GENDEF_EXE gendef REQUIRED)
    find_program(DLLTOOL_EXE dlltool REQUIRED)
    set(NODE_DEF "${CMAKE_BINARY_DIR}/electron.def")
    add_custom_command(
        OUTPUT ${NODE_DEF}
        COMMAND ${GENDEF_EXE} - ${ELECTRON_EXECUTABLE} > ${NODE_DEF}
        DEPENDS ${ELECTRON_EXECUTABLE}
        COMMENT "Generating electron.def"
    )
    add_custom_command(
        OUTPUT ${NODE_IMPLIB}
        COMMAND ${DLLTOOL_EXE} -d ${NODE_DEF} -l ${NODE_IMPLIB}
        DEPENDS ${NODE_DEF}
        COMMENT "Creating libelectron.a"
    )
    add_custom_target(electron_implib DEPENDS ${NODE_IMPLIB})
else()
    add_custom_target(electron_implib)
endif()

set(PCAN_DLL "${CMAKE_SOURCE_DIR}/API/External/PCANBasic.dll")
set(PCAN_IMPLIB "${CMAKE_BINARY_DIR}/libpcanbasic.a")
if(MINGW AND NOT EXISTS ${PCAN_IMPLIB})
    set(PCAN_DEF "${CMAKE_BINARY_DIR}/pcanbasic.def")
    add_custom_command(
        OUTPUT ${PCAN_DEF}
        COMMAND ${GENDEF_EXE} - ${PCAN_DLL} > ${PCAN_DEF}
        DEPENDS ${PCAN_DLL}
        COMMENT "Generating PCANBasic.def"
    )
    add_custom_command(
        OUTPUT ${PCAN_IMPLIB}
        COMMAND ${DLLTOOL_EXE} -d ${PCAN_DEF} -l ${PCAN_IMPLIB}
        DEPENDS ${PCAN_DEF}
        COMMENT "Creating libpcanbasic.a"
    )
    add_custom_target(pcan_implib DEPENDS ${PCAN_IMPLIB})
else()
    add_custom_target(pcan_implib)
endif()

set(COMMON_INCLUDES
    ${CMAKE_JS_INC}
    ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api
    ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api/include
)

if(NOT CMAKE_JS_INC)
    set(NODE_INCLUDE_PATHS
        "C:/Users/${USERNAME}/.node-gyp/${CMAKE_JS_VERSION}/include/node"
        "C:/Program Files/nodejs/include/node"
        "$ENV{APPDATA}/../Local/node-gyp/Cache/${CMAKE_JS_VERSION}/include/node"
    )
    find_path(NODE_INCLUDE_PATH NAMES node_api.h PATHS ${NODE_INCLUDE_PATHS} NO_DEFAULT_PATH)
    if(NODE_INCLUDE_PATH)
        set(CMAKE_JS_INC ${NODE_INCLUDE_PATH})
    else()
        message(WARNING "Could not find node_api.h - set CMAKE_JS_INC manually")
    endif()
endif()

add_library(Sutrika_Lib STATIC
    API/Static/Sutrika.cpp
)
target_include_directories(Sutrika_Lib PUBLIC
    ${CMAKE_SOURCE_DIR}/API/Static
    ${CMAKE_SOURCE_DIR}/API/External
)

# sutrika.node
add_library(Sutrika_AddOn SHARED
    API/Sutrika_API.cpp
)
set_target_properties(Sutrika_AddOn PROPERTIES
    PREFIX ""
    SUFFIX ".node"
    OUTPUT_NAME "sutrika"
)
target_include_directories(Sutrika_AddOn PRIVATE
    ${COMMON_INCLUDES}
    ${CMAKE_SOURCE_DIR}/API/Static
    ${CMAKE_SOURCE_DIR}/API/External
)
target_link_libraries(Sutrika_AddOn PRIVATE
    Sutrika_Lib
    ${PCAN_IMPLIB}
    ${NODE_IMPLIB}
)
add_dependencies(Sutrika_AddOn pcan_implib electron_implib)
target_compile_definitions(Sutrika_AddOn PRIVATE
    NAPI_CPP_EXCEPTIONS
)

# Static MiniGW Linking
if(MINGW)
    target_link_options(Sutrika_AddOn PRIVATE
        -static-libgcc
        -static-libstdc++
    )
endif()


# Adding DLLs
add_custom_command(TARGET Sutrika_AddOn POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PCAN_DLL}"
        "$<TARGET_FILE_DIR:Sutrika_AddOn>"
    COMMENT "Copying PCANBasic.dll to Output Folder"
)
set(THREAD_DLL "${CMAKE_SOURCE_DIR}/API/External/libwinpthread-1.dll")
add_custom_command(TARGET Sutrika_AddOn POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THREAD_DLL}"
        "$<TARGET_FILE_DIR:Sutrika_AddOn>"
    COMMENT "Copying libwinpthread-1.dll to Output Folder"
)