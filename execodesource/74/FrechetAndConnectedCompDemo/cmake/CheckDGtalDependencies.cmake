# -----------------------------------------------------------------------------
# Check Mandatory Dependencies
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Look for boost 
# -----------------------------------------------------------------------------

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_FOUND TRUE)
message(STATUS "Boost and boost_program_options found.")


set(ImaGeneArg_USE_STATIC_LIBS   ON)
set(ImaGeneArg_USE_MULTITHREADED ON)
set(ImaGeneArg_USE_STATIC_RUNTIME OFF)
set(ImaGeneArg_FOUND TRUE)
message(STATUS "ImaGeneArg found.")

SET(Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/boost-1.57)
SET(ImaGeneArg_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/ImaGene)
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${ImaGeneArg_INCLUDE_DIRS} )
SET(DGtalLibInc ${Boost_INCLUDE_DIRS} ${ImaGeneArg})


# -----------------------------------------------------------------------------
# Setting librt dependency on Linux
# -----------------------------------------------------------------------------
if (UNIX AND NOT(APPLE))
  SET(DGtalLibDependencies ${DGtalLibDependencies} -lrt)
endif()
