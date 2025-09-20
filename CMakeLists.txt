cmake_minimum_required(VERSION 3.31)

project(
	libism
	VERSION 0.1.0
	DESCRIPTION "Tiny system monitoring library (CPU/MEM/DISK)"
	LANGUAGES C
)

include(GNUInstallDirs)

option(LIBISM_BUILD_SHARED "Build shared libism (.so) instead of static (.a)" OFF)

set(
	LIBISM_SOURCES
	src/cpu/cpu.c
	src/mem/mem.c
	src/disk/disk.c
)

if(ISM_BUILD_SHARED)
  add_library(libism SHARED ${LIBISM_SOURCES})
else()
  add_library(libism STATIC ${LIBISM_SOURCES})
endif()

add_library(libism::libism ALIAS libism)

set_target_properties(
  libism PROPERTIES 
  C_STANDARD 99 #idk what standart tbh
  C_STANDARD_REQUIRED OFF #thats why OFF
  OUTPUT_NAME "ism"
  VERSION ${PROJECT_VERSION}
)

target_include_directories(
	libism
	PUBLIC
    	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	PRIVATE
    	${PROJECT_SOURCE_DIR}/src
)

target_compile_options(
	libism PRIVATE
	-Wall -Wextra -Werror -O2
)

install(
	TARGETS libism
	EXPORT libismTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
	EXPORT libismTargets
	NAMESPACE libism::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libism
)

