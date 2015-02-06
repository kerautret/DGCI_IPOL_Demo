#CMakeLists associated to the demoIPOL subdir

# Make sure the compiler can find include files.
if ( CPP11_INITIALIZER_LIST )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif ( CPP11_INITIALIZER_LIST )

# Make sure the compiler can find include files.
include_directories (${PROJECT_SOURCE_DIR}/src/)
include_directories (${PROJECT_BINARY_DIR}/src)

# Make sure the compiler can find include files.
include_directories (${PROJECT_SOURCE_DIR}/demoIPOL_ExtrConnectedReg/)
include_directories (${PROJECT_BINARY_DIR}/demoIPOL_ExtrConnectedReg/)


# Make sure the linker can find the Hello library once it is built.
link_directories (${PROJECT_BINARY_DIR})

# ConfigExamples.h instanciation.
configure_file(${PROJECT_SOURCE_DIR}/demoIPOL_ExtrConnectedReg/ConfigDemoIPOL_ExtrConnectedReg.h.in 
  ${PROJECT_BINARY_DIR}/demoIPOL_ExtrConnectedReg/ConfigDemoIPOL_ExtrConnectedReg.h)

#------Examples subdirectories ------

# add_subdirectory(...)


#--------- Root directory


SET(DEMO_IPOL_SRC
  pgm2freeman
  displayContours
  extract3D
 )

FOREACH(FILE ${DEMO_IPOL_SRC})
  add_executable(${FILE} ${FILE})
  target_link_libraries (${FILE} DGtal DGtalIO)
ENDFOREACH(FILE)





