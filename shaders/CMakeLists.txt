set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_DEST_DIR ${CMAKE_BINARY_DIR}/src/shaders)

# Crea el directorio de destino si no existe
file(MAKE_DIRECTORY ${SHADER_DEST_DIR})

# Encuentra todos los archivos de shaders en el directorio de shaders
file(GLOB SHADER_FILES "${SHADER_SRC_DIR}/*.frag" "${SHADER_SRC_DIR}/*.vert")

# Crea una tarea personalizada para copiar los archivos de shaders
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_SRC_DIR} ${SHADER_DEST_DIR}
    COMMENT "Copiando shaders desde ${SHADER_SRC_DIR} a ${SHADER_DEST_DIR}"
)
