cmake_minimum_required(VERSION 3.12)
project(lower-user-interface)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SOURCE_DIR src) #Libraries' source directory
set(INCLUDE_DIR include) #Libraries' include directory

# Options
option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_STATIC_LIBS "Build static library" ON)

# Default build type
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(SOURCE_FILES "${SOURCE_DIR}/button.cpp" "${SOURCE_DIR}/element.cpp" "${SOURCE_DIR}/extra.cpp" "${SOURCE_DIR}/item.cpp" "${SOURCE_DIR}/itemHolder.cpp" "${SOURCE_DIR}/scrollBar.cpp" "${SOURCE_DIR}/set.cpp" "${SOURCE_DIR}/textField.cpp")
set(HEADER_FILES "${INCLUDE_DIR}/button.hpp" "${INCLUDE_DIR}/element.hpp" "${INCLUDE_DIR}/extra.hpp" "${INCLUDE_DIR}/item.hpp" "${INCLUDE_DIR}/itemHolder.hpp" "${INCLUDE_DIR}/scrollBar.hpp" "${INCLUDE_DIR}/set.hpp" "${INCLUDE_DIR}/textField.hpp")

# Configure library type(s)/class(es)
set(LIBRARY_CLASSES STATIC)
if (BUILD_SHARED_LIBS)
	list(APPEND LIBRARY_CLASSES SHARED)
endif()
if (NOT BUILD_STATIC_LIBS)
	list(REMOVE_ITEM LIBRARY_CLASSES STATIC)
endif()

# Build and install each library type/class
foreach (LCLASS IN LISTS LIBRARY_CLASSES)
	set(LNAME lui_${LCLASS})
	add_library(${LNAME} ${LCLASS} ${SOURCE_FILES})
	target_include_directories(${LNAME} PUBLIC ${INCLUDE_DIR})
	set_target_properties(${LNAME} PROPERTIES OUTPUT_NAME lui)
	install(TARGETS ${LNAME})
endforeach()

# Also install headers
install(FILES ${HEADER_FILES} DESTINATION include/lui)
install(FILES "${INCLUDE_DIR}/lui.hpp" DESTINATION include/lui)

