# Copyright (c) 2024 Chair for Design Automation, TUM
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

if(TARGET MQT::CoreDD)
  file(GLOB_RECURSE DD_TEST_SOURCES *.cpp)
  package_add_test(mqt-core-dd-test MQT::CoreDD ${DD_TEST_SOURCES})
  target_link_libraries(mqt-core-dd-test PRIVATE MQT::CoreCircuitOptimizer)
endif()
