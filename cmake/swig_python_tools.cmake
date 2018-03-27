# =======================================
# Macros and functions related to swig
# and python
# =======================================

# -----------------------------------
# Build targets to generate python
# docstrings from xml doxygen output.
#
# Warning : xml files must exist
# before any call to this macro!!
# -----------------------------------
macro(doxy2swig_docstrings COMP)
  if(WITH_${COMPONENT}_DOXY2SWIG)
    update_xml_doxy_config_file(${COMP})
    build_doc_xml(${COMP})
    set(DOCSTRINGS_FILES)
    # for each header of the current component ...
    foreach(_F ${${COMP}_HDRS})
      get_filename_component(_XFWE ${_F} NAME_WE)
      get_filename_component(_EXT ${_F} EXT)
      string(REPLACE "_" "__" _FWE "${_XFWE}")
      file(GLOB ${_FWE}_XMLS
	${DOXYGEN_OUTPUT}/xml/*class${_FWE}.xml
	${DOXYGEN_OUTPUT}/xml/*struct${_FWE}.xml
	${DOXYGEN_OUTPUT}/xml/${_FWE}_8h*.xml)
      foreach(_FXML ${${_FWE}_XMLS})
	get_filename_component(_FWE_XML ${_FXML} NAME_WE)
	set(outfile_name ${SICONOS_SWIG_ROOT_DIR}/${_FWE_XML}.i)
	add_custom_command(OUTPUT ${outfile_name} 
          DEPENDS ${DOXYGEN_OUTPUT}/xml/${_FWE_XML}.xml
          COMMAND ${PYTHON_EXECUTABLE}
	  ARGS "${CMAKE_BINARY_DIR}/share/doxy2swig.py"
          "--function-signature"
          "--type-info"
          "--constructor-list"
          "--attribute-list"
          "--overloaded-functions"
	  ${DOXYGEN_OUTPUT}/xml/${_FWE_XML}.xml ${outfile_name}
          COMMENT "docstrings generation for ${_FWE} (parsing ${_FWE_XML}.xml)")
	add_custom_target(doc_${_FWE_XML}.i DEPENDS ${outfile_name})
	list(APPEND DOCSTRINGS_FILES ${outfile_name})
      endforeach()
    endforeach()

    if (DOCSTRINGS_FILES)
      add_custom_command(OUTPUT ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i
        DEPENDS ${DOCSTRINGS_FILES}
        COMMAND cat
        ARGS ${DOCSTRINGS_FILES} > ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i
        COMMENT "${COMP} docstrings concatenation")
    else()
      add_custom_command(OUTPUT ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i
        DEPENDS ${DOCSTRINGS_FILES}
        COMMAND touch
        ARGS ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i)
    endif()
  else()
    add_custom_command(OUTPUT ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i
      DEPENDS ${DOCSTRINGS_FILES}
      COMMAND touch
      ARGS ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i
      )
  endif()
  add_custom_target(${COMP}_docstrings DEPENDS ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i)
endmacro()

# ----------------------------------------------------------------------
# Build a swig module from .i files
#
# Usage :
# add_siconos_swig_submodule(full_name)
#
# full_name must be a path to file.i, path relative to siconos
# python package root.
# For example, to build a swig module 'bullet' in siconos.mechanics.contact_detection,
# call this macro with full_name = mechanics/contact_detection/bullet
# and to build a swig module for kernel in siconos,
# call this macro with full_name = ./kernel
#
# ----------------------------------------------------------------------
macro(add_siconos_swig_sub_module fullname)
  get_filename_component(_name ${fullname} NAME)
  get_filename_component(_path ${fullname} PATH)
  
  message(" -- Build module ${_name} in directory ${_path} for parent ${COMPONENT}")
  # Add component dependencies to the current submodule deps.
  if(DEFINED SWIG_MODULE_${COMPONENT}_EXTRA_DEPS)
    set(SWIG_MODULE_${_name}_EXTRA_DEPS ${SWIG_MODULE_${COMPONENT}_EXTRA_DEPS})
  endif()
  
  # add as dependencies all the i files
  file(GLOB ${_name}_I_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/*.i)
  foreach(_f ${${_name}_I_FILES})
    list(APPEND SWIG_MODULE_${_name}_EXTRA_DEPS ${_f})
  endforeach()

  # set properties for current '.i' file
  set(swig_file ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/${_name}.i)

  # set output dir
  set(CMAKE_SWIG_OUTDIR "${SICONOS_SWIG_ROOT_DIR}/${_path}")

  # compile flags
  foreach(_dir ${${COMPONENT}_SWIG_INCLUDE_DIRECTORIES})
    set(${COMPONENT}_SWIG_DEFS "-I${_dir};${${COMPONENT}_SWIG_DEFS}")
  endforeach()

  # extra per-module flags if any
  set(${COMPONENT}_SWIG_DEFS_${_name} "${${COMPONENT}_SWIG_DEFS};${${COMPONENT}_SWIG_DEFS_${_name}}")

  IF(WITH_CXX AND (BUILD_AS_CPP OR NOT ${COMPONENT} MATCHES "numerics"))
    set_source_files_properties(${swig_file}
      PROPERTIES SWIG_FLAGS "${${COMPONENT}_SWIG_DEFS_${_name}}" CPLUSPLUS ON)
  ENDIF(WITH_CXX AND (BUILD_AS_CPP OR NOT ${COMPONENT} MATCHES "numerics"))

  # --- build swig module ---
  if(CMAKE_VERSION VERSION_LESS 3.8.0)
    swig_add_module(${_name} python ${swig_file})
  else()
    set(ADDITIONAL_SWIG_DEFINES ${ADDITIONAL_SWIG_DEFINES} -DBOOST_NOEXCEPT)
    swig_add_library(${_name} LANGUAGE python SOURCES ${swig_file})
  endif()
  # WARNING ${swig_generated_file_fullname} is overriden 
  set(${_name}_generated_file_fullname ${swig_generated_file_fullname})
  set_source_files_properties( ${swig_generated_file_fullname}
      PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing")
  # Set path for the library generated by swig for the current module --> siconos python package path
  set_property(TARGET ${SWIG_MODULE_${_name}_REAL_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${SICONOS_SWIG_ROOT_DIR}/${_path})
  message(" -- ${_name} generated (swig) file will be ${${_name}_generated_file_fullname}")

  # Set the SONAME for the SWIG module to the Siconos SONAME
  set_target_properties(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES
    NO_SONAME OFF
    VERSION "${SICONOS_SOVERSION}"
    SOVERSION "${SICONOS_SOVERSION_MAJOR}")

  IF(MSVC AND ${COMPONENT} MATCHES "kernel")
    set_source_files_properties(${${_name}_generated_file_fullname} PROPERTIES COMPILE_FLAGS "/bigobj")
  ENDIF()

  # Add a post-build step that prepends utf-8 coding indicator to .py files
  add_custom_command(TARGET ${SWIG_MODULE_${_name}_REAL_NAME}
    POST_BUILD COMMAND sh -c "(echo '# -*- coding: utf-8 -*-'; cat ${SICONOS_SWIG_ROOT_DIR}/${_path}/${_name}.py) > ${SICONOS_SWIG_ROOT_DIR}/${_path}/${_name}.tmp; mv ${SICONOS_SWIG_ROOT_DIR}/${_path}/${_name}.tmp ${SICONOS_SWIG_ROOT_DIR}/${_path}/${_name}.py" VERBATIM)

  # Check dependencies and then link ...
  add_dependencies(${SWIG_MODULE_${_name}_REAL_NAME} ${COMPONENT})

  if(UNIX AND NOT APPLE)
    # do not link against the Python library on unix, it is useless
    swig_link_libraries(${_name} ${${COMPONENT}_LINK_LIBRARIES} ${COMPONENT})
  else()
    swig_link_libraries(${_name} ${PYTHON_LIBRARIES} ${${COMPONENT}_LINK_LIBRARIES} ${COMPONENT})
  endif()

  # set dep between docstrings and python bindings
  add_dependencies(${SWIG_MODULE_${_name}_REAL_NAME} ${COMPONENT}_docstrings)
  
  # set dependency of sphinx apidoc to this target
  if(USE_SPHINX)
    add_dependencies(apidoc ${SWIG_MODULE_${_name}_REAL_NAME})
  endif()
  
  # Copy __init__.py file if needed
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/__init__.py.in)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_path}/__init__.py.in
      ${SICONOS_SWIG_ROOT_DIR}/${_path}/__init__.py @ONLY)
  elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/__init__.py)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_path}/__init__.py
      ${SICONOS_SWIG_ROOT_DIR}/${_path}/__init__.py COPYONLY)
  endif()

  # --- install python files and target ---
  # install path ...
  set(DEST "${SICONOS_PYTHON_INSTALL_DIR}/${SICONOS_PYTHON_PACKAGE}/${_path}")

  #install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${_name}.py DESTINATION ${DEST})
  install(TARGETS ${SWIG_MODULE_${_name}_REAL_NAME} LIBRARY DESTINATION ${DEST})
  
endmacro()

macro(swig_module_setup modules_list)
  if(WITH_${COMPONENT}_PYTHON_WRAPPER)
    # we should use lowercase name for python module (pep8 ...)
    message(" -- Prepare python bindings for component ${COMPONENT} ...")
    # build python bindings
    include_directories(${SICONOS_SWIG_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
    if(HAVE_SICONOS_IO)
      include_directories(${CMAKE_SOURCE_DIR}/io/src)
      if(HAVE_SICONOS_MECHANICS)
	include_directories(${CMAKE_SOURCE_DIR}/io/src/mechanics)
      endif()
    endif()
    foreach(module ${${COMPONENT}_PYTHON_MODULES})
      add_siconos_swig_sub_module(${module})
    endforeach()
  endif()
endmacro()

include(tools4tests)
