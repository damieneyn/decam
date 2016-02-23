# ---- Auto-configure system flags ----
# 1 - Architecture
if(CMAKE_CL_64 OR CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
	set(DETECTED_ARCH "X64")
else()
	set(DETECTED_ARCH "X86")
endif()
set(DE_ARCH ${DETECTED_ARCH} CACHE STRING "Name of the targetted processor architecture (X86,X64)")

# 2 - Operating System
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(DETECTED_OS "WIN32")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux") 
	set(DETECTED_OS "LINUX")
else()
	set(DETECTED_OS "Undefined")
endif()
set(DE_OS ${DETECTED_OS} CACHE STRING "Name of the targetted operating system (LINUX,WIN32)")

# 3 - Compiler
if(CMAKE_COMPILER_IS_GNUCC)
	set(DETECTED_COMPILER "GCC")
elseif(MSVC)
	set(DETECTED_COMPILER "MSC")
endif()
set(DE_COMPILER ${DETECTED_COMPILER} CACHE STRING "Name of the compiler used to build DE (MSC,GCC,ICC,ACC)")

# ---- configure MSVC ----
if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	foreach(flag_var
	CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
	CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)

	# replace /MD by /MT
	if(${flag_var} MATCHES "/MD")
	string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
	endif(${flag_var} MATCHES "/MD") 

	endforeach(flag_var)

	# enable parallelism
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

	add_definitions(/wd4355) 
endif()

# ---- configure GCC ----
if(DE_COMPILER MATCHES "GCC")
	# enable position independent code except on windows because with mingw it is activated by default
if(NOT DE_OS MATCHES "WIN32") 
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif() 
endif()

# ---- configure header directories ----
include_directories(.)
include_directories(../dependencies/include)

# ---- configure output directory ----
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# ---- configure library directories ----
# TODO : use same name for dependencies directories
if(DE_ARCH MATCHES "X86")
	set(arch_id "x86")
endif()
if(DE_ARCH MATCHES "X64")
	set(arch_id "x64")
endif() 

if(DE_OS MATCHES "LINUX")
	set(system_id "l")
endif()
if(DE_OS MATCHES "WIN32")
	set(system_id "w")
endif() 

if(DE_COMPILER MATCHES "GCC")
	set(compiler_id "gcc")
endif()

#
#message( STATUS "MSVC_VERSION: " ${MSVC_VERSION} )
#Visual C++, 32-bit, version 6.0 1200
#Visual C++, 32-bit, version .net 2002 1300
#Visual C++, 32-bit, version .net 2003 1310
#Visual C++, 32-bit, version 2005 1400 (vc80)
#Visual C++, 32-bit, version 2005 SP1 14?? (vc80_sp1)
#Visual C++, 32-bit, version 2008 1500 (vc90)
#MSVC90 (Visual C++ 2008 SP1) MSVC_VERSION=1500 (Compiler Version 15.00.30729.01)
#MSVC100 (Visual C++ 2010) MSVC_VERSION=1600 (Compiler Version 16.00.30319.01)
#MSVC110 (Visual C++ 2011) MSVC_VERSOIN=1700
#MSVC120 (Visual C++ 2013) MSVC_VERSION=1800 (Compiler Version 18.00.21005.01)
#MESSAGE(STATUS " MSVC 71 " ${MSVC71})
#if(MSVC_VERSION EQUAL 1310 OR MSVC_VERSION EQUAL 1400 OR MSVC_VERSION EQUAL 1500 OR MSVC_VERSION EQUAL 1600 OR MSVC_VERSION EQUAL 1700 OR MSVC_VERSION EQUAL 1800 )

if(DE_COMPILER MATCHES "MSC")
#MESSAGE(${MSVC_VERSION})
	if(MSVC_VERSION EQUAL 1500)
		set(compiler_id "msc9")
	elseif(MSVC_VERSION EQUAL 1600)
		set(compiler_id "msc10")
	elseif(MSVC_VERSION EQUAL 1700)
		set(compiler_id "msc11")
	elseif(MSVC_VERSION EQUAL 1800)
		set(compiler_id "msc12")	
	else()
		MESSAGE(error: VISUAL STUDIO VERSION NOT SUPPORTED - recompile dependancies)
	endif() 
endif() 
set(DE_DEP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/lib/${arch_id}_${system_id}_${compiler_id})
link_directories(${DE_DEP_DIR})
include_directories(${OpenCV_INCLUDE_DIR})

# ---- configure required build flags ----
add_definitions(-DCOMPILER_${DE_COMPILER})
add_definitions(-DOS_${DE_OS})
add_definitions(-DARCH_${DE_ARCH})

# ---- adding system files ----
#add_subdirectory(desys)

# ---- configure build ----
if(DE_CUSTOMER_RELEASE)
	add_definitions(-DCUSTOMER_RELEASE)
	set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")
	set(CMAKE_C_FLAGS_DISTRIBUTION "-O3") 
endif()


if(DE_MAKE_EXE)
	add_subdirectory(destda)
endif()

# --- configure packages ---
#if(DE_CAMERA)
	add_subdirectory(decam)
#endif()

	
# ---- configure optimization flags ----
if(DE_MULTI_THREAD)
	add_definitions(-DMULTI_THREADING)
endif()


# ---- configure output directory ----
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#--------------

