# -----------------------------------------------
# Macro to add a CxxTest to the build and CTest fixture
#
# From the Cmake Wiki
# http://www.cmake.org/Wiki/CMakeMacroAddCxxTest
# -----------------------------------------------

# TODO: Should this be fatal? Should we exclude tests 
#       from the Normal Build?

# If Perl AND Python are not found flag a Fatal error
IF (NOT PERL_FOUND AND NOT PYTHONINTERP_FOUND)
   MESSAGE(FATAL_ERROR "Neather Perl nor Python are installed, Install one or the other")
ENDIF (NOT PERL_FOUND AND NOT PYTHONINTERP_FOUND)

# If Python Interp is found use that
# -----------------------------------
IF(PYTHONINTERP_FOUND)
    SET(CXX_INTERP ${PYTHON_EXECUTABLE})
    SET(CXXTESTGEN ${CMAKE_SOURCE_DIR}/src/cxxtest/cxxtestgen.py)
    MESSAGE(STATUS "Python Found..")
ENDIF(PYTHONINTERP_FOUND)

# If Perl is found use that
# ---------------------------
IF(PERL_FOUND)
    SET(CXX_INTERP ${PERL_EXECUTABLE})
    SET(CXXTESTGEN ${CMAKE_SOURCE_DIR}/src/cxxtest/cxxtestgen.pl)
    MESSAGE(STATUS "Perl Found..")
ENDIF(PERL_FOUND)

# Definiton of the macro
# --------------------------
MACRO(ADD_CXXTEST NAME)

    # Generate the parts
    # ------------------------
    FOREACH(_PART ${ARGN})
        GET_FILENAME_COMPONENT(_NAME ${_PART} NAME)
        GET_FILENAME_COMPONENT(_NAME_WE ${_PART} NAME_WE)
        ADD_CUSTOM_COMMAND(
                            OUTPUT ${_NAME_WE}.cpp
                            COMMAND ${CXX_INTERP} ${CXXTESTGEN}
                            --part
                            -o ${_NAME_WE}.cpp
                            ${_NAME}
                            DEPENDS ${_PART}
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                          )
    ENDFOREACH(_PART)

    # Generate the runner
    # ------------------------
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}_runner.cpp
      COMMAND
        ${CXX_INTERP} ${CXXTESTGEN}
        --runner=ErrorPrinter --root
        -o ${NAME}_runner.cpp
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Enumerate all generated files
    # ------------------------
    SET(PARTS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}_runner.cpp)
    FOREACH(_PART ${ARGN})
        GET_FILENAME_COMPONENT(_PART_WE ${_PART} NAME_WE)
        SET(PARTS ${PARTS} ${_PART_WE}.cpp)
    ENDFOREACH(_PART)
    
    # Add the executable to the build ( TODO: EXCLUDE ALL ? )
    # ------------------------
    ADD_EXECUTABLE(${NAME} ${PARTS})
    
    # Add the executable to CTest
    # ------------------------
    ADD_TEST(${NAME} ${EXECUTABLE_OUTPUT_PATH}/${NAME})

ENDMACRO(ADD_CXXTEST)

