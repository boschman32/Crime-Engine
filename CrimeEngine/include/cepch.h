#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <filesystem>

#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iterator>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <future>

// Header files
#include "Core/Logger.h"

//Libraries

//Tmxlite
#include "tmxlite/Map.hpp"
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/Tileset.hpp>

//spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>

//Sol
#include <sol/sol.hpp>

//FMOD
//#include <fmod_studio.hpp>

//Optick
#include <optick.h>
#include <optick.config.h>

//eventpp
#include <eventpp/eventqueue.h>
#include <eventpp/eventdispatcher.h>
#include <eventpp/callbacklist.h>
#include <eventpp/mixins/mixinfilter.h>

//FontAwesome
#include <FontAwesome/FontAwesome.h>

////GLM
#include <vec2.hpp>
#include <vec3.hpp>
#include <matrix.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_inverse.hpp>
#include <gtx/matrix_major_storage.hpp>

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <gtx/string_cast.hpp>
#include <gtx/quaternion.hpp>
#pragma warning( pop )

#ifdef PLATFORM_WINDOWS
//Imgui
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

//includes for rendering
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <fstream>

#include <Xinput.h>
#include <shellapi.h> // For CommandLineToArgvW

#include <wrl.h>

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d3d12sdklayers.h>
#include <d3dx12.h>
#include <DirectXCollision.h>

#include <filesystem>

#include "Platform/Windows/DX12_Helper.h"

#elif PLATFORM_SWITCH

#include "nvn/nvn.h"
#include "nvn/nvn_Cpp.h"

#include "nvn/nvn_FuncPtrGlobal.h"

#include <atomic>

#include <nn/nn_SdkAssert.h>
#include <nn/init.h>
#include <nn/os.h>
#include <nn/nn_TimeSpan.h>
#include <nn/nn_Common.h>
#include <nn/nn_Log.h>
#include <nn/nn_Assert.h>
#include <nn/fs.h>
#include <nn/vi.h>
#include <nn/hid.h>
#include <nn/oe/oe_DisplayResolutionApis.h>

#include <nv/nv_MemoryManagement.h>

#include "Platform/Switch/NVNHelper.h"
#endif