cmake_minimum_required(VERSION 3.15)
project(sutrika)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MINGW)
    set(CMAKE_SHARED_LINKER_FLAGS "")
endif()

if(NOT ELECTRON_EXECUTABLE)
    file(GLOB ELECTRON_EXE "${CMAKE_SOURCE_DIR}/node_modules/electron/dist/electron.exe")
    if(ELECTRON_EXE)
        set(ELECTRON_EXECUTABLE ${ELECTRON_EXE})
    else()
        message(FATAL_ERROR "Cannot Find electron.exe")
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

# TimeKeeper Static Library
add_library(timekeeper_lib STATIC API/Static/TimeKeeper.cpp)
target_include_directories(timekeeper_lib PUBLIC ${CMAKE_SOURCE_DIR}/API/Static)

# logger.node
add_library(logger_lib STATIC
    API/Static/Logger.cpp
)
target_include_directories(logger_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/API/Static
)
add_library(logger_addon SHARED
    API/Logger_API.cpp
)
set_target_properties(logger_addon PROPERTIES
    PREFIX ""
    SUFFIX ".node"
    OUTPUT_NAME "logger"
)
target_include_directories(logger_addon PRIVATE
    ${COMMON_INCLUDES}
    ${CMAKE_SOURCE_DIR}/API/Static
)
target_link_libraries(logger_addon PRIVATE
    logger_lib
    ${NODE_IMPLIB}
)
add_dependencies(logger_addon electron_implib)
target_compile_definitions(logger_addon PRIVATE
    NAPI_CPP_EXCEPTIONS
)


# pcan.node
add_library(pcan_lib STATIC
    API/Static/PCAN.cpp
)
target_include_directories(pcan_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/API/Static
    ${CMAKE_SOURCE_DIR}/API/External
)
add_library(pcan_addon SHARED
    API/PCAN_API.cpp
)
set_target_properties(pcan_addon PROPERTIES
    PREFIX ""
    SUFFIX ".node"
    OUTPUT_NAME "pcan"
)
target_include_directories(pcan_addon PRIVATE
    ${COMMON_INCLUDES}
    ${CMAKE_SOURCE_DIR}/API/Static
    ${CMAKE_SOURCE_DIR}/API/External
)
target_link_libraries(pcan_addon PRIVATE
    pcan_lib
    ${PCAN_IMPLIB}
    ${NODE_IMPLIB}
)
add_dependencies(pcan_addon pcan_implib electron_implib)
target_compile_definitions(pcan_addon PRIVATE
    NAPI_CPP_EXCEPTIONS
)


# docan.node
add_library(docan_lib STATIC
    API/Static/DoCAN.cpp
)
target_include_directories(docan_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/API/Static
    ${CMAKE_SOURCE_DIR}/API/External
)
target_link_libraries(docan_lib PUBLIC
    logger_lib
    timekeeper_lib
    pcan_lib
    ${PCAN_IMPLIB}
)
add_library(docan_addon SHARED
    API/DoCAN_API.cpp
)
set_target_properties(docan_addon PROPERTIES
    PREFIX ""
    SUFFIX ".node"
    OUTPUT_NAME "docan"
)
target_include_directories(docan_addon PRIVATE
    ${COMMON_INCLUDES}
    ${CMAKE_SOURCE_DIR}/API/Static
    ${CMAKE_SOURCE_DIR}/API/External
)
target_link_libraries(docan_addon PRIVATE
    docan_lib
    ${NODE_IMPLIB}
)
add_dependencies(docan_addon electron_implib)
target_compile_definitions(docan_addon PRIVATE NAPI_CPP_EXCEPTIONS)