# Copyright (c) 2024 Chair for Design Automation, TUM
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

set(MQT_CORE_TARGET_NAME "mqt-core")
set(MQT_CORE_TARGETS "")

if(MQT_CORE_INSTALL)
  include(GNUInstallDirs)
  set(MQT_CORE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/mqt-core")
  set(MQT_CORE_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/mqt-core")
  set(MQT_CORE_TARGETS_EXPORT_NAME "mqt-core-targets")
  set(MQT_CORE_CMAKE_CONFIG_TEMPLATE "${PROJECT_SOURCE_DIR}/cmake/mqt-core-config.cmake.in")
  set(MQT_CORE_CMAKE_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  set(MQT_CORE_CMAKE_VERSION_CONFIG_FILE
      "${MQT_CORE_CMAKE_CONFIG_DIR}/mqt-core-config-version.cmake")
  set(MQT_CORE_CMAKE_PROJECT_CONFIG_FILE "${MQT_CORE_CMAKE_CONFIG_DIR}/mqt-core-config.cmake")
  set(MQT_CORE_CMAKE_PROJECT_TARGETS_FILE "${MQT_CORE_CMAKE_CONFIG_DIR}/mqt-core-targets.cmake")

  if(APPLE)
    set(BASEPOINT @loader_path)
  else()
    set(BASEPOINT $ORIGIN)
  endif()
  set(CMAKE_INSTALL_RPATH ${BASEPOINT} ${BASEPOINT}/${CMAKE_INSTALL_LIBDIR})
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

if(NOT TARGET MQT::ProjectWarnings)
  # use the warnings specified in CompilerWarnings.cmake
  add_library(project_warnings INTERFACE)

  # standard compiler warnings
  include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
  set_project_warnings(project_warnings)

  # add MQT alias
  add_library(MQT::ProjectWarnings ALIAS project_warnings)
  set_target_properties(project_warnings PROPERTIES EXPORT_NAME ProjectWarnings)

  # add to list of MQT core targets
  list(APPEND MQT_CORE_TARGETS project_warnings)
endif()

if(NOT TARGET MQT::ProjectOptions)
  # use the options specified in CompilerOptions.cmake
  add_library(project_options INTERFACE)

  # standard compiler options
  include(${PROJECT_SOURCE_DIR}/cmake/CompilerOptions.cmake)
  enable_project_options(project_options)

  # sanitizer options if supported by compiler
  include(${PROJECT_SOURCE_DIR}/cmake/Sanitizers.cmake)
  enable_sanitizers(project_options)

  # add MQT alias
  add_library(MQT::ProjectOptions ALIAS project_options)
  set_target_properties(project_options PROPERTIES EXPORT_NAME ProjectOptions)

  # add to list of MQT core targets
  list(APPEND MQT_CORE_TARGETS project_options)
endif()

# add the IR package
add_subdirectory(ir)

# add the algorithms package
add_subdirectory(algorithms)

# add the circuit_optimizer package
add_subdirectory(circuit_optimizer)

# add datastructures package
add_subdirectory(datastructures)

# add DD package library
add_subdirectory(dd)

# add ZX package library
add_subdirectory(zx)

# add NA library
add_subdirectory(na)

# ** Note ** The following target will soon be removed from the project. All top-level projects
# should switch to using the mqt-core Python package.
if(BINDINGS AND NOT TARGET mqt-core-python)
  # add Python interface library
  add_library(
    ${MQT_CORE_TARGET_NAME}-python ${MQT_CORE_INCLUDE_BUILD_DIR}/python/qiskit/QuantumCircuit.hpp
                                   python/qiskit/QuantumCircuit.cpp)

  # link with main project library and pybind11 libraries
  target_link_libraries(
    ${MQT_CORE_TARGET_NAME}-python
    PUBLIC MQT::CoreIR pybind11::pybind11
    PRIVATE MQT::ProjectOptions MQT::ProjectWarnings)

  # add MQT alias
  add_library(MQT::CorePython ALIAS ${MQT_CORE_TARGET_NAME}-python)
  set_target_properties(
    ${MQT_CORE_TARGET_NAME}-python
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               EXPORT_NAME CorePython)
  list(APPEND MQT_CORE_TARGETS ${MQT_CORE_TARGET_NAME}-python)
endif()

if(BUILD_MQT_CORE_BINDINGS)
  add_subdirectory(python)
endif()

# Installation instructions for the main library
if(MQT_CORE_INSTALL)
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    ${MQT_CORE_CMAKE_CONFIG_TEMPLATE} ${MQT_CORE_CMAKE_PROJECT_CONFIG_FILE}
    INSTALL_DESTINATION ${MQT_CORE_CONFIG_INSTALL_DIR}
    PATH_VARS MQT_CORE_WITH_GMP
    NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)
  write_basic_package_version_file(
    ${MQT_CORE_CMAKE_VERSION_CONFIG_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)

  install(FILES ${MQT_CORE_CMAKE_PROJECT_CONFIG_FILE} ${MQT_CORE_CMAKE_VERSION_CONFIG_FILE}
          DESTINATION ${MQT_CORE_CONFIG_INSTALL_DIR})

  install(
    TARGETS ${MQT_CORE_TARGETS}
    EXPORT ${MQT_CORE_TARGETS_EXPORT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${MQT_CORE_TARGET_NAME}_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${MQT_CORE_TARGET_NAME}_Runtime
            NAMELINK_COMPONENT ${MQT_CORE_TARGET_NAME}_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${MQT_CORE_TARGET_NAME}_Development
    INCLUDES
    DESTINATION ${MQT_CORE_INCLUDE_INSTALL_DIR})

  install(DIRECTORY ${MQT_CORE_INCLUDE_BUILD_DIR}/ DESTINATION ${MQT_CORE_INCLUDE_INSTALL_DIR})

  install(
    EXPORT ${MQT_CORE_TARGETS_EXPORT_NAME}
    FILE ${MQT_CORE_PROJECT_TARGETS_FILE}
    NAMESPACE MQT::
    DESTINATION ${MQT_CORE_CONFIG_INSTALL_DIR}
    COMPONENT ${MQT_CORE_TARGET_NAME}_Development)

  install(
    FILES ${PROJECT_SOURCE_DIR}/cmake/Cache.cmake
          ${PROJECT_SOURCE_DIR}/cmake/FindGMP.cmake
          ${PROJECT_SOURCE_DIR}/cmake/PackageAddTest.cmake
          ${PROJECT_SOURCE_DIR}/cmake/PreventInSourceBuilds.cmake
          ${PROJECT_SOURCE_DIR}/cmake/StandardProjectSettings.cmake
    DESTINATION ${MQT_CORE_CONFIG_INSTALL_DIR})
endif()
