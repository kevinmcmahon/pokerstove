find_package (Threads)

# build the test
file(GLOB test_sources *.test.cpp)
add_executable(peval_tests ${test_sources})
target_link_libraries(peval_tests peval gtest gtest_main ${CMAKE_THREAD_LIBS_INIT})
target_compile_definitions(peval_tests PRIVATE
  POKERSTOVE_TESTDATA_DIR="${CMAKE_SOURCE_DIR}/src/lib/testdata")

# build the library sources from all sources, removing the test sources
file(GLOB lib_sources *.cpp)
foreach(source_file ${test_sources})
  list(REMOVE_ITEM lib_sources ${source_file})
endforeach()

# peval library
add_library(peval ${lib_sources})

add_test(NAME TestPevalFast
  COMMAND $<TARGET_FILE:peval_tests> --gtest_filter=-GoldenVectors.*)
add_test(NAME TestPevalGolden
  COMMAND $<TARGET_FILE:peval_tests> --gtest_filter=GoldenVectors.*)
set_tests_properties(TestPevalFast PROPERTIES LABELS "fast")
set_tests_properties(TestPevalGolden PROPERTIES LABELS "golden;integration")
