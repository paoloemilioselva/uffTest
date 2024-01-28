cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET "uffTest")
add_definitions(-DNOMINMAX)
add_definitions(-DTBB_USE_DEBUG)

project( 
    ${TARGET}
    LANGUAGES CXX C
)

# check target config
if(NOT TARGET_CONFIG)
    set( TARGET_CONFIG "usd-23.11")
endif()

string(REPLACE "-" ";" _t_config ${TARGET_CONFIG})
list(LENGTH _t_config _t_config_length)
if(_t_config_length LESS 2)
    MESSAGE(FATAL_ERROR "Wrong TARGET_CONFIG value " ${TARGET_CONFIG} )
endif()

list(GET _t_config 0 _target_name )

set( USD_LIBS "" )
if( _target_name MATCHES "houdini" )
    list(GET _t_config 1 HOUDINI_MAJ_MIN )
    list(GET _t_config 2 HOUDINI_PATCH )
    list(GET _t_config 3 HOUDINI_PYTHON_VERSION )
    set( HOUDINI_VERSION "Houdini ${HOUDINI_MAJ_MIN}.${HOUDINI_PATCH}" )
    set( HOUDINI_ROOT "C:\\Program Files\\Side Effects Software\\${HOUDINI_VERSION}" )
elseif( _target_name MATCHES "usd" )
    list(GET _t_config 1 USD_VERSION )
    set(USD_ROOT "C:\\dev\\usd-${USD_VERSION}" )
else()
    MESSAGE(FATAL_ERROR "Invalid target " ${_target_name})
endif()
include(cmake/FindUsd.cmake)

include(cmake/FindTBB.cmake)

add_library( ${TARGET} SHARED
    uffTest.cpp
    uffTest.h
)

target_compile_options( 
    ${TARGET}
    PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /O2 /wd4273 /Zi /experimental:external /external:W0>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:/Ob0 /Od> 
)

target_link_options( 
    ${TARGET}
    PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/ignore:4217 /ignore:4049> 
)

target_link_directories( 
    ${TARGET}
    PRIVATE 
    ${USD_LIBRARY_DIR}
)

target_link_libraries(
    ${TARGET}
    PUBLIC
    ${USD_LIBS}
)

set(_installation_folder "")
if( _target_name MATCHES "houdini" )
    set(_installation_folder ${HOUDINI_ROOT_USER}/${TARGET})
    configure_file(houdini-extra/plugInfo.json plugInfo.json @ONLY)
    configure_file(houdini-extra/${TARGET}.json ${TARGET}.json @ONLY)
    install(TARGETS ${TARGET} DESTINATION ${_installation_folder}/dso/usd_plugins)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.json DESTINATION ${HOUDINI_ROOT_USER}/packages)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json DESTINATION ${_installation_folder}/dso/usd_plugins)

elseif( _target_name MATCHES "usd" )
    set(_installation_folder bin)
    install(TARGETS ${TARGET} RUNTIME DESTINATION ${_installation_folder}/usd-extra/plugin/usd)
    install(FILES ${CMAKE_SOURCE_DIR}/usd-extra/plugInfo.json DESTINATION ${_installation_folder}/usd-extra/plugin/usd/${TARGET}/resources )
endif()