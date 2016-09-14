# vim: ts=2 sw=2 ft=CMake
# Copyright (C) 2007-2011 LuaDist.
# Copyright (C) 2015 Atronix Engineering, Inc
# Created by Peter Drahos
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.
project ( libffi C ASM)
cmake_minimum_required ( VERSION 2.8 )
if ( MSVC )
	enable_language(ASM_MASM)
endif ( )
# NOTE: For now this only supports x86 systems, mainly OSX, Linux and Windows (32/64bit)
set ( TARGET X86)
set ( TARGETDIR x86)
# Check headers
include ( CheckIncludeFiles )
check_include_files ( alloca.h HAVE_ALLOCA_H )
check_include_files ( dlfcn.h HAVE_DLFCN_H )
check_include_files ( inttypes.h HAVE_INTTYPES_H )
check_include_files ( memory.h HAVE_MEMORY_H )
check_include_files ( stdint.h HAVE_STDINT_H )
check_include_files ( stdlib.h HAVE_STDLIB_H )
check_include_files ( stddef.h HAVE_STDDEF_H )
check_include_files ( strings.h HAVE_STRINGS_H )
check_include_files ( string.h HAVE_STRING_H )
check_include_files ( float.h HAVE_FLOAT_H )
check_include_files ( sys/mman.h HAVE_SYS_MMAN_H )
check_include_files ( sys/stat.h HAVE_SYS_STAT_H )
check_include_files ( sys/types.h HAVE_SYS_TYPES_H )
check_include_files ( unistd.h HAVE_UNISTD_H )

include ( CheckPrototypeDefinition )
set (STDC_HEADERS 0)
if ( DEFINED HAVE_DLFCN_H
		AND DEFINED HAVE_STDINT_H
		AND DEFINED HAVE_STDDEF_H
		AND DEFINED HAVE_STDLIB_H
		AND DEFINED HAVE_INTTYPES_H
		AND DEFINED HAVE_STRINGS_H
		AND DEFINED HAVE_STRING_H
		AND DEFINED HAVE_FLOAT_H )
	check_prototype_definition(memchr "void *memchr(const void *s, int c, size_t n)" "NULL" string.h memchrExists)
	if ( ${memchrExists} )
		check_prototype_definition( free "void free(void *p)" "" stdlib.h freeExists )
		if ( ${freeExists} )
			message(STATUS "ANSI C header files - found")
			set ( STDC_HEADERS 1 CACHE INTERNAL "System has ANSI C header files" )
		endif ( )
	endif ( )
endif ( )

if(NOT STDC_HEADERS)
	message(STATUS "ANSI C header files - not found")
	set(STDC_HEADERS 0 CACHE INTERNAL "System has ANSI C header files")
endif(NOT STDC_HEADERS)

# Check functions
include ( CheckFunctionExists )
check_function_exists ( memcpy HAVE_MEMCPY )
check_function_exists ( mmap HAVE_MMAP )
if ( HAVE_MMAP )
	set ( HAVE_MMAP_ANON 1 )
	set ( HAVE_MMAP_FILE 1 )
endif ( )
check_function_exists ( alloca HAVE_ALLOCA )
# Check types
include ( CheckTypeSize )
check_type_size ( "long double" HAVE_LONG_DOUBLE )
check_type_size ( "double" SIZEOF_DOUBLE )
check_type_size ( "long double" SIZEOF_LONG_DOUBLE )
check_type_size ( "void*" SIZEOF_VOID_P )
# ASM related settings
try_compile ( HAVE_AS_CFI_PSEUDO_OP
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/as_test.c
	CMAKE_FLAGS "-DCMAKE_C_LINK_EXECUTABLE='echo not linking now...'"
	COMPILE_DEFINITIONS	-DPSEUDO_CFI)
try_compile ( HAVE_AS_ASCII_PSEUDO_OP
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/as_test.c
	CMAKE_FLAGS "-DCMAKE_C_LINK_EXECUTABLE='echo not linking now...'"
	COMPILE_DEFINITIONS	-DPSEUDO_ASCII)
