add_executable(mo-tests mo-test.cpp)
find_package(GTest REQUIRED)

target_link_libraries(mo-tests
        PRIVATE
        GTest::GTest
        GTest::gtest_main
        mo)

add_test(mo mo-tests)