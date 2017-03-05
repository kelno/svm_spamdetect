project(spam_svm)
cmake_minimum_required(VERSION 3.7)

add_library(libsvm_string
  libsvm_string/svm.cpp
)

include_directories(libsvm_string)
add_executable(spam_svm 
    main.cpp
    SVMModel.cpp
)

target_link_libraries(spam_svm
  PUBLIC
    libsvm_string)
    
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /O2 /EHsc /I. /D _WIN64 /D _CRT_SECURE_NO_DEPRECATE")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -O3 -fPIC -D _STRING")
endif()