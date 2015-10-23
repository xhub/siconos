IF(MSVC AND "${CMAKE_Fortran_COMPILER}" MATCHES "gfortran")
  SET(GCC_A "")
  EXECUTE_PROCESS(COMMAND ${CMAKE_Fortran_COMPILER} -print-file-name=libgfortran.dll.a
	  OUTPUT_VARIABLE LIBGFORTRAN_DLL_A)
  EXECUTE_PROCESS(COMMAND cygpath -m ${LIBGFORTRAN_DLL_A}
    OUTPUT_VARIABLE LIBGFORTRAN)
  STRING(REGEX REPLACE "\n.*$" "" LIBGFORTRAN ${LIBGFORTRAN})
  MESSAGE(STATUS "libgfortran.dll.a :: ${LIBGFORTRAN}")
  SET(${COMPONENT}_LINK_LIBRARIES ${${COMPONENT}_LINK_LIBRARIES} ${LIBGFORTRAN})
  GET_FILENAME_COMPONENT(GFORTRAN_DIR ${CMAKE_Fortran_COMPILER} PATH)
  IF(NOT CMAKE_AR)
    SET(CMAKE_AR "${GFORTRAN_DIR}/ar")
  ENDIF(NOT CMAKE_AR)
  APPEND_Fortran_FLAGS("-static -static-libgcc -static-libgfortran -fno-stack-check -fno-stack-protector -mno-stack-arg-probe") # XXX No test :( -- xhub
ENDIF()
