set(TARGET_TREE rbtree)
set(TARGET_SET set)

set(SOURCES_TREE
    ${PROJECT_SOURCE_DIR}/source/main_tree.cpp
   )

set(SOURCES_SET
    ${PROJECT_SOURCE_DIR}/source/main_set.cpp
   )

add_executable(${TARGET_TREE} ${SOURCES_TREE})
target_include_directories(${TARGET_TREE} PRIVATE "${PROJECT_SOURCE_DIR}/include")

add_executable(${TARGET_SET} ${SOURCES_SET})
target_include_directories(${TARGET_SET} PRIVATE "${PROJECT_SOURCE_DIR}/include")

set_target_properties(${TARGET_TREE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(${TARGET_SET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS ${TARGET_TREE}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
install(TARGETS ${TARGET_SET}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

if (MSVC)
    target_compile_options(${TARGET_TREE} PRIVATE
        # Warnings
        /W4
        /WX
    )
    target_compile_options(${TARGET_SET} PRIVATE
    # Warnings
    /W4
    /WX
    )
else()
    target_compile_options(${TARGET_TREE} PRIVATE
        # Warnings
        -Wall 
        -Wextra  
        -Werror
        # Sanitizers 
        $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
    target_link_options(${TARGET_TREE} PRIVATE
        # Sanitizers
        $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
    target_compile_options(${TARGET_SET} PRIVATE
        # Warnings
        -Wall 
        -Wextra  
        -Werror
        # Sanitizers 
        $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
    target_link_options(${TARGET_SET} PRIVATE
        # Sanitizers
        $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
endif()
