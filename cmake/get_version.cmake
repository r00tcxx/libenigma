function(GET_REP_VERSION major minor build commit)
  execute_process(
    COMMAND git describe --tags --long --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  if(NOT GIT_DESCRIBE)
    set(${major} 0 PARENT_SCOPE)
    set(${minor} 0 PARENT_SCOPE)
    set(${build} 0 PARENT_SCOPE)
    set(${commit} 0 PARENT_SCOPE)
  else()
    string(REGEX MATCH "([0-9]+)\\.([0-9]+)-([0-9]+)-g([0-9a-f]+)" VERSION_MATCHES ${GIT_DESCRIBE})
    set(${major} ${CMAKE_MATCH_1} PARENT_SCOPE)
    set(${minor} ${CMAKE_MATCH_2} PARENT_SCOPE)
    set(${build} ${CMAKE_MATCH_3} PARENT_SCOPE)
    set(${commit} ${CMAKE_MATCH_4} PARENT_SCOPE)
  endif()
endfunction()