cmake_minimum_required (VERSION 3.6.0)
project (CryVideoPlayer VERSION 1.0 LANGUAGES CXX)
option(OPTION_PCH "Enable Precompiled headers" ON)
find_package (CryCommon REQUIRED)
find_package (CryAction REQUIRED)

set (SourceGroup_Root
	"Plugin.cpp"
		"StdAfx.cpp"
		"Plugin.h"
		"PluginEnv.h"
		"StdAfx.h"
)

set (SourceGroup_Audio
	"Audio/AudioDecoder.cpp"
	"Audio/AudioWrapper.cpp"
	"Audio/AudioDecoder.h"
	"Audio/AudioWrapper.h"
)

set (SourceGroup_Core
	"Core/DecoderThread.cpp"
	"Core/ThreadManager.cpp"
	"Core/Timer.cpp"
	"Core/DecoderThread.h"
	"Core/ThreadManager.h"
	"Core/Timer.h"
)

set (SourceGroup_Flownodes
	"FlowNodes/FlowNodes.cpp"
)

set (SourceGroup_Input
	"Input/InputDispatcher.cpp"
	"Input/InputDispatcher.h"
)

set (SourceGroup_Render
	"Render/Frame.cpp"
	"Render/FrameBuffer.cpp"
	"Render/RenderWrapper.cpp"
	"Render/Frame.h"
	"Render/FrameBuffer.h"
	"Render/RenderWrapper.h"
)

set (SourceGroup_Utils
	"Utils/Buffer.h"
	"Utils/ObjectsPool.h"
	"Utils/PacketQueue.h"
	"Utils/SafeQueue.h"
)

set (SourceGroup_Video
	"Video/2DVideoQueue.cpp"
	"Video/Packet.cpp"
	"Video/TextureVideoQueue.cpp"
	"Video/VideoFileReader.cpp"
	"Video/VideoPlayer.cpp"
	"Video/2DVideoQueue.h"
	"Video/Packet.h"
	"Video/TextureVideoQueue.h"
	"Video/VideoFileReader.h"
	"Video/VideoPlayer.h"
)

source_group("Main" FILES ${SourceGroup_Root})
source_group("Audio" FILES ${SourceGroup_Audio})
source_group("Core" FILES ${SourceGroup_Core})
source_group("FlowNodes" FILES ${SourceGroup_Flownodes})
source_group("Input" FILES ${SourceGroup_Input})
source_group("Render" FILES ${SourceGroup_Render})
source_group("Utils" FILES ${SourceGroup_Utils})
source_group("Video" FILES ${SourceGroup_Video})


set (SOURCE
	${SourceGroup_Root}
	${SourceGroup_Audio}
	${SourceGroup_Core}
	${SourceGroup_Flownodes}
	${SourceGroup_Input}
	${SourceGroup_Render}
	${SourceGroup_Utils}
	${SourceGroup_Video}
)

set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE) 

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
	set (PRECOMPILED_SOURCE ${SOURCE})
	list(REMOVE_ITEM PRECOMPILED_SOURCE "StdAfx.cpp")

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
	${LIBOGG_LIB}/debug/libogg_static.lib
	${LIBVORBIS_LIB}/debug/libvorbis_static.lib
	${LIBVORBIS_LIB}/debug/libvorbisfile_static.lib
	${SDL2_LIB}/SDL2main.lib
	${SDL2_LIB}/SDL2.lib
	${LIBAV_LIB}/swscale.lib)

if (WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE Ntdll)
endif(WIN32)