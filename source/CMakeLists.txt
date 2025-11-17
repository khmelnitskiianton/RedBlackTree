set(TARGET_TREE rbtree)
set(TARGET_TREE_BENCH rbtree-bench)
set(TARGET_SET set)
set(TARGET_SET_BENCH set-bench)

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

if (TIME_COUNT)
    add_executable(${TARGET_TREE_BENCH} ${SOURCES_TREE})
    target_include_directories(${TARGET_TREE_BENCH} PRIVATE "${PROJECT_SOURCE_DIR}/include")
    target_compile_definitions(${TARGET_TREE_BENCH} PRIVATE TIME_COUNT)

    add_executable(${TARGET_SET_BENCH} ${SOURCES_SET})
    target_include_directories(${TARGET_SET_BENCH} PRIVATE "${PROJECT_SOURCE_DIR}/include")
    target_compile_definitions(${TARGET_SET_BENCH} PRIVATE TIME_COUNT)

    set_target_properties(${TARGET_TREE_BENCH} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    set_target_properties(${TARGET_SET_BENCH} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    install(TARGETS ${TARGET_TREE_BENCH}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
    install(TARGETS ${TARGET_SET_BENCH}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
endif()


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
    if (TIME_COUNT)
        target_compile_options(${TARGET_TREE_BENCH} PRIVATE
            # Warnings
            /W4
            /WX
        )
        target_compile_options(${TARGET_SET_BENCH} PRIVATE
            # Warnings
            /W4
            /WX
        )
    endif()
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
    if (TIME_COUNT)
        target_compile_options(${TARGET_TREE_BENCH} PRIVATE
            # Warnings
            -Wall 
            -Wextra  
            -Werror
            # Sanitizers 
            $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
        )
        target_link_options(${TARGET_TREE_BENCH} PRIVATE
            # Sanitizers
            $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
        )
        target_compile_options(${TARGET_SET_BENCH} PRIVATE
            # Warnings
            -Wall 
            -Wextra  
            -Werror
            # Sanitizers 
            $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
        )
        target_link_options(${TARGET_SET_BENCH} PRIVATE
            # Sanitizers
            $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
        )
    endif()
endif()

if (VIS_LOGS)
    target_compile_definitions(${TARGET_TREE} PRIVATE VIS_LOGS)
    target_compile_definitions(${TARGET_SET} PRIVATE VIS_LOGS)

    if (TIME_COUNT)
        target_compile_definitions(${TARGET_TREE_BENCH} PRIVATE VIS_LOGS)
        target_compile_definitions(${TARGET_SET_BENCH} PRIVATE VIS_LOGS)
    endif()
endif()