# Copyright (c) 2024 Chair for Design Automation, TUM
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

#
# This file is part of the MQT CORE library released under the MIT license. See README.md or go to
# https://github.com/cda-tum/mqt-core for more information.
#

if(NOT TARGET MQT::CoreIR)
  # collect headers and source files
  file(GLOB_RECURSE IR_HEADERS ${MQT_CORE_INCLUDE_BUILD_DIR}/ir/*.hpp)
  file(GLOB_RECURSE IR_SOURCES **.cpp)

  # add IR Package library
  add_library(${MQT_CORE_TARGET_NAME}-ir ${IR_HEADERS} ${IR_SOURCES})

  # add link libraries
  target_link_libraries(${MQT_CORE_TARGET_NAME}-ir PRIVATE MQT::ProjectOptions MQT::ProjectWarnings)

  # set include directories
  target_include_directories(
    ${MQT_CORE_TARGET_NAME}-ir PUBLIC $<BUILD_INTERFACE:${MQT_CORE_INCLUDE_BUILD_DIR}>
                                      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

  # add MQT alias
  add_library(MQT::CoreIR ALIAS ${MQT_CORE_TARGET_NAME}-ir)

  # set versioning information
  set_target_properties(
    ${MQT_CORE_TARGET_NAME}-ir
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               EXPORT_NAME CoreIR)

  # generate export header
  include(GenerateExportHeader)
  generate_export_header(${MQT_CORE_TARGET_NAME}-ir BASE_NAME mqt_core_ir)
  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${MQT_CORE_TARGET_NAME}-ir PUBLIC MQT_CORE_IR_STATIC_DEFINE)
  endif()

  # install export header
  if(MQT_CORE_INSTALL)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mqt_core_ir_export.h
            DESTINATION ${MQT_CORE_INCLUDE_INSTALL_DIR})
  endif()

  # add to list of MQT core target
  set(MQT_CORE_TARGETS
      ${MQT_CORE_TARGETS} ${MQT_CORE_TARGET_NAME}-ir
      PARENT_SCOPE)
endif()
