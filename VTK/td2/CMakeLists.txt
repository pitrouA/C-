#cherche cmake
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

#nom du projet
project(ad_td2)

#importer les librairies vtk
find_package(VTK QUIET) #version 9.0.0

#arrete tout si vtk non trouve
if (NOT VTK_FOUND)
  message("Skipping Prog: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")

#chemin vers les dossiers
file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )

#generer exe
add_executable(${PROJECT_NAME} ${SRC_LIST})

#ajout librairies
target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})

#tests
enable_testing()
add_test(SimpleTest ${PROJECT_NAME})
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ${PROJECT_NAME})

# vtk_module_autoinit
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES}
)
