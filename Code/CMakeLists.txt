#BEGIN-CUSTOM
set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE) 

add_definitions("/wd4996 /wd4800")
set_target_properties(${THIS_PROJECT} PROPERTIES LINK_FLAGS "/ignore:4099 /ignore:4075 /ignore:4098")

# Core
set(CORE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../Includes)

# libvpx
set(LIBVPX_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libvpx/includes)
set(LIBVPX_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libvpx/lib)

# libwebm
set(LIBWEBM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libwebm/includes)
set(LIBWEBM_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libwebm/lib)

# libogg
set(LIBOGG_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libogg/includes)
set(LIBOGG_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libogg/lib)

# libvorbis
set(LIBVORBIS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libvorbis/includes)
set(LIBVORBIS_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libvorbis/lib)

# SDL2
set(SDL2_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/SDL2/includes)
set(SDL2_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/SDL2/lib)

# libav
set(LIBAV_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libav/include)
set(LIBAV_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libav/bin)

include_directories(
	${CORE_INCLUDE} 
	${LIBVPX_INCLUDE} 
	${LIBWEBM_INCLUDE} 
	${LIBOGG_INCLUDE} 
	${LIBVORBIS_INCLUDE} 
	${SDL2_INCLUDE} 
	${LIBAV_INCLUDE})

target_link_libraries(${THIS_PROJECT} PRIVATE 
	optimized ${LIBWEBM_LIB}/release/libwebm.lib
	debug ${LIBWEBM_LIB}/debug/libwebm.lib
	${LIBVPX_LIB}/libvpx.lib
	optimized ${LIBOGG_LIB}/release/libogg_static.lib
	debug ${LIBOGG_LIB}/debug/libogg_static.lib
	optimized ${LIBVORBIS_LIB}/release/libvorbis_static.lib
	debug ${LIBVORBIS_LIB}/debug/libvorbis_static.lib
	optimized ${LIBVORBIS_LIB}/release/libvorbisfile_static.lib
	debug ${LIBVORBIS_LIB}/debug/libvorbisfile_static.lib
	${SDL2_LIB}/SDL2main.lib
	${SDL2_LIB}/SDL2.lib
	${LIBAV_LIB}/swscale.lib)

set_solution_startup_target(${THIS_PROJECT})

#END-CUSTOM