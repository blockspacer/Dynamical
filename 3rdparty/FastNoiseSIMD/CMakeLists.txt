# This is currently very rudimentary and likely needs various improvements.

cmake_minimum_required(VERSION 3.7) # Requiring 3.7 might be incorrect.
project(FastNoiseSIMD)

set(CMAKE_CXX_STANDARD 11) # Requiring C++11 might be incorrect.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(directory ${PROJECT_NAME})
set(library_name ${PROJECT_NAME})
set(file_prefix ${PROJECT_NAME})

set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${library_name})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")

set(flags)
if(${MSVC})
	if(${FN_COMPILE_AVX2} OR ${FN_COMPILE_AVX512})
		list(APPEND flags "/arch:AVX2")
	endif()
elseif()
	if(${FN_COMPILE_AVX2} OR ${FN_COMPILE_AVX512})
		list(APPEND flags "-march=core-avx2")
	endif()
endif()

add_library(${library_name} STATIC
	${directory}/${file_prefix}.cpp
	${directory}/${file_prefix}_avx2.cpp
	${directory}/${file_prefix}_avx512.cpp
	${directory}/${file_prefix}_internal.cpp
	${directory}/${file_prefix}_neon.cpp
	${directory}/${file_prefix}_sse2.cpp
	${directory}/${file_prefix}_sse41.cpp
	)

target_include_directories(${library_name} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/include
	${directory}/
	)

set(public_headers
	${CMAKE_CURRENT_BINARY_DIR}/include/${file_prefix}.h
	# ${directory}/${file_prefix}_internal.h
	)
set_target_properties(${library_name} PROPERTIES
	PUBLIC_HEADER "${public_headers}"
	)

target_compile_options(${library_name} PRIVATE
	"${flags}"
	)

install(TARGETS ${library_name}
	CONFIGURATIONS Debug
	ARCHIVE DESTINATION lib/Debug
	PUBLIC_HEADER DESTINATION include
	)
install(TARGETS ${library_name}
	CONFIGURATIONS Release
	ARCHIVE DESTINATION lib/Release
	PUBLIC_HEADER DESTINATION include
	)
