function(compile_shaders TARGET_NAME SHADER_SRC_DIR SHADER_DST_DIR)
  file(GLOB SHADER_SOURCES
    "${SHADER_SRC_DIR}/*.vert"
    "${SHADER_SRC_DIR}/*.frag"
  )

  set(SHADER_OUTPUTS "")

  foreach(SHADER_SOURCE ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME)
    set(SHADER_OUTPUT "${SHADER_DST_DIR}/${SHADER_NAME}.spv")
    list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
    set(GLSLC_ARGS "-I${CMAKE_CURRENT_SOURCE_DIR}/include/etna/shaders")

    add_custom_command(
      OUTPUT ${SHADER_OUTPUT}
      COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_DST_DIR}"
      COMMAND ${GLSLC} ${GLSLC_ARGS} "${SHADER_SOURCE}" -o "${SHADER_OUTPUT}"
      DEPENDS "${SHADER_SOURCE}"
      COMMENT "Compiling shader ${SHADER_NAME}"
    )
  endforeach()

  add_custom_target(compile_shaders_${TARGET_NAME} DEPENDS ${SHADER_OUTPUTS})
endfunction()
