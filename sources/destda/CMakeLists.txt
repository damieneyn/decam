set(destda_source_files
	destda.cpp
)

add_executable(
	destda
	${destda_source_files}
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


if(DE_CAMERA)
	set(DEPS_DESTDA decam)
endif()

target_link_libraries(destda decam ${DEPS_SESTDA})