try_compile ( HAVE_AS_STRING_PSEUDO_OP
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/as_test.c
	CMAKE_FLAGS "-DCMAKE_C_LINK_EXECUTABLE='echo not linking now...'"
	COMPILE_DEFINITIONS	-DPSEUDO_STRING)
try_compile ( HAVE_AS_REGISTER_PSEUDO_OP
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/as_test.c
	CMAKE_FLAGS "-DCMAKE_C_LINK_EXECUTABLE='echo not linking now...'"
	COMPILE_DEFINITIONS	-DPSEUDO_REGISTER)
try_compile ( HAVE_AS_SPARC_UA_PCREL
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/as_test.c
	CMAKE_FLAGS "-DCMAKE_C_LINK_EXECUTABLE='echo not linking now...'"
	COMPILE_DEFINITIONS	-DSPARC_UA_PCREL)
if ( MSVC )
	set ( HAVE_AS_X86_PCREL 0 )
else ( )
	try_compile ( HAVE_AS_X86_PCREL
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/as_pc.S
		CMAKE_FLAGS "-DCMAKE_ASM_LINK_EXECUTABLE='echo not linking now...'")
endif ( )

try_compile ( HAVE_AS_X86_64_UNWIND_SECTION_TYPE
	${CMAKE_CURRENT_BINARY_DIR}
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/as_x86_64_unwind.S
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/as_x86_64_unwind.c
	CMAKE_FLAGS "-DCMAKE_C_LINK_EXECUTABLE='echo not linking now...'")

# GCC related settings
if ( NOT WIN32 )
	set ( HAVE_RO_EH_FRAME 1 )
	set ( EH_FRAME_FLAGS a )
endif ( )

try_compile ( HAVE_HIDDEN_VISIBILITY_ATTRIBUTE
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests/hidden_visibility.c
	CMAKE_FLAGS "-DCMAKE_C_LINK_EXECUTABLE='echo not linking now...'")
set ( HAVE_LONG_DOUBLE_VARIANT 0 ) #only 1 on ppc-linux, ppc-freebsd and ppc-openbsd
set ( FFI_EXEC_TRAMPOLINE_TABLE 0)
# FFI Options
option ( FFI_CLOSURES "Enable closures" on )
option ( FFI_DEBUG "Compile with debug" off )
option ( FFI_NO_STRUCTS "Define this is you do not want support for aggregate types." 
	off )
option ( FFI_NO_RAW_API "Define this is you do not want support for the raw API." 
	off )
option ( USING_PURIFY "Define this if you are using Purify and want to suppress spurious messages." 
	off )
#include_directories ( ${PROJECT_BINARY_DIR} include src src/x86 )
set(SRC_C
	src/prep_cif.c
	src/types.c
	src/raw_api.c
	src/java_raw_api.c
	src/closures.c
	)

set (GCC_FLAGS "")

# Basic architecture detection
if ( SIZEOF_VOID_P MATCHES 8 )
	set ( HAVE_64BIT 1 )
else ( )
	set ( HAVE_64BIT 0 )
endif ( )

message("system_processor: ${CMAKE_SYSTEM_PROCESSOR}")
if ( "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^aarch64" )
	set ( TARGET "AARCH64" )
	set ( TARGETDIR "aarch64" )
