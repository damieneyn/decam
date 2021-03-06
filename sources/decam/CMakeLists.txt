
# decam
set(decam_source_files 	
	dueyeOpenCv.cpp
	dueye.cpp
	dueye.h
	
	CACHE INTERNAL ""
)
set(decam_source_dirs "" CACHE INTERNAL "")
source_group("" FILES ${decam_source_files})

# ---- check compiler version ----
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


# ---- configure compiler warnings ----
if(DE_COMPILER MATCHES "GCC")
	add_definitions(-Wchar-subscripts -Wformat -Wformat-y2k -Wformat-security -Wmissing-braces -Wparentheses -Wswitch -Wunused-label -Wunused-variable -Wfloat-equal -Wcast-align -Wsign-compare -Wsequence-point )
	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "3.2.4")
		add_definitions(-Winit-self -Wextra -std=gnu++11)
	endif()
	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.2.0")
		add_definitions(-Waddress -Wlogical-op  -Wvla -Warray-bounds ) #-Wc++0x-compat
	endif()
endif()
if(DE_COMPILER MATCHES "MSC")
	add_definitions(/W4 /wd4100 /wd4702 /wd4389 /wd4355 /wd4211 /wd4355 /wd4065 ) #Warning as error : /WX )
endif()
if(DE_COMPILER MATCHES "ICC")
	add_definitions(-Werror -wd654 -wd1125 -wd177)
endif()

# ---- configure MSVC ----
if(MSVC)
	foreach(flag_var
	CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
	CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
	# remove /W3
	string(REGEX REPLACE "/W3" "" ${flag_var} "${${flag_var}}") 
	endforeach(flag_var)
endif()


add_library(
	decam
	STATIC
	${decam_source_files}
)


if(DE_OS MATCHES "LINUX")
	set (UEYE_LIBS ueye_api)
else()
	if(DE_ARCH MATCHES "X64")
		set (UEYE_LIBS uEye_api_64 uEye_tools_64)
	else()
		set (UEYE_LIBS uEye_api uEye_tools)
	endif()
endif()

target_link_libraries(decam ${OpenCV_LIBRARIES} ${UEYE_LIBS})


