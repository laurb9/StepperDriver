cmake_minimum_required(VERSION 3.24)
idf_component_register(
    SRC_DIRS "src"  # Add all source files here
    INCLUDE_DIRS "src" 
    REQUIRES  "arduino" # Add include directories here
)