elseif ( "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^AMD64" 
		OR
		"${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^x86_64" 
		)
	set ( TARGET "X86_64" )
	set ( TARGETDIR "x86" )
	if ( APPLE )
		set ( TARGET "X86_DARWIN" )
		if ( DEFINED CMAKE_OSX_ARCHITECTURES )
			if (x${CMAKE_OSX_ARCHITECTURES} STREQUAL "xi386") #compiling for 32-bit on 64-bit systems
				set ( FFI_ASM_FLAGS "-arch i386")
				set (	HAVE_HIDDEN_VISIBILITY_ATTRIBUTE 0 ) # with -arch i386, it complains .hidden an unknown directive
				set ( X86_DARWIN32 TRUE )
			else ( )
				set ( X86_DARWIN64 TRUE )
			endif ( )
		endif ( )
	elseif ( WIN32 OR CYGWIN OR MINGW )
		if ( HAVE_64BIT )
			set ( TARGET "X86_WIN64" )
			set ( FFI_BIT "64" )
		else ( )
			set ( TARGET "X86_WIN32" )
			set ( FFI_BIT 32 )
			set ( SYMBOL_UNDERSCORE 1 )
		endif ( )

	endif ( )
elseif ( "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^X86" )
	set ( TARGET "X86" )
	set ( TARGETDIR "x86" )
	if ( ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD"
			OR
			${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD"
			)
		set ( TARGET "X86_FREEBSD" )
	elseif ( APPLE )
		set ( TARGET "X86_DARWIN" )
		if ( DEFINED CMAKE_OSX_ARCHITECTURES )
			if (x${CMAKE_OSX_ARCHITECTURES} STREQUAL "xx86_64") #compiling for 64-bit on 32-bit systems
				set ( FFI_ASM_FLAGS "-arch x86_64")
				set ( X86_DARWIN64 TRUE )
			else ()
				set ( X86_DARWIN32 TRUE )
			endif ( )
		endif ( )
	elseif ( WIN32 OR CYGWIN OR MINGW )
		set ( TARGET "X86_WIN32" )
		set ( FFI_BIT 32 )
		set ( SYMBOL_UNDERSCORE 1 )
	endif ( )

else ( "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^arm" )
	set ( TARGET "ARM" )
	set ( TARGETDIR "arm" )
	if ( APPLE )
		set ( FFI_EXEC_TRAMPOLINE_TABLE 1 )
	endif ( )
endif ( )

if(${TARGET} STREQUAL "X86")
	set(X86 TRUE)
elseif ( ${TARGET} STREQUAL "X86_FREEBSD" )
	set( X86_FREEBSD TRUE )
elseif ( ${TARGET} STREQUAL "X86_WIN32" )
	set( X86_WIN32 TRUE )
elseif ( ${TARGET} STREQUAL "X86_WIN64" )
	set( X86_WIN64 TRUE )
elseif ( ${TARGET} STREQUAL "X86_DARWIN" )
	set( X86_DARWIN TRUE )
elseif ( ${TARGET} STREQUAL "AARCH64" )
	set ( AARCH64 TRUE )
elseif ( ${TARGET} STREQUAL "ARM" )
	set ( ARM TRUE )
elseif ( ${TARGET} STREQUAL "X86_64" )
	set ( X86_64 TRUE )
endif ( )

if ( WIN32 )
	if ( !MSVC )
		set ( GCC_FLAGS ${GCC_FLAGS} "-no-undefined" )
	endif ()

	# MSVC specific 
	set ( SRC_C ${SRC_C} src/x86/ffi.c )
	if ( MSVC)
		set ( HAVE_AS_X86_PCREL 0 )
		set ( SRC_ASM ${SRC_ASM} ${CMAKE_CURRENT_BINARY_DIR}/win${FFI_BIT}_plain.asm)
		add_custom_command(OUTPUT
			${CMAKE_CURRENT_BINARY_DIR}/win${FFI_BIT}_plain.asm
			COMMAND cl.exe /EP /P /I . /I ${CMAKE_CURRENT_SOURCE_DIR}/src/x86 /I ${CMAKE_CURRENT_SOURCE_DIR}/include /I ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/x86/win${FFI_BIT}.S /Fiwin${FFI_BIT}_plain.asm)
		set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win${FFI_BIT}_plain.asm PROPERTIES GENERATED true)
	else ( )
		set ( SRC_ASM ${SRC_ASM} src/x86/win${FFI_BIT}.S)
	endif ( )
