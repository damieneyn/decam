cmake_minimum_required(VERSION 2.8)

project(decam)

find_package( OpenCV REQUIRED )

if (OpenCV_FOUND)
  include_directories(${OpenCV_INCLUDE_DIRS})
endif (OpenCV_FOUND)


# ---- Architecture Option ----
option(DE_MULTI_THREAD "Set ON to Build with MultiThreading" ON)

# ---- Packages Option ----

# ---- Distribution Option ----
option(DE_MAKE_EXE "Set ON to Build an Executable" ON)
option(DE_CUSTOMER_RELEASE "Set ON to Build a Customer Release" OFF)


add_subdirectory(sources)
