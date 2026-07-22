if(MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        /W4
        /WX
        /permissive-
        /EHsc
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    add_compile_options(
        -Wall
        -Wextra
        -Werror
    )
endif()
