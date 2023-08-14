# #############################################################################

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

# #############################################################################
#
add_executable(co_server
  co_server.cpp
  client.cpp client.hpp)

list(APPEND TARGETS co_server)

# #############################################################################
#
add_executable(thread_server
  thread_server.cpp
  client.cpp client.hpp)

list(APPEND TARGETS thread_server)

# #############################################################################
#
add_executable(co_client co_client.cpp)

list(APPEND TARGETS co_client)

# #############################################################################
#

set_target_properties(${TARGETS} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

foreach(target ${TARGETS})
  target_compile_features(${target} PUBLIC cxx_std_20)
  set_target_properties(${target} PROPERTIES CXX_STANDARD_REQUIRED ON)

  target_link_libraries(${target} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${target}
      PRIVATE -g -Wall -Wextra -Werror -pedantic)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${target}
      PRIVATE /W4 /WX)
  endif()
endforeach()

# #############################################################################