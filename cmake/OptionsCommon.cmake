ADD_DEFINITIONS(-DBUILDING_WITH_CMAKE=1)

SET(WTF_INCLUDE_DIRECTORIES
    "${JAVASCRIPTCORE_DIR}"
    "${JAVASCRIPTCORE_DIR}/wtf"
    "${JAVASCRIPTCORE_DIR}/wtf/unicode"
    "${DERIVED_SOURCES_DIR}"
)

SET(WEBKIT_LIBRARY_TYPE STATIC)

IF (ICU_FOUND)
    SET(WTF_USE_ICU_UNICODE 1)
    ADD_DEFINITIONS(-DWTF_USE_ICU_UNICODE=1)
    LIST(APPEND WTF_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIRS})
ENDIF ()

IF (CMAKE_HAVE_PTHREAD_H AND NOT WTF_PLATFORM_QT)
    SET(WTF_USE_PTHREADS 1)
    ADD_DEFINITIONS(-DWTF_USE_PTHREADS=1)
ELSE ()
    ADD_DEFINITIONS(-DENABLE_JSC_MULTIPLE_THREADS=0)
ENDIF ()

IF (WTF_OS_UNIX)
    ADD_DEFINITIONS(-DXP_UNIX)
ENDIF (WTF_OS_UNIX)

ADD_DEFINITIONS(-DHAVE_CONFIG_H=1)

FILE(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR})
