# Copyright (c) 2024 Chair for Design Automation, TUM
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

if(TARGET MQT::CoreAlgorithms)
  file(GLOB_RECURSE ALGO_TEST_SOURCES *.cpp)
  package_add_test(mqt-core-algorithms-test MQT::CoreAlgorithms ${ALGO_TEST_SOURCES})
  target_link_libraries(mqt-core-algorithms-test PRIVATE MQT::CoreDD MQT::CoreCircuitOptimizer)
endif()
