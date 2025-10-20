find_package(Boost REQUIRED COMPONENTS filesystem)

set(TARGET rbtree)

set(SOURCES 
    ${PROJECT_SOURCE_DIR}/source/main.cpp
   )

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} PRIVATE Boost::filesystem)
target_include_directories(${TARGET} PRIVATE "${PROJECT_SOURCE_DIR}/include")

set_target_properties(${TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS ${TARGET}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

if (MSVC)
    target_compile_options(${TARGET} PRIVATE
        # Warnings
        /W4
        /WX
    )
else()
    target_compile_options(${TARGET} PRIVATE
        # Warnings
        -Wall 
        -Wextra 
        -Werror
        # Sanitizers 
        $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
    target_link_options(${TARGET} PRIVATE
        # Sanitizers
        $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
endif()
