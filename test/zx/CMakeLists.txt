# Copyright (c) 2024 Chair for Design Automation, TUM
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

if(TARGET MQT::CoreZX)
  file(GLOB_RECURSE ZX_TEST_SOURCES *.cpp)
  package_add_test(mqt-core-zx-test MQT::CoreZX ${ZX_TEST_SOURCES})
endif()
