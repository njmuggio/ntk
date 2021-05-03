include(CTest)
enable_testing()

add_subdirectory(unity)

add_executable(ntk_tests ntk_tests.c test_articles.h)
target_link_libraries(ntk_tests ntk unity)

add_test(ntk ntk_tests)
