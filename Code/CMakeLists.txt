cmake_minimum_required (VERSION 3.6.2)
set(CRYENGINE_DIR "G:/_Development/CustomCryEngine/Crytek/CRYENGINE_5.5")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/../")

include("${TOOLS_CMAKE_DIR}/CommonOptions.cmake")

add_subdirectory("${CRYENGINE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CRYENGINE")

include("${TOOLS_CMAKE_DIR}/Configure.cmake")
start_sources()

sources_platform(ALL)
add_sources("Code_uber.cpp"
    PROJECTS CEVPlayer
    SOURCE_GROUP "Root"
		"Plugin.cpp"
		"StdAfx.cpp"
		"Plugin.h"
		"PluginEnv.h"
		"StdAfx.h"
)
add_sources("Audio_uber.cpp"
    PROJECTS CEVPlayer
    SOURCE_GROUP "Audio"
		"Audio/AudioDecoder.cpp"
		"Audio/AudioWrapper.cpp"
		"Audio/AudioDecoder.h"
		"Audio/AudioWrapper.h"
)
add_sources("Core_uber.cpp"
    PROJECTS CEVPlayer
    SOURCE_GROUP "Core"
		"Core/DecoderThread.cpp"
		"Core/ThreadManager.cpp"
		"Core/Timer.cpp"
		"Core/DecoderThread.h"
		"Core/ThreadManager.h"
		"Core/Timer.h"
)
add_sources("FlowNodes_uber.cpp"
    PROJECTS CEVPlayer
    SOURCE_GROUP "FlowNodes"
		"FlowNodes/FlowNodes.cpp"
)
add_sources("Input_uber.cpp"
    PROJECTS CEVPlayer
    SOURCE_GROUP "Input"
		"Input/InputDispatcher.cpp"
		"Input/InputDispatcher.h"
)
add_sources("Render_uber.cpp"
    PROJECTS CEVPlayer
    SOURCE_GROUP "Render"
		"Render/Frame.cpp"
		"Render/FrameBuffer.cpp"
		"Render/RenderWrapper.cpp"
		"Render/Frame.h"
		"Render/FrameBuffer.h"
		"Render/RenderWrapper.h"
)
add_sources("Schematyc_uber.cpp"
    PROJECTS CEVPlayer
    SOURCE_GROUP "Schematyc"
		"Schematyc/TextureVideoPlayerComponent.cpp"
		"Schematyc/TextureVideoPlayerComponent.h"
)
add_sources("Utils_uber.cpp"
    PROJECTS CEVPlayer
    SOURCE_GROUP "Utils"
		"Utils/Buffer.h"
		"Utils/ObjectsPool.h"
		"Utils/PacketQueue.h"
		"Utils/SafeQueue.h"
)
add_sources("Video_uber.cpp"
    PROJECTS CEVPlayer
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

CryEngineModule(CEVPlayer FORCE_SHARED PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE
    "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
    "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
    "${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
    "${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)


if(OPTION_ENGINE)
    if(NOT EXISTS "${CRYENGINE_DIR}/Code/Sandbox/EditorQt")
		add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
		set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
		if (WIN32)
			set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}CEVPlayer.cryproject\"")
		endif()
	endif()
else()
	add_library(GameLauncher STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
	set_target_properties(GameLauncher PROPERTIES LINKER_LANGUAGE CXX)
	if (WIN32)
		set_visual_studio_debugger_command(GameLauncher "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}CEVPlayer.cryproject\"")
	endif()

    add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}CEVPlayer.cryproject\"")
    endif()

	add_library(GameServer STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
	set_target_properties(GameServer PROPERTIES LINKER_LANGUAGE CXX)
	if (WIN32)
		set_visual_studio_debugger_command(GameServer "${CRYENGINE_DIR}/bin/win_x64/Game_Server.exe" "-project \"${PROJECT_DIR}CEVPlayer.cryproject\"")
	endif()
endif()

# Set StartUp project in Visual Studio
set_solution_startup_target(GameLauncher)

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}CEVPlayer.cryproject\"" )
endif()

#BEGIN-CUSTOM
set(CMAKE_CONFIGURATION_TYPES "Profile" CACHE STRING "" FORCE) 

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
	${LIBWEBM_LIB}/release/libwebm.lib
	${LIBVPX_LIB}/libvpx.lib
	${LIBOGG_LIB}/release/libogg_static.lib
	${LIBVORBIS_LIB}/release/libvorbis_static.lib
	${LIBVORBIS_LIB}/release/libvorbisfile_static.lib
	${SDL2_LIB}/SDL2main.lib
	${SDL2_LIB}/SDL2.lib
	${LIBAV_LIB}/swscale.lib)

set_solution_startup_target(${THIS_PROJECT})

#END-CUSTOM