elseif ( CYGWIN )
	set ( SYMBOL_UNDERSCORE 1 )
	set ( SRC_C ${SRC_C} src/x86/ffi.c )
	set ( SRC_ASM ${SRC_ASM} src/x86/win${FFI_BIT}.S)
endif ( )

if ( APPLE )
	set ( SYMBOL_UNDERSCORE 1 )
	set ( FFI_MMAP_EXEC_WRIT 1 )
endif ( )

if ( ${TARGET} STREQUAL X86 ) 
	set ( SRC_C ${SRC_C}
		src/x86/ffi.c
		)
	set ( SRC_ASM ${SRC_ASM}
		src/x86/sysv.S
		src/x86/win32.S
		)
endif ( )

if ( DEFINED X86_FREEBSD )
	set ( SRC_C ${SRC_C}
		src/x86/ffi.c
		)
	set ( SRC_ASM ${SRC_ASM}
		src/x86/freebsd.S
		src/x86/win32.S
		)
endif ( )

if ( DEFINED X86_64 )
	set ( SRC_C ${SRC_C}
		src/x86/ffi.c
		src/x86/ffi64.c
		)
	set ( SRC_ASM ${SRC_ASM}
		src/x86/unix64.S
		src/x86/sysv.S
		)
endif ( )

if ( DEFINED X86_DARWIN )
	set ( SRC_C ${SRC_C}
		src/x86/ffi.c
		src/x86/ffi64.c
		)
	set ( SRC_ASM ${SRC_ASM}
		src/x86/darwin.S
		src/x86/darwin64.S
		)
endif ( )

if ( DEFINED X86_DARWIN32 ) 
	set ( SRC_ASM ${SRC_ASM}
		src/x86/win32.S
		)
endif ( )

if ( DEFINED ARM ) 
	set ( HAVE_AS_X86_PCREL 0 )
	set ( SRC_C ${SRC_C} src/arm/ffi.c )
	set ( SRC_ASM ${SRC_ASM} src/arm/sysv.S )
endif ( )

if ( DEFINED AARCH64 ) 
	set ( SRC_C ${SRC_C} src/aarch64/ffi.c )
	set ( SRC_ASM ${SRC_ASM} src/aarch64/sysv.S )
endif ( )

if ( ${FFI_EXEC_TRAMPOLINE_TABLE} )
	set ( SRC_ASM ${SRC_ASM} src/arm/trampoline.S )
endif ( )

# Generate headers
configure_file ( "${PROJECT_SOURCE_DIR}/include/ffi.h.in" "${PROJECT_BINARY_DIR}/ffi.h" )
configure_file ( "${PROJECT_SOURCE_DIR}/fficonfig.cmake.in" "${PROJECT_BINARY_DIR}/fficonfig.h" )
# Use C to compile the assembly (this way preprocessor is used)
set_source_files_properties ( ${SRC_C} PROPERTIES LANGUAGE C )
if (DEFINED FFI_ASM_FLAGS)
	set_source_files_properties (${SRC_ASM} PROPERTIES COMPILE_FLAGS ${FFI_ASM_FLAGS})
endif ( )

# Compile libffi
add_library ( ffi SHARED ${SRC_C} ${SRC_ASM} )
add_library ( ffi_s STATIC ${SRC_ASM} ${SRC_C} )
set(FFI_INCLUDE_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGETDIR}
	${CMAKE_CURRENT_BINARY_DIR}
	)
include_directories(
	${FFI_INCLUDE_DIRS}
	)
set(FFI_INCLUDE_DIRS 
	${FFI_INCLUDE_DIRS}

	PARENT_SCOPE
	)
# Install
#install_library ( ffi )
#install_doc ( doc/ )
#install_header ( ${PROJECT_BINARY_DIR}/ffi.h ${PROJECT_BINARY_DIR}/fficonfig.h ${TARGET_HEADER} )
#install_data ( LICENSE README )
