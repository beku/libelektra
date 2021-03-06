# For static inclusion of libraries we need a list
# of symbols to make the static runtime linker work.
#
# For that there is the program exportsymbols which
# generates the sourcefiles exported_symbols.h
# and exported_symbols.c containing an array of
# all symbols.
#
# The program needs a list of all backends as
# argument.
#
# Symbols of libelektratools will be treaded specially.
add_executable (exportsymbols exportsymbols.c)

include(LibAddMacros)
find_util(exportsymbols EXE_SYM_LOC EXE_SYM_ARG)

add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/exported_symbols.h
		DEPENDS exportsymbols
		COMMAND ${EXE_SYM_LOC}
		ARGS ${EXE_SYM_ARG} ${PLUGINS}
		)

add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/exported_symbols.c
		DEPENDS exportsymbols
		COMMAND ${EXE_SYM_LOC}
		ARGS ${EXE_SYM_ARG} ${PLUGINS}
		)


# Include the shared header files of the elektra project
include(LibAddMacros)
add_headers(HDR_FILES)
# not always needed.. only with cppplugins
add_cppheaders(HDR_FILES)

#include the current binary directory to get exported_symbols.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

#now add all source files of this folder
file (GLOB SRC_FILES *.c)

#remove those we don't want..
file (GLOB RM_FILES exportsymbols.c kdbmainpage.c)
list (REMOVE_ITEM SRC_FILES ${RM_FILES} ${WIN32_RM_FILES})

#now add all source files of other folders
get_property (elektra_SRCS GLOBAL PROPERTY elektra_SRCS)
list (APPEND SRC_FILES ${elektra_SRCS})

set (SOURCES ${SRC_FILES} ${HDR_FILES})
list (APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/exported_symbols.h")

#the targets built to export
set (targets_built)


if (BUILD_SHARED)
	get_property (elektra-shared_SRCS GLOBAL PROPERTY elektra-shared_SRCS)
	add_library (elektra SHARED ${SOURCES} ${elektra-shared_SRCS})

	get_property (elektra-shared_LIBRARIES GLOBAL PROPERTY elektra-shared_LIBRARIES)
	target_link_libraries (elektra ${elektra-shared_LIBRARIES})

	get_property (elektra-shared_INCLUDES GLOBAL PROPERTY elektra-shared_INCLUDES)
	include_directories (${elektra-shared_INCLUDES})

	set_target_properties (elektra PROPERTIES
			COMPILE_DEFINITIONS HAVE_KDBCONFIG_H)
	set_target_properties (elektra PROPERTIES
			VERSION ${KDB_VERSION} SOVERSION ${SO_VERSION})
	set_target_properties (elektra PROPERTIES CLEAN_DIRECT_OUTPUT 1)

	#search only in the prefix the user decided to install elektra
	#(e.g. /usr/local/)
	set (ELEKTRA_RPATH "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/${TARGET_PLUGIN_FOLDER}")

	#RPATH settings, see http://www.itk.org/Wiki/CMake_RPATH_handling
	#those settings are needed to find the plugins at runtime even without ld.so.conf
	set_target_properties ( elektra PROPERTIES INSTALL_RPATH "${ELEKTRA_RPATH}")

	install (TARGETS elektra DESTINATION lib${LIB_SUFFIX} EXPORT ElektraTargetsLibelektra)

	list (APPEND targets_built elektra)
endif (BUILD_SHARED)

if (BUILD_FULL OR BUILD_STATIC)
	#for the next two builds, append some more sources
	get_property (elektra-full_SRCS GLOBAL PROPERTY elektra-full_SRCS)
	list (APPEND SOURCES ${elektra-full_SRCS}
		${BINARY_INCLUDE_DIR}/kdberrors.h)
	list (APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/exported_symbols.c")

	#and get all libraries to link against
	get_property (elektra-full_LIBRARIES GLOBAL PROPERTY elektra-full_LIBRARIES)

	# include the directories of all libraries for the static or
	# full-shared build
	get_property (elektra-full_INCLUDES GLOBAL PROPERTY elektra-full_INCLUDES)

	include_directories (${elektra-full_INCLUDES})
endif (BUILD_FULL OR BUILD_STATIC)


if (BUILD_FULL)
	add_library (elektra-full SHARED ${SOURCES})

	target_link_libraries (elektra-full ${elektra-full_LIBRARIES})

	set_target_properties (elektra-full PROPERTIES
			COMPILE_DEFINITIONS "HAVE_KDBCONFIG_H;ELEKTRA_STATIC")
	set_target_properties (elektra-full PROPERTIES
			VERSION ${KDB_VERSION} SOVERSION ${SO_VERSION})
	set_target_properties (elektra-full PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	set_target_properties (elektra-full PROPERTIES LINKER_LANGUAGE CXX)

	install (TARGETS elektra-full DESTINATION lib${LIB_SUFFIX} EXPORT ElektraTargetsLibelektra)

	list (APPEND targets_built elektra-full)
endif (BUILD_FULL)


if (BUILD_STATIC)
	add_library (elektra-static STATIC ${SOURCES})

	target_link_libraries (elektra-static ${elektra-full_LIBRARIES})

	set_target_properties (elektra-static PROPERTIES
			COMPILE_DEFINITIONS "HAVE_KDBCONFIG_H;ELEKTRA_STATIC")
	set_target_properties (elektra-static PROPERTIES
			VERSION ${KDB_VERSION} SOVERSION ${SO_VERSION})
	set_target_properties (elektra-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	set_target_properties (elektra-static PROPERTIES LINKER_LANGUAGE CXX)

	install (TARGETS elektra-static DESTINATION lib${LIB_SUFFIX} EXPORT ElektraTargetsLibelektra)

	list (APPEND targets_built elektra-static)
endif (BUILD_STATIC)

export (TARGETS ${targets_built} FILE "${CMAKE_CURRENT_BINARY_DIR}/ElektraTargetsLibelektra.cmake")
install (EXPORT ElektraTargetsLibelektra DESTINATION "${TARGET_CMAKE_FOLDER}")

#if you want the same name for those 3 libraries, here is how to do it:
#http://www.itk.org/Wiki/CMake_FAQ#How_do_I_make_my_shared_and_static_libraries_have_the_same_root_name.2C_but_different_suffixes.3F
