cmake_minimum_required(VERSION 3.14)
project(shape_growth_cadmium CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Dynamic factory signature selection ----
# You're using make_dynamic_atomic_model<Atomic, TIME>(...)
add_definitions(-DCADMIUM_FACTORY_AL_TM)
# ---------------------------------------------

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/../cadmium/include
  ${CMAKE_SOURCE_DIR}/../DESTimes/include
)

file(GLOB SRC src/*.cpp)
add_executable(shape_growth ${SRC})
