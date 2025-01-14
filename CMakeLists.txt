cmake_minimum_required(VERSION 3.12)
project(pyg)
set(CMAKE_CXX_STANDARD 14)
set(PYG_VERSION 0.1.0)

option(BUILD_TEST "Enable testing" OFF)
option(BUILD_BENCHMARK "Enable benchmarks" OFF)
option(WITH_COV "Enable code coverage" OFF)
option(USE_PYTHON "Link to Python when building" OFF)
option(WITH_CUDA "Enable CUDA support" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1")

set(WITH_MKL_BLAS 0)
if(USE_MKL_BLAS AND DEFINED BLAS_INCLUDE_DIR)
  find_file(MKL_INCLUDE_FOUND mkl.h ${BLAS_INCLUDE_DIR} NO_DEFAULT_PATH)
  if(MKL_INCLUDE_FOUND)
    set(WITH_MKL_BLAS 1)
  else()
    if(WITH_COV)
      message(FATAL_ERROR "The mkl.h file was not found - pass the correct directory or set USE_MKL_BLAS=OFF.")
    else()
      message(WARNING "The mkl.h file was not found - building pyg-lib without MKL BLAS support.")
    endif()
  endif()
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pyg_lib/csrc/config.h.in "${CMAKE_CURRENT_SOURCE_DIR}/pyg_lib/csrc/config.h")

if (USE_PYTHON)
  add_definitions(-DUSE_PYTHON)
  find_package(Python3 REQUIRED COMPONENTS Development)
endif()

set(CSRC pyg_lib/csrc)
file(GLOB_RECURSE ALL_SOURCES ${CSRC}/*.cpp)
file(GLOB_RECURSE ALL_HEADERS ${CSRC}/*.h)
add_library(${PROJECT_NAME} SHARED ${ALL_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
if(MKL_INCLUDE_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${BLAS_INCLUDE_DIR})
endif()

if (NOT "$ENV{EXTERNAL_PHMAP_INCLUDE_DIR}" STREQUAL "")
  include_directories($ENV{EXTERNAL_PHMAP_INCLUDE_DIR})
else()
  set(PHMAP_DIR third_party/parallel-hashmap)
  target_include_directories(${PROJECT_NAME} PRIVATE ${PHMAP_DIR})
endif()

find_package(Torch REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_LIBRARIES})

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif()

if (USE_PYTHON)
  target_link_libraries(${PROJECT_NAME} PRIVATE Python3::Python)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

message("-- CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("-- CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message("-- CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

set_target_properties(${PROJECT_NAME} PROPERTIES
  EXPORT_NAME PyG
  INSTALL_RPATH ${TORCH_INSTALL_PREFIX}/lib)
