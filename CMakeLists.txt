add_executable( remesher3d_exe EXCLUDE_FROM_ALL main.cpp remesher3d.cpp 
../marching-tets/tet-functions.cpp ../marching-tets/marchingtet.cpp 
./sizing-fields/edgelengthsizingfield.cpp)
target_link_libraries( remesher3d_exe flux_shared )

target_compile_definitions( remesher3d_exe PUBLIC -DFLUX_FULL_UNIT_TEST=false )

add_custom_target( remesher3d command $<TARGET_FILE:remesher3d_exe> 1 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/projects/remesher3d )

ADD_DEBUG_TARGETS( remesher3d ${CMAKE_SOURCE_DIR}/projects/remesher3d/ )