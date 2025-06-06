# Copyright (c) 2024 Chair for Design Automation, TUM
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

if(NOT TARGET ${MQT_CORE_TARGET_NAME}-zx)
  # make sure the MQT::multiprecision target is available
  if(NOT TARGET MQT::Multiprecision)
    # handle the case for a complete Boost installation
    if(NOT TARGET Boost::multiprecision)
      add_library(multiprecision INTERFACE)
      target_link_libraries(multiprecision INTERFACE Boost::headers)
      add_library(MQT::Multiprecision ALIAS multiprecision)
      set(MQT_CORE_ZX_SYSTEM_BOOST
          TRUE
          CACHE BOOL "Whether a system version of Boost was used")
    else()
      # Boost::multiprecision does not provide its own installation instructions. As a consequence,
      # this needs a workaround.

      # first, we get the include directory of the multiprecision library
      get_target_property(MULTIPRECISION_INCLUDE_DIR Boost::multiprecision
                          INTERFACE_INCLUDE_DIRECTORIES)

      # then, we create a target that will be used to export the include directory
      add_library(multiprecision INTERFACE)
      target_include_directories(
        multiprecision SYSTEM INTERFACE $<BUILD_INTERFACE:${MULTIPRECISION_INCLUDE_DIR}>
                                        $<INSTALL_INTERFACE:${MQT_CORE_INCLUDE_INSTALL_DIR}>)
      add_library(MQT::Multiprecision ALIAS multiprecision)

      if(MQT_CORE_INSTALL)
        # finally, we create install instructions for the respective header files
        install(DIRECTORY ${MULTIPRECISION_INCLUDE_DIR}/boost
                DESTINATION ${MQT_CORE_INCLUDE_INSTALL_DIR})
      endif()

      set(MQT_CORE_ZX_SYSTEM_BOOST
          FALSE
          CACHE BOOL "Whether a system version of Boost was used")
    endif()
  endif()

  # collect headers and source files
  file(GLOB_RECURSE ZX_HEADERS ${MQT_CORE_INCLUDE_BUILD_DIR}/zx/*.hpp)
  file(GLOB_RECURSE ZX_SOURCES **.cpp)

  # add ZX package library
  add_library(${MQT_CORE_TARGET_NAME}-zx ${ZX_HEADERS} ${ZX_SOURCES})

  # add link libraries
  target_link_libraries(
    ${MQT_CORE_TARGET_NAME}-zx
    PUBLIC MQT::CoreIR MQT::Multiprecision
    PRIVATE MQT::ProjectOptions MQT::ProjectWarnings)

  option(MQT_CORE_WITH_GMP "Whether to use GMP for multiprecision arithmetic" OFF)
  if(MQT_CORE_WITH_GMP)
    find_package(GMP REQUIRED)
    # link to GMP libraries if present
    target_compile_definitions(${MQT_CORE_TARGET_NAME}-zx PUBLIC GMP)
    target_link_libraries(${MQT_CORE_TARGET_NAME}-zx PUBLIC GMP::gmp GMP::gmpxx)
  endif()

  # set include directories
  target_include_directories(
    ${MQT_CORE_TARGET_NAME}-zx PUBLIC $<BUILD_INTERFACE:${MQT_CORE_INCLUDE_BUILD_DIR}>
                                      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

  # add MQT alias
  add_library(MQT::CoreZX ALIAS ${MQT_CORE_TARGET_NAME}-zx)

  # set versioning information
  set_target_properties(
    ${MQT_CORE_TARGET_NAME}-zx
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               EXPORT_NAME CoreZX)

  # generate export header
  include(GenerateExportHeader)
  generate_export_header(${MQT_CORE_TARGET_NAME}-zx BASE_NAME mqt_core_zx)
  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${MQT_CORE_TARGET_NAME}-zx PUBLIC MQT_CORE_ZX_STATIC_DEFINE)
  endif()

  # install export header
  if(MQT_CORE_INSTALL)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mqt_core_zx_export.h
            DESTINATION ${MQT_CORE_INCLUDE_INSTALL_DIR})
  endif()

  # add to list of MQT core targets
  set(MQT_CORE_TARGETS
      ${MQT_CORE_TARGETS} ${MQT_CORE_TARGET_NAME}-zx multiprecision
      PARENT_SCOPE)
endif()
