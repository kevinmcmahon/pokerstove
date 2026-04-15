find_package (Threads)

# build the test
file(GLOB test_sources *.test.cpp)
add_executable(penum_tests ${test_sources})
target_link_libraries(penum_tests penum peval gtest gtest_main ${CMAKE_THREAD_LIBS_INIT})
target_compile_definitions(penum_tests PRIVATE
  POKERSTOVE_TESTDATA_DIR="${CMAKE_SOURCE_DIR}/src/lib/testdata")

# build the library sources from all sources, removing the test sources
file(GLOB lib_sources *.cpp)
foreach(source_file ${test_sources})
  list(REMOVE_ITEM lib_sources ${source_file})
endforeach()

# penum library
add_library(penum ${lib_sources})

add_test(NAME TestPenumFast
  COMMAND $<TARGET_FILE:penum_tests> --gtest_filter=-GoldenVectors.*)
add_test(NAME TestPenumGolden
  COMMAND $<TARGET_FILE:penum_tests> --gtest_filter=GoldenVectors.*)
add_test(NAME TestPenumSlow
  COMMAND $<TARGET_FILE:penum_tests>
          --gtest_also_run_disabled_tests
          --gtest_filter=PartitionEnumerator.DISABLED_SlowAllOfcpDraws)
set_tests_properties(TestPenumFast PROPERTIES LABELS "fast")
set_tests_properties(TestPenumGolden PROPERTIES LABELS "golden;integration")
set_tests_properties(TestPenumSlow PROPERTIES LABELS "slow" DISABLED TRUE)
