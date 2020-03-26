#pragma once

// Smart pointers
#include <memory>
template <class T>
using Ptr = std::unique_ptr<T>;
template <class T>
using Ref = std::shared_ptr<T>;
template <class T>
using Weak = std::weak_ptr<T>;
#include <wrl.h> // For using Microsoft::WRL::ComPtr<T>

// Serialization streams
#include <fstream>
typedef std::fstream InputOutputFileStream;
typedef std::ofstream OutputFileStream;
typedef std::ifstream InputFileStream;
#include <sstream>
typedef std::stringstream StringStream;

// Containers
#include <string>
typedef std::string String;

#include <map>
template <class P, class Q>
using Map = std::map<P, Q>;

#include <unordered_map>
template <class P, class Q>
using HashMap = std::unordered_map<P, Q>;

#include <utility>
template <typename...P>
using Tuple = std::tuple<P...>;
template <class P, class Q>
using Pair = std::pair<P, Q>;

#include <vector>
template <class T>
using Vector = std::vector<T>;

#include <filesystem>
using FilePath = std::filesystem::path;

// Math Containers
#include <d3d11.h>
#include "vendor/DirectXTK/Inc/SimpleMath.h"
typedef DirectX::SimpleMath::Matrix Matrix;
typedef DirectX::SimpleMath::Vector2 Vector2;
typedef DirectX::SimpleMath::Vector3 Vector3;
typedef DirectX::SimpleMath::Vector4 Vector4;
typedef DirectX::SimpleMath::Quaternion Quaternion;
typedef DirectX::SimpleMath::Color Color;

#include <DirectXColors.h>
namespace ColorPresets = DirectX::Colors;

#include <variant>
typedef Vector<std::variant<bool, int, char, float, String, Vector2, Vector3, Vector4, Matrix>> VariantVector;
class Entity;
using Variant = std::variant<bool, int, char, float, String, FilePath, Vector2, Vector3, Vector4, Matrix, VariantVector, Ref<Entity>, Vector<String>>;
#define Extract(TypeName, variant) std::get<TypeName>((variant))

#include <functional>
template <class T>
using Function = std::function<T>;

#include "JSON/json.hpp"
namespace JSON = nlohmann;

#include "imgui.h"
#include "sol/sol.hpp"

// target Windows 7 or later
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

#ifndef WINDOWS_NO_DUMP
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#endif

#define NOMINMAX

#define STRICT

#include <Windows.h>
