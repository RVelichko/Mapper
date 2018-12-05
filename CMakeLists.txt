cmake_minimum_required(VERSION 2.8)

message("\nConfigure project Run...")

project(Run)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release."
        FORCE
        )
endif()


# Общие флаги для всех сборок.
add_definitions(
    -pedantic -pedantic-errors
    -pipe -fPIC
    -Wall -Wno-long-long -Wno-inline
    -finline-functions -finline-limit=2000
    --param large-function-growth=5000 --param inline-unit-growth=600
    -I.
    -DDLL_PUBLIC=__attribute__\(\(visibility\(\"default\"\)\)\)
    )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -D__STDC_LIMIT_MACROS=1 -D_GLIBCXX_USE_NANOSLEEP")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    #Добавляем флаги для построения отладочной инфомрации.
    add_definitions(-fno-inline-functions)
    add_definitions(-fno-inline)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-O3)
    add_definitions(-ggdb3)
    add_definitions(-s)
    add_definitions(-DNDEBUG)
endif()
message("Build type: \"${CMAKE_BUILD_TYPE}\"")


# Установка вспомогательных констант
set(ROOT_DIR       ${CMAKE_CURRENT_SOURCE_DIR})
set(UTILS_DIR      ${ROOT_DIR}/utils)
set(BINARY_DIR     ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_DIR      ${ROOT_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)

add_definitions(-DSRC_DIR="${ROOT_DIR}")


# Указание общих настроек
link_directories("/usr/lib")
link_directories("/usr/local/lib")


# Настройка опций сборки компонент
OPTION(ENABLE_TESTS "Enable TESTS support [default: OFF]" OFF)


find_package(Threads REQUIRED)


# Поиск библиотек Boost версии старше 1.50
find_package(Boost 1.50 COMPONENTS
    system
    graph
    unit_test_framework
    )

if(Boost_FOUND)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_STATIC_RUNTIME OFF)
    set(BOOST_ALL_DYN_LINK ON)

    message(STATUS "Boost_LIBRARY_DIRS: \"${Boost_LIBRARY_DIRS}\"")
    message(STATUS "Boost_INCLUDE_DIRS: \"${Boost_INCLUDE_DIRS}\"")
    message(STATUS "Boost_LIBRARIES:")
    foreach(BOOST_LIB ${Boost_LIBRARIES})
        message(STATUS "    \"${BOOST_LIB}\"")
    endforeach()

    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})

    add_definitions(-DBOOST_DIN_LINK)
else()
    message(FATAL_ERROR "Boost libraries not found. Please get a copy from http://www.boost.org")
endif()


# Поиск дополнительных библиотек
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
include(${CMAKE_DIR}/TakeComponent.cmake)


# Установка общих директорий включаемых файлов
include_directories(${UTILS_DIR})


# Включение в сборку компонент
add_subdirectory(${UTILS_DIR})

include_directories(${ROOT_DIR})

if(ENABLE_TESTS)
    set(ALL_TESTS ON)

    string( TOLOWER ${ENABLE_TESTS} LOWER_STR)
    if (NOT ${LOWER_STR} MATCHES "on" AND NOT ${LOWER_STR} MATCHES "all")
        set(ALL_TESTS OFF)
        separate_arguments(LOWER_STR)
        foreach (TEST_NAME ${LOWER_STR})
            message(STATUS "Enable test: `${TEST_NAME}`")
            set(ut_${TEST_NAME} ON)
        endforeach()
    else()
        message(STATUS "Enable ALL tests")
    endif()

    enable_testing()

    include(${CMAKE_DIR}/UTest.cmake)

    # Инициализировать общие тесты
    set(TESTS_DIR ${ROOT_DIR}/tests)
    message(STATUS "TESTS_DIR: ${TESTS_DIR}")
    add_subdirectory("${TESTS_DIR}")
else(ENABLE_TESTS)
    message(STATUS "Directory tests is not included to build. Run cmake -DENABLE_TESTS=ON or -DENABLE_TESTS=\"ut_test_1 ut_test_2 ...\" to include.")
endif()

add_library(test_utils
    LinksGenerator.cpp
    )

add_library(mapper
    Mapper.cpp
    )

add_executable(exec-mapper
    main.cpp
    )
target_link_libraries(exec-mapper
    test_utils
    mapper
    log
    pthread
    )
