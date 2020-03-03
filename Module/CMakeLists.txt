if (WINDOWS AND OPTION_ENGINE AND NOT OPTION_DEDICATED_SERVER)
	option(PLUGIN_CEVPLAYER "Enable CEVPlayer plug-in for video playing" ON)
else()
	option(PLUGIN_CEVPLAYER "Enable CEVPlayer plug-in for video playing" OFF)
endif()
	
if (NOT PLUGIN_CEVPLAYER)
	return()
endif()

start_sources()

sources_platform(ALL)
add_sources("NoUberFile"
    SOURCE_GROUP "Root"
		"Plugin.cpp"
		"StdAfx.cpp"
		"Plugin.h"
		"PluginEnv.h"
		"StdAfx.h"
)
add_sources("CEVPlayer_audio_uber_0.cpp"
    SOURCE_GROUP "Audio"
		"Audio/AudioDecoder.cpp"
		"Audio/AudioWrapper.cpp"
		"Audio/AudioDecoder.h"
		"Audio/AudioWrapper.h"
)
add_sources("CEVPlayer_core_uber_0.cpp"
    SOURCE_GROUP "Core"
		"Core/DecoderThread.cpp"
		"Core/ThreadManager.cpp"
		"Core/Timer.cpp"
		"Core/DecoderThread.h"
		"Core/ThreadManager.h"
		"Core/Timer.h"
)
add_sources("CEVPlayer_flownodes_uber_0.cpp"
    SOURCE_GROUP "FlowNodes"
		"FlowNodes/FlowNodes.cpp"
)
add_sources("CEVPlayer_input_uber_0.cpp"
    SOURCE_GROUP "Input"
		"Input/InputDispatcher.cpp"
		"Input/InputDispatcher.h"
)
add_sources("CEVPlayer_render_uber_0.cpp"
    SOURCE_GROUP "Render"
		"Render/Frame.cpp"
		"Render/FrameBuffer.cpp"
		"Render/RenderWrapper.cpp"
		"Render/Frame.h"
		"Render/FrameBuffer.h"
		"Render/RenderWrapper.h"
)
add_sources("CEVPlayer_components_uber_0.cpp"
    SOURCE_GROUP "Components"
		"Components/TextureVideoPlayerComponent.cpp"
		"Components/TextureVideoPlayerComponent.h"
		"Components/ILightComponent.h"
)
add_sources("CEVPlayer_utils_uber_0.cpp"
    SOURCE_GROUP "Utils"
		"Utils/Buffer.h"
		"Utils/ObjectsPool.h"
		"Utils/PacketQueue.h"
		"Utils/SafeQueue.h"
)
add_sources("CEVPlayer_video_uber_0.cpp"
    SOURCE_GROUP "Video"
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

end_sources()

CryEngineModule(CEVPlayer FORCE_SHARED_WIN PCH "StdAfx.cpp" SOLUTION_FOLDER "_SpaceRaccoon_/Plugins")

target_include_directories(${THIS_PROJECT} 
	PRIVATE 
		"${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
		"${CRYENGINE_DIR}/Code/CryEngine/CryAction"
		"${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
	PUBLIC
		../Interface
)

add_definitions("/wd4996 /wd4800")
set_target_properties(${THIS_PROJECT} PROPERTIES LINK_FLAGS "/ignore:4099 /ignore:4075 /ignore:4098")

# libvpx
set(LIBVPX_INCLUDE "${SDK_DIR}/_CustomSDKs/libvpx/includes")
set(LIBVPX_LIB "${SDK_DIR}/_CustomSDKs/libvpx/lib")

# libwebm
set(LIBWEBM_INCLUDE "${SDK_DIR}/_CustomSDKs/libwebm/includes")
set(LIBWEBM_LIB "${SDK_DIR}/_CustomSDKs/libwebm/lib")

# libogg
set(LIBOGG_INCLUDE "${SDK_DIR}/_CustomSDKs/libogg/includes")
set(LIBOGG_LIB "${SDK_DIR}/_CustomSDKs/libogg/lib")

# libvorbis
set(LIBVORBIS_INCLUDE "${SDK_DIR}/_CustomSDKs/libvorbis/includes")
set(LIBVORBIS_LIB "${SDK_DIR}/_CustomSDKs/libvorbis/lib")

# libav
set(LIBAV_INCLUDE "${SDK_DIR}/_CustomSDKs/libav/include")
set(LIBAV_LIB "${SDK_DIR}/_CustomSDKs/libav/bin")

include_directories( 
	${LIBVPX_INCLUDE} 
	${LIBWEBM_INCLUDE} 
	${LIBOGG_INCLUDE}
	${LIBVORBIS_INCLUDE} 
	${LIBAV_INCLUDE})
	
#if(NOT OPTION_STATIC_LINKING)
	target_link_libraries(${THIS_PROJECT} PRIVATE 
		${LIBWEBM_LIB}/release/libwebm.lib
		${LIBVPX_LIB}/libvpx.lib
		${LIBOGG_LIB}/release/libogg_static.lib
		${LIBVORBIS_LIB}/release/libvorbis_static.lib
		${LIBVORBIS_LIB}/release/libvorbisfile_static.lib
		${LIBAV_LIB}/swscale.lib)
#else()
#	set_property(TARGET ${THIS_PROJECT} PROPERTY INTERFACE_LINK_LIBRARIES 
#		${LIBWEBM_LIB}/release/libwebm.lib
#		${LIBVPX_LIB}/libvpx.lib
#		${LIBOGG_LIB}/release/libogg_static.lib
#		${LIBVORBIS_LIB}/release/libvorbis_static.lib
#		${LIBVORBIS_LIB}/release/libvorbisfile_static.lib
#		${LIBAV_LIB}/swscale.lib)
#endif() 

target_link_libraries(${THIS_PROJECT} PRIVATE SDL2)

deploy_runtime_files("${LIBAV_LIB}/avutil-54.dll")
deploy_runtime_files("${LIBAV_LIB}/swscale-3.dll")