add_executable(${PROJECT_NAME})

# Agrega los archivos fuente al target
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    hilbert.cpp
    morton.cpp
    qtree.cpp
    # otree.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
