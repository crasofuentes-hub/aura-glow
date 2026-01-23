add_library(auraglow_engine
  src/engine.cpp
)

target_include_directories(auraglow_engine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(auraglow_engine PUBLIC cxx_std_17)

if(MSVC)
  target_compile_options(auraglow_engine PRIVATE /W4 /permissive-)
else()
  target_compile_options(auraglow_engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- Rust staticlib integration ---
option(AURAGLOW_ENABLE_RUST "Build and link Rust staticlib" ON)

if(AURAGLOW_ENABLE_RUST)
  find_program(CARGO_EXE NAMES cargo REQUIRED)

  set(AURAGLOW_RUST_ROOT "${CMAKE_SOURCE_DIR}/rust")
  set(AURAGLOW_RUST_MANIFEST "${AURAGLOW_RUST_ROOT}/Cargo.toml")

  # Cargo target dir dentro del build (reproducible, no ensucia rust/target)
  set(AURAGLOW_CARGO_TARGET "${CMAKE_BINARY_DIR}/cargo-target")
  file(MAKE_DIRECTORY "${AURAGLOW_CARGO_TARGET}")

  if(WIN32)
    set(AURAGLOW_RUST_LIB "${AURAGLOW_CARGO_TARGET}/release/auraglow.lib")
  else()
    set(AURAGLOW_RUST_LIB "${AURAGLOW_CARGO_TARGET}/release/libauraglow.a")
  endif()

  add_custom_command(
    OUTPUT "${AURAGLOW_RUST_LIB}"
    COMMAND "${CMAKE_COMMAND}" -E env
            "CARGO_TARGET_DIR=${AURAGLOW_CARGO_TARGET}"
            "${CARGO_EXE}" build --manifest-path "${AURAGLOW_RUST_MANIFEST}" --release -p auraglow
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Building Rust staticlib (auraglow) into ${AURAGLOW_CARGO_TARGET}"
    VERBATIM
  )

  add_custom_target(auraglow_rust_build DEPENDS "${AURAGLOW_RUST_LIB}")

  add_library(auraglow_rust STATIC IMPORTED GLOBAL)
  set_target_properties(auraglow_rust PROPERTIES IMPORTED_LOCATION "${AURAGLOW_RUST_LIB}")

  
  target_compile_definitions(auraglow_engine PRIVATE AURAGLOW_ENABLE_RUST=1)

  add_dependencies(auraglow_engine auraglow_rust_build)
  target_link_libraries(auraglow_engine PRIVATE auraglow_rust)
  target_include_directories(auraglow_engine PRIVATE "${CMAKE_SOURCE_DIR}/engine/include")
endif()

