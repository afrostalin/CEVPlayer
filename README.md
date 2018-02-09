
# Short description
**CryVideoPlayer** - plugin for CRYENGINE V allowing you to play video on the screen and on the game objects

**It's includes :** 
* Flowgraph nodes for creating video player (Screen video / Texture video)
* VideoPlayerComponent for easy integration with new components system
* Schematic functions for easy creation schematic entitys with video player component
* Example project for fast study functionality 
* Full source code 
* Absolutly free license

# Building
## Before compiling
* Go to 3rd folder and unpack `3rd.7z` archive
* Also you need installed CRYENGINE V
## Compiling
* Use CryVideoPlugin.cryproject for generation solution
* Open solution with Visual Studio and build it

# Using
* After compiling plugin go to `bin/win_x64` folder and copy plugin to you project bin folder
* Also you need copy `avutil-54.dll` and `swscale-3.dll` from `3rd/libav/bin` folder to you project bin folder and also to `CRYENGINE/bin/win_x64` folder
* Add CryVideoPlayer to you .cryproject file

```bash
{
    "type": "EPluginType::Native",
    "path": "bin/win_x64/CryVideoPlugin.dll"
}
```
* That's all. Now you can start sandbox and create logic for playing video in you game

# Additional information
**Warning №1 : CryVideoPlayer uses code of <a href=https://libav.org>Libav</a> licensed under the <a href=https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>LGPLv2.1</a> and its source and binaries can be downloaded <a href=http://builds.libav.org/windows/>here</a>**

**Warning №2 : CryVideoPlayer compatible only with CRYENGINE 5.4 now**

**Warning №3 : CryVideoPlayer can play video only in .webm format**

**Warning №4 : All video files should be in `Videos` folder**

**Warning №5 : TextureVideoPlayer not playing audio - use audio triggers for this**

**Warning №6 : Don't use for TextureVideoPlayer video in very hight resolution - it's can be perfomance problems (optimal HD and low)**

**Warning №7 : Don't play video when level loading - you game will be crash!**

# TODO
* Add ability to play video when level loading (Problem with blocking OnPostUpdate event)
* Maybe create special shader for YUV->RGBA conversion
* Audio triggers for in-game video
* Make plugin compatible with CRYENGINE 5.3

# WIKI and lessons
Please see [WIKI](https://github.com/afrostalin/CryVideoPlayer/wiki)

Fast study functionality with [example project](https://github.com/afrostalin/VideoPluginExample) project

# GitHub community

I will be happy with any help from the community, so I'm waiting for your pull requests 

If you have any problem with this plugin please use [GitHub issues](https://github.com/afrostalin/CryVideoPlayer/issues) mechanism

# Contacts

If you need more from me, write me directly to my email : chernecoff@gmail.com
