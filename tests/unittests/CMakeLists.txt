enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(unittests 
  unittests.cpp
)
target_include_directories(unittests PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(
    unittests 
    PRIVATE
    GTest::gtest
)

if (MSVC)
    target_compile_options(unittests PRIVATE
        # Warnings
        /W4
        /WX
    )
else()
    target_compile_options(unittests PRIVATE
        # Warnings
        -Wall 
        -Wextra 
        -Werror
        # Sanitizers
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
    target_link_options(unittests PRIVATE
        # Sanitizers
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
endif()

gtest_discover_tests(unittests)