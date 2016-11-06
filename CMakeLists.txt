########################################################################################################################
# CMakefile para compilar la librería thdanalyzer y los programas de ejemplo asociados.
#
# Para compilar hay que instalar previamente la herramienta cmake 2.8+, disponible para Linux, Windows y Mac, una vez
# instalada, hacemos:
#
# mkdir -p build
# cd build
# cmake ..
# make
########################################################################################################################
PROJECT(cpputils)
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.6)
set (CMAKE_LEGACY_CYGWIN_WIN32 0)
set (CMAKE_VERBOSE_MAKEFILE on)

add_definitions ("-Wall -g -std=c++11")


add_executable(test_url src/test_url.cpp src/url.cpp)
target_link_libraries(test_url m pthread)

add_executable(test_path src/test_path.cpp src/path.cpp src/str.cpp)
target_link_libraries(test_path m pthread)

add_executable(test_query_string src/test_query_string.cpp src/query_string.cpp)
target_link_libraries(test_query_string m pthread)

