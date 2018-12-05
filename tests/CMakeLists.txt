# Пример добавления теста:
# add_unit_tests(<Имя теста> <библиотека 1>  ... <библиотека N>)

include(${CMAKE_DIR}/UTest.cmake)

add_unit_test(ut_create_graph test_utils log pthread)
add_unit_test(ut_mapper test_utils mapper log pthread)
