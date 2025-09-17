cmake_minimum_required(VERSION 3.31)

project(
	ism
	VERSION 0.1.0
	DESCRIPTION "Tiny system monitoring library (CPU/MEM/DISK)"
	LANGUAGES C
)

include(GNUInstallDirs)

option(ISM_BUILD_SHARED "Build shared libism (.so) instead of static (.a)" OFF)

set(
	ISM_SOURCES
	src/cpu/cpu.c
	src/mem/mem.c
	src/disk/disk.c
)

if(ISM_BUILD_SHARED)
  add_library(ism SHARED ${ISM_SOURCES})
else()
  add_library(ism STATIC ${ISM_SOURCES})
endif()

add_library(ism::ism ALIAS ism)

set_target_properties(
  ism PROPERTIES 
  C_STANDARD 17 #idk what standart tbh
  C_STANDARD_REQUIRED OFF #thats why OFF
  OUTPUT_NAME "ism"
  VERSION ${PROJECT_VERSION}
)

target_include_directories(
	ism
	PUBLIC
    	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>   # для сборки из исходников
    	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>   # для потребителей после install
	PRIVATE
    	${PROJECT_SOURCE_DIR}/src
)

target_compile_options(
	ism PRIVATE
	-Wall -Wextra -Werror -O2
)

install(
	TARGETS ism
	EXPORT ismTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
	EXPORT ismTargets
	NAMESPACE ism::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ism
)

