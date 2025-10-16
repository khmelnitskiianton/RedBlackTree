set(TARGET triangles)

set(SOURCES 
    ${PROJECT_SOURCE_DIR}/source/main.cpp
   )

add_executable(${TARGET} ${SOURCES})

target_include_directories(${TARGET} PUBLIC "${PROJECT_SOURCE_DIR}/include")
set_target_properties(${TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_options(${TARGET} PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:
        # Warnings
        -Wall -Wextra -Werror

        # Sanitizers 
        -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr
    >
)
target_link_options(${TARGET} PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:
        # Sanitizers
        -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr
    >
)
