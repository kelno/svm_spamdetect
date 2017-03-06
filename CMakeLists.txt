project(spam_svm)
cmake_minimum_required(VERSION 3.7)

file(GLOB svm_SRC
    "svm/*.h"
    "svm/*.cpp"
)

add_library(svm
	${svm_SRC}
)

include_directories(svm)
add_executable(svm_test 
    svm_test/main.cpp
)

target_link_libraries(svm_test
  PUBLIC
    svm)
    
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /O2 /EHsc /I. /D _WIN64 /D _CRT_SECURE_NO_DEPRECATE /openmp")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -O3 -fPIC -D _STRING -fopenmp")
endif()