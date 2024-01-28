set( HOUDINI_ROOT_USER "C:\\Users\\$ENV{USERNAME}\\Documents\\houdini${HOUDINI_MAJ_MIN}" )

MESSAGE(STATUS "HOUDINI_VERSION = " ${HOUDINI_VERSION} )
MESSAGE(STATUS "HOUDINI_ROOT = " ${HOUDINI_ROOT} )
MESSAGE(STATUS "HOUDINI_ROOT_USER = " ${HOUDINI_ROOT_USER} )
MESSAGE(STATUS "HOUDINI_PYTHON_VERSION = " ${HOUDINI_PYTHON_VERSION} )

# Usd interface
add_library(UsdInterface INTERFACE)
add_library(Usd::Usd ALIAS UsdInterface)

find_package(Houdini REQUIRED PATHS ${HOUDINI_ROOT}/toolkit/cmake)
target_link_libraries(UsdInterface INTERFACE Houdini)

# DSO
target_compile_definitions(UsdInterface
    INTERFACE
    USE_HBOOST
    BOOST_NS=hboost
    Houdini_FOUND=TRUE
)

set(_houdini_libs
    OpenImageIO_sidefx;
    hboost_filesystem-mt-x64;
    hboost_iostreams-mt-x64;
    hboost_system-mt-x64;
    hboost_regex-mt-x64;
    )

foreach(_houdini_lib ${_houdini_libs})
    find_library(${_houdini_lib}_path
            NAMES
            ${_houdini_lib}
            PATHS
            ${HOUDINI_ROOT}/dsolib
            ${HOUDINI_ROOT}/custom/houdini/dsolib/
            REQUIRED
            )

    message(STATUS "Found ${_houdini_lib}: ${${_houdini_lib}_path}")

    target_link_libraries(UsdInterface
            INTERFACE
            ${${_houdini_lib}_path}
            )

endforeach()

# Python
find_library(
    _houdini_python_lib
    NAMES
    python${HOUDINI_PYTHON_VERSION}
    PATHS
    ${HOUDINI_ROOT}/python${HOUDINI_PYTHON_VERSION}/libs
    REQUIRED
    )

find_library(_houdini_hboost_python
        NAMES
        hboost_python${HOUDINI_PYTHON_VERSION}-mt-x64
        PATHS
        ${HOUDINI_ROOT}/dsolib
        ${HOUDINI_ROOT}/custom/houdini/dsolib/
        REQUIRED
        )

target_link_libraries(Houdini INTERFACE ${_houdini_python_lib} ${_houdini_hboost_python})

# Usd
list(APPEND CMAKE_FIND_LIBRARY_PREFIXES lib) # append lib prefix to have same behaviour on win and lin
set(_houdini_pxr_libs pxr_ar;pxr_arch;pxr_cameraUtil;pxr_garch;pxr_gf;pxr_glf;pxr_hd;pxr_hdSt;pxr_hdx;pxr_hf;pxr_hgi;pxr_hgiGL;pxr_hio;pxr_js;pxr_kind;pxr_ndr;pxr_pcp;pxr_plug;pxr_pxOsd;pxr_sdf;pxr_sdr;pxr_tf;pxr_trace;pxr_usd;pxr_usdHydra;pxr_usdImaging;pxr_usdImagingGL;pxr_usdLux;pxr_usdRender;pxr_usdShade;pxr_usdSkel;pxr_usdUtils;pxr_usdVol;pxr_vt;pxr_work;pxr_usdGeom)
foreach(_pxr_lib ${_houdini_pxr_libs})
    find_library(${_pxr_lib}_path
            NAMES
            ${_pxr_lib}
            PATHS
            ${HOUDINI_ROOT}/dsolib
            ${HOUDINI_ROOT}/custom/houdini/dsolib/
            REQUIRED
            )

    target_link_libraries(UsdInterface
            INTERFACE
            ${${_pxr_lib}_path}
            )

endforeach()

# Find Usd Schema Generator
if (NOT USD_SCHEMA_GENERATOR)
    find_program(USD_SCHEMA_GENERATOR
            NAMES
            usdGenSchema
            usdGenSchema.py
            PATHS
            ${HOUDINI_ROOT}/bin
            ${HOUDINI_ROOT}/Resources/bin
            REQUIRED
            NO_DEFAULT_PATH
            )

    get_filename_component(USD_SCHEMA_GENERATOR_EXT
            ${USD_SCHEMA_GENERATOR}
            EXT
            )

    if("${USD_SCHEMA_GENERATOR_EXT}" STREQUAL ".py")
        find_program(HYTHON_EXECUTABLE
                NAMES
                hython
                PATHS
                ${HOUDINI_ROOT}/bin
                ${HOUDINI_ROOT}/Resources/bin
                REQUIRED
                NO_DEFAULT_PATH
                )
        list(PREPEND USD_SCHEMA_GENERATOR ${HYTHON_EXECUTABLE})
        set(USD_SCHEMA_GENERATOR ${USD_SCHEMA_GENERATOR} CACHE STRING "" FORCE)
    endif()
endif()


set(USD_LIBS 
    Usd::Usd
)