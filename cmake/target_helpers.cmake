include_guard()

function(_acqua_get_defined_var_count)
    set(defined_count 0)
    foreach(arg IN LISTS ARGN)
        if(${arg})
            math(EXPR defined_count "${defined_count}+1")
        endif()
    endforeach()
    return(PROPAGATE defined_count)
endfunction()

function(_aqua_gather_files dir)
    set(public_path  "${dir}/public")
    set(private_path "${dir}/private")
    if(EXISTS "${public_path}")
        file(GLOB_RECURSE public_headers CONFIGURE_DEPENDS "${public_path}/*.hpp")
        file(GLOB_RECURSE public_sources CONFIGURE_DEPENDS "${public_path}/*.cpp")
    endif()
    if(EXISTS "${private_path}")
        file(GLOB_RECURSE private_headers CONFIGURE_DEPENDS "${private_path}/*.hpp")
        file(GLOB_RECURSE private_sources CONFIGURE_DEPENDS "${private_path}/*.cpp")
    endif()
    return(PROPAGATE public_path private_path public_headers public_sources private_headers private_sources)
endfunction()

function(aqua_add_target target_name)
    message(STATUS "Configuring target ${target_name}")

    set(options CONSOLE_EXECUTABLE GUI_EXECUTABLE SHARED_LIBRARY STATIC_LIBRARY HEADER_ONLY_LIBRARY)
    set(oneValueArgs DIRECTORY)
    set(multiValueArgs CONFIGURATIONS)
    cmake_parse_arguments(PARSE_ARGV 1 arg
            "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )

    _acqua_get_defined_var_count(arg_CONSOLE_EXECUTABLE arg_GUI_EXECUTABLE arg_SHARED_LIBRARY arg_STATIC_LIBRARY arg_HEADER_ONLY_LIBRARY)
    message(STATUS "Count: ${defined_count}")
    if(${defined_count} EQUAL 0)
        message(FATAL_ERROR "Missing target type, use only one of ${options}")
    elseif(${defined_count} GREATER 1)
        message(FATAL_ERROR "Multiple target type, use only one of ${options}")
    endif()
    
    set(target_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    if(arg_DIRECTORY)
        set(target_dir "${arg_DIRECTORY}")
    endif()
    _aqua_gather_files("${target_dir}")

    if(public_sources)
        message(SEND_ERROR "Public sources are not permitted.")
        return()
    endif()

    if(arg_CONSOLE_EXECUTABLE OR arg_GUI_EXECUTABLE)

        if(NOT private_sources)
            message(SEND_ERROR "EXECUTABLE requires at least 1 private source file.")
            return()
        endif()

        if(WIN32 AND arg_GUI_EXECUTABLE)
            set(win32_property "WIN32")
        else()
            set(win32_property "")
        endif()

        add_executable("${target_name}" ${win32_property}
            ${public_headers}
            ${private_headers}
            ${private_sources}
        )
        if(public_headers)
            target_include_directories("${target_name}" PUBLIC "${public_path}")
        endif()
        if(private_headers)
            target_include_directories("${target_name}" PRIVATE "${private_path}")
        endif()

    elseif(arg_SHARED_LIBRARY OR arg_STATIC_LIBRARY)
    
        if(NOT private_sources OR NOT public_headers)
            message(SEND_ERROR "SHARED_LIBRARY and STATIC_LIBRARY requires at least 1 private source file and 1 public header.")
            return()
        endif()
    
        if(arg_SHARED_LIBRARY)
            set(lib_type "SHARED")
        else()
            set(lib_type "STATIC")
        endif()
        add_library("${target_name}" ${lib_type}
            ${public_headers}
            ${private_headers}
            ${private_sources}
        )
        if(public_headers)
            target_include_directories("${target_name}" PUBLIC "${public_path}")
        endif()
        if(private_headers)
            target_include_directories("${target_name}" PRIVATE "${private_path}")
        endif()
    
    elseif(arg_HEADER_ONLY_LIBRARY)
    
        if(private_sources)
            message(SEND_ERROR "HEADER_ONLY_LIBRARY can't have source files.")
        endif()
        if(NOT public_headers)
            message(SEND_ERROR "HEADER_ONLY_LIBRARY requires at least 1 public header.")
        endif()
        if(private_headers)
            message(WARNING "HEADER_ONLY_LIBRARY has private headers but they'll be propagated as public")
        endif()
        if(NOT public_headers AND NOT private_headers)
            return()
        endif()
    
        add_library("${target_name}" INTERFACE ${public_headers} ${private_headers})
        if(public_headers)
            target_include_directories("${target_name}" INTERFACE "${public_path}")
        endif()
        if(private_headers)
            target_include_directories("${target_name}" INTERFACE "${private_path}")
        endif()
    
    else()
        message(SEND_ERROR "Unsupported target")
    endif()
    message(STATUS "Target ${target_name} configured.")
endfunction()

function(aqua_add_subtarget target_name parent_target_name)
    if(NOT TARGET ${parent_target_name})
        message(SEND_ERROR "${parent_target_name} isn't a valid parent target.")
        return()
    endif()

    get_target_property(_real_parent_name ${parent_target_name} ALIASED_TARGET)
    if(_real_parent_name)
        set(parent_target_name ${_real_parent_name})
    endif()
    unset(_real_parent_name)

    get_target_property(parent_dir ${parent_target_name} SOURCE_DIR)
    aqua_add_target(${target_name} DIRECTORY "${parent_dir}/${target_name}" ${ARGN})

    if(NOT TARGET ${target_name})
        return()
    endif()

    get_target_property(parent_type ${parent_target_name} TYPE)
    if("${parent_type}" STREQUAL "INTERFACE_LIBRARY")
        target_link_libraries(${parent_target_name} INTERFACE ${target_name})
    else()
        target_link_libraries(${parent_target_name} PRIVATE ${target_name})
    endif()
endfunction()