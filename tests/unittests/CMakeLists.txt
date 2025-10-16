enable_testing()

find_package(GTest REQUIRED)

add_executable(unittests 
  unittests.cpp
)
target_include_directories(unittests PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(unittests PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:
        # Warnings
        -Wall -Wextra -Werror
        # Sanitizers 
        -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr
    >
)
target_link_options(unittests PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:
        # Sanitizers
        -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr
    >
)

target_link_libraries(
    unittests 
    PRIVATE
    GTest::gtest
)

include(GoogleTest)
gtest_discover_tests(unittests)