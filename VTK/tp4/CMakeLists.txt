#cherche cmake
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

#nom du projet
project(ad_td4)

SET(OperatingSystem "Linux")

#importer les librairies vtk & mpi
find_package(VTK QUIET) #version 9.0.0
find_package(MPI REQUIRED)

#arrete tout si vtk non trouve
if (NOT VTK_FOUND)
  message("Skipping Prog: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")

#definit le chemin vers le fichier de donnees
set (MY_MESHES_PATH ../..)

#fichiers de conf
configure_file (
  "${PROJECT_SOURCE_DIR}/Config.h.in"
  "${PROJECT_BINARY_DIR}/Config.h"
  )

#inclus le config
include_directories("${PROJECT_BINARY_DIR}")
include_directories(${MPI_INCLUDE_PATH})

#chemin vers les dossiers
file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h )

#generer exe
add_executable(${PROJECT_NAME} ${SRC_LIST})

#ajout librairies
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES} ${MPI_LIBRARIES})

#tests
enable_testing()
add_test(SimpleTest ${PROJECT_NAME})

if(MPI_COMPILE_FLAGS)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()


add_custom_target(prun1
    COMMAND mpirun    -np 1 ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(prun2
    COMMAND mpirun   -np 2 ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(prun4
    COMMAND mpirun    -np 4 ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# vtk_module_autoinit
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES}
)
