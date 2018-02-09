start_sources()

sources_platform(ALL)
add_sources("Code_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "Root"
		"../Code/Plugin.cpp"
		"../Code/StdAfx.cpp"
		"../Code/Plugin.h"
		"../Code/PluginEnv.h"
		"../Code/StdAfx.h"
)
add_sources("Audio_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "Audio"
		"../Code/Audio/AudioDecoder.cpp"
		"../Code/Audio/AudioWrapper.cpp"
		"../Code/Audio/AudioDecoder.h"
		"../Code/Audio/AudioWrapper.h"
)
add_sources("Core_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "Core"
		"../Code/Core/DecoderThread.cpp"
		"../Code/Core/ThreadManager.cpp"
		"../Code/Core/Timer.cpp"
		"../Code/Core/DecoderThread.h"
		"../Code/Core/ThreadManager.h"
		"../Code/Core/Timer.h"
)
add_sources("FlowNodes_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "FlowNodes"
		"../Code/FlowNodes/FlowNodes.cpp"
)
add_sources("Input_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "Input"
		"../Code/Input/InputDispatcher.cpp"
		"../Code/Input/InputDispatcher.h"
)
add_sources("Render_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "Render"
		"../Code/Render/Frame.cpp"
		"../Code/Render/FrameBuffer.cpp"
		"../Code/Render/RenderWrapper.cpp"
		"../Code/Render/Frame.h"
		"../Code/Render/FrameBuffer.h"
		"../Code/Render/RenderWrapper.h"
)
add_sources("Schematyc_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "Schematyc"
		"../Code/Schematyc/TextureVideoPlayerComponent.cpp"
		"../Code/Schematyc/TextureVideoPlayerComponent.h"
)
add_sources("Utils_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "Utils"
		"../Code/Utils/Buffer.h"
		"../Code/Utils/ObjectsPool.h"
		"../Code/Utils/PacketQueue.h"
		"../Code/Utils/SafeQueue.h"
)
add_sources("Video_uber.cpp"
    PROJECTS CryVideoPlugin
    SOURCE_GROUP "Video"
		"../Code/Video/2DVideoQueue.cpp"
		"../Code/Video/Packet.cpp"
		"../Code/Video/TextureVideoQueue.cpp"
		"../Code/Video/VideoFileReader.cpp"
		"../Code/Video/VideoPlayer.cpp"
		"../Code/Video/2DVideoQueue.h"
		"../Code/Video/Packet.h"
		"../Code/Video/TextureVideoQueue.h"
		"../Code/Video/VideoFileReader.h"
		"../Code/Video/VideoPlayer.h"
)

end_sources()

CryEngineModule(CryVideoPlayer PCH "StdAfx.cpp" SOLUTION_FOLDER "CustomPlugins")

target_include_directories(${THIS_PROJECT} 
	PRIVATE 
		"${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
		"${CRYENGINE_DIR}/Code/CryEngine/CryAction"
		"${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
		"${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

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
	${LIBAV_INCLUDE}
	../Code)

if(NOT OPTION_STATIC_LINKING)
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
else()
	set_property(TARGET ${THIS_PROJECT} PROPERTY INTERFACE_LINK_LIBRARIES 
		${LIBWEBM_LIB}/release/libwebm.lib
		${LIBVPX_LIB}/libvpx.lib
		${LIBOGG_LIB}/release/libogg_static.lib
		${LIBVORBIS_LIB}/release/libvorbis_static.lib
		${LIBVORBIS_LIB}/release/libvorbisfile_static.lib
		${SDL2_LIB}/SDL2main.lib
		${SDL2_LIB}/SDL2.lib
		${LIBAV_LIB}/swscale.lib)
endif()