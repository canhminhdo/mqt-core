# Copyright (c) 2024 Chair for Design Automation, TUM
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

if(TARGET MQT::CoreDS)
  file(GLOB_RECURSE DS_TEST_SOURCES *.cpp)
  package_add_test(mqt-core-ds-test MQT::CoreDS ${DS_TEST_SOURCES})
endif()
