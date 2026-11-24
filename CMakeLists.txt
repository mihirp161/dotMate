cmake_minimum_required(VERSION 3.16)
project(dotmate2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(dotmate_core
    src/text_utils.cpp
    src/knowledge_base.cpp
    src/matcher.cpp
    src/cycle_tracker.cpp
    src/privacy.cpp
    src/returning_user_store.cpp
    src/session.cpp
)
target_include_directories(dotmate_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

add_executable(dotmate_tests
    tests/test_main.cpp
    tests/test_text_utils.cpp
    tests/test_matcher.cpp
    tests/test_cycle_tracker.cpp
    tests/test_session.cpp
)
target_link_libraries(dotmate_tests PRIVATE dotmate_core)

enable_testing()
add_test(NAME dotmate_tests COMMAND dotmate_tests)

if(EMSCRIPTEN)
    # WebAssembly build -- see scripts/build-wasm.sh:
    #   emcmake cmake -S . -B build-wasm && cmake --build build-wasm
    # Produces dotmate.mjs + dotmate.wasm for web/wasm/. The test suite is
    # built too and runs under Node (ctest does that automatically), so the
    # same checks cover the native and the WebAssembly build.
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")
    target_compile_options(dotmate_core PUBLIC -fexceptions)  # the core throws and catches

    add_executable(dotmate_wasm src/wasm_bindings.cpp)
    target_link_libraries(dotmate_wasm PRIVATE dotmate_core)
    set_target_properties(dotmate_wasm PROPERTIES OUTPUT_NAME dotmate SUFFIX ".mjs")
    target_link_options(dotmate_wasm PRIVATE
        -fexceptions
        -sMODULARIZE=1 -sEXPORT_ES6=1 -sEXPORT_NAME=createDotmate
        -sENVIRONMENT=web,node
        -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=1048576
        -sDYNAMIC_EXECUTION=0                      # no eval: keeps a strict CSP possible
        "-sEXPORTED_FUNCTIONS=['_dotmate_init','_dotmate_send_line']"
        "-sEXPORTED_RUNTIME_METHODS=['cwrap']"
        "--embed-file=${CMAKE_CURRENT_SOURCE_DIR}/data/menstrual_qa.json@/data/menstrual_qa.json"
    )
    target_link_options(dotmate_tests PRIVATE -fexceptions -sENVIRONMENT=node)
else()
    add_executable(dotmate src/main.cpp)
    target_link_libraries(dotmate PRIVATE dotmate_core)

    # Copy the data folder next to the built executable so main.cpp's
    # locate_data_file() finds it without any extra setup.
    add_custom_command(TARGET dotmate POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:dotmate>/data
    )
endif()
