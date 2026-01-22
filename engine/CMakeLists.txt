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

  # Put all Rust build outputs inside the CMake build tree (clean + reproducible)
  set(AURAGLOW_CARGO_TARGET_DIR "${CMAKE_BINARY_DIR}/cargo-target")

  if(WIN32)
    set(AURAGLOW_RUST_LIB "${AURAGLOW_CARGO_TARGET_DIR}/release/auraglow.lib")
  else()
    set(AURAGLOW_RUST_LIB "${AURAGLOW_CARGO_TARGET_DIR}/release/libauraglow.a")
  endif()

  add_custom_command(
    OUTPUT "${AURAGLOW_RUST_LIB}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${AURAGLOW_CARGO_TARGET_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E env
            "CARGO_TARGET_DIR=${AURAGLOW_CARGO_TARGET_DIR}"
            "${CARGO_EXE}" build --manifest-path "${AURAGLOW_RUST_MANIFEST}" --release -p auraglow
    WORKING_DIRECTORY "${AURAGLOW_RUST_ROOT}"
    DEPENDS "${AURAGLOW_RUST_MANIFEST}"
    COMMENT "Building Rust staticlib (auraglow) into ${AURAGLOW_CARGO_TARGET_DIR}"
    VERBATIM
  )

  add_custom_target(auraglow_rust_build DEPENDS "${AURAGLOW_RUST_LIB}")

  add_library(auraglow_rust STATIC IMPORTED GLOBAL)
  set_target_properties(auraglow_rust PROPERTIES IMPORTED_LOCATION "${AURAGLOW_RUST_LIB}")

  add_dependencies(auraglow_engine auraglow_rust_build)
  target_link_libraries(auraglow_engine PRIVATE auraglow_rust)
endif()
