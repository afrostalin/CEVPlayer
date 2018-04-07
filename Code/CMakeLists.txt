source_group("Audio" FILES ${SourceGroup_Audio})
source_group("Core" FILES ${SourceGroup_Core})
source_group("FlowNodes" FILES ${SourceGroup_Flownodes})
source_group("Input" FILES ${SourceGroup_Input})
source_group("Render" FILES ${SourceGroup_Render})
source_group("Utils" FILES ${SourceGroup_Utils})
source_group("Video" FILES ${SourceGroup_Video})
	${SourceGroup_Render}
	${SourceGroup_Utils}
	${SourceGroup_Video}
#BEGIN-CUSTOM
set(CMAKE_CONFIGURATION_TYPES "Profile" CACHE STRING "" FORCE) 

add_definitions("/wd4996 /wd4800")
set_target_properties(${THIS_PROJECT} PROPERTIES LINK_FLAGS "/ignore:4099 /ignore:4075 /ignore:4098 /ignore:4005")

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

if(OPTION_PCH AND MSVC)
	${LIBWEBM_LIB}/release/libwebm.lib

	set (PRECOMPILED_BINARY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pch)
	set_source_files_properties (${PRECOMPILED_SOURCE} PROPERTIES COMPILE_FLAGS "/Yu\"StdAfx.h\" /Fp\"${PRECOMPILED_BINARY}\"" OBJECT_DEPENDS "${PRECOMPILED_BINARY}")
	set_source_files_properties (StdAfx.cpp PROPERTIES COMPILE_FLAGS "/Yc\"StdAfx.h\" /Fp\"${PRECOMPILED_BINARY}\"" OBJECT_OUTPUTS "${PRECOMPILED_BINARY}")
endif (OPTION_PCH AND MSVC)

add_library(${PROJECT_NAME} MODULE ${SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE 
	CryCommon
	CryAction
	${LIBWEBM_LIB}/debug/libwebm.lib
	${LIBVPX_LIB}/libvpx.lib
	${LIBOGG_LIB}/release/libogg_static.lib
	${LIBVORBIS_LIB}/release/libvorbis_static.lib
	${LIBVORBIS_LIB}/release/libvorbisfile_static.lib
	debug ${LIBVORBIS_LIB}/debug/libvorbis_static.lib
	optimized ${LIBVORBIS_LIB}/release/libvorbisfile_static.lib
	debug ${LIBVORBIS_LIB}/debug/libvorbisfile_static.lib
	${SDL2_LIB}/SDL2main.lib
	${SDL2_LIB}/SDL2.lib
	${LIBAV_LIB}/swscale.lib)

if (WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE Ntdll)
endif(WIN32)