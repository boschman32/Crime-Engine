#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <functional>
#include <memory>
#include <utility>

//Tmxlite
#include "tmxlite/Map.hpp"
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/Tileset.hpp>

//eventpp
#include <eventpp/eventdispatcher.h>
#include <eventpp/eventqueue.h>
#include <eventpp/callbacklist.h>
#include <eventpp/mixins/mixinfilter.h>

//Optick
#include <optick.h>

//spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>

//GLM
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
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>
#endif