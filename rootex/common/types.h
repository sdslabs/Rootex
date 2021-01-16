#pragma once

// target Windows 7 or later
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif // !_WIN32_WINNT
#include <sdkddkver.h>

#ifndef WINDOWS_NO_DUMP
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
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

/// Convert nanoseconds to milliseconds
#define NS_TO_MS 1e-6f
/// Convert milliseconds to nanoseconds
#define MS_TO_NS 1e+6f
/// Convert milliseconds to seconds
#define MS_TO_S 1e-3f
/// Convert seconds to milliseconds
#define S_TO_MS 1e+3f

/// Future data type for reading future variables
#include <future>
template <class T>
using Future = std::future<T>;

/// Promise data types for sharing futures
#include <future>
template <class T>
using Promise = std::promise<T>;

/// Promise data types for sharing futures
#include <atomic>
template <class T>
using Atomic = std::atomic<T>;

// Smart pointers
#include <memory>
/// std::unique_ptr
template <class T>
using Ptr = std::unique_ptr<T>;
/// std::shared_ptr
template <class T>
using Ref = std::shared_ptr<T>;
/// std::weak_ptr
template <class T>
using Weak = std::weak_ptr<T>;
#include <wrl.h> // For using Microsoft::WRL::ComPtr<T>

// Serialization streams
#include <fstream>
/// std::fstream
typedef std::fstream InputOutputFileStream;
/// std::ofstream
typedef std::ofstream OutputFileStream;
/// std::ifstream
typedef std::ifstream InputFileStream;
#include <sstream>
/// std::stringstream
typedef std::stringstream StringStream;

// Containers
#include <string>
/// std::string
typedef std::string String;

#include <map>
/// std::map
template <class P, class Q>
using Map = std::map<P, Q>;

#include <unordered_map>
/// std::unordered_map
template <class P, class Q>
using HashMap = std::unordered_map<P, Q>;

#include <utility>
/// std::tuple
template <typename...P>
using Tuple = std::tuple<P...>;
/// std::pair
template <class P, class Q>
using Pair = std::pair<P, Q>;

#include <optional>
/// std::optional
template <class T>
using Optional = std::optional<T>;

#include <vector>
/// std::vector
template <class T>
using Vector = std::vector<T>;

#include <stack>
/// std::stack
template <class T>
using Stack = std::stack<T>;

#include <filesystem>
/// std::filesystem::path
using FilePath = std::filesystem::path;

// Math Containers
#include <d3d11.h>
#include "vendor/DirectXTK/Inc/SimpleMath.h"
/// DirectX::SimpleMath::Matrix
typedef DirectX::SimpleMath::Matrix Matrix;
/// DirectX::SimpleMath::Vector2
typedef DirectX::SimpleMath::Vector2 Vector2;
/// DirectX::SimpleMath::Vector3
typedef DirectX::SimpleMath::Vector3 Vector3;
/// DirectX::SimpleMath::Vector4
typedef DirectX::SimpleMath::Vector4 Vector4;
/// DirectX::SimpleMath::Quaternion
typedef DirectX::SimpleMath::Quaternion Quaternion;
/// DirectX::SimpleMath::Ray
typedef DirectX::SimpleMath::Ray Ray;
/// DirectX::SimpleMath::BoundingBox
typedef DirectX::BoundingBox BoundingBox;
/// DirectX::SimpleMath::Color
typedef DirectX::SimpleMath::Color Color;

#include <DirectXColors.h>
/// DirectX::Colors
namespace ColorPresets = DirectX::Colors;

#include <variant>
/// Vector of std::variant of bool, int, char, float, String, Vector2, Vector3, Vector4, Matrix
typedef Vector<std::variant<bool, int, char, float, String, Vector2, Vector3, Vector4, Matrix>> VariantVector;
class Scene;
class Entity;
/// A variant able to hold multiple kinds of data, one at a time.
using Variant = std::variant<bool, int, char, float, String, Vector2, Vector3, Vector4, Matrix, VariantVector, Scene*, Entity*, Vector<String>>;
/// Extract the value of type TypeName from a Variant
template <typename T>
T Extract(const Variant& v)
{
	return std::get<T>(v);
}

#include "JSON/json.hpp"
/// Namespace for the JSON library
namespace JSON = nlohmann;

namespace nlohmann
{
template <>
struct adl_serializer<Vector2>
{
	static void to_json(json& j, const Vector2& v)
	{
		j["x"] = v.x;
		j["y"] = v.y;
	}

	static void from_json(const json& j, Vector2& v)
	{
		v.x = j.at("x");
		v.y = j.at("y");
	}
};
template <>
struct adl_serializer<Vector3>
{
	static void to_json(json& j, const Vector3& v)
	{
		j["x"] = v.x;
		j["y"] = v.y;
		j["z"] = v.z;
	}

	static void from_json(const json& j, Vector3& v)
	{
		v.x = j.at("x");
		v.y = j.at("y");
		v.z = j.at("z");
	}
};
template <>
struct adl_serializer<Vector4>
{
	static void to_json(json& j, const Vector4& v)
	{
		j["x"] = v.x;
		j["y"] = v.y;
		j["z"] = v.z;
		j["w"] = v.w;
	}

	static void from_json(const json& j, Vector4& v)
	{
		v.x = j.at("x");
		v.y = j.at("y");
		v.z = j.at("z");
		v.w = j.at("w");
	}
};
template <>
struct adl_serializer<Color>
{
	static void to_json(json& j, const Color& v)
	{
		j["r"] = v.R();
		j["g"] = v.G();
		j["b"] = v.B();
		j["a"] = v.A();
	}

	static void from_json(const json& j, Color& v)
	{
		v.x = j.at("r");
		v.y = j.at("g");
		v.z = j.at("b");
		v.w = j.at("a");
	}
};
template <>
struct adl_serializer<Quaternion>
{
	static void to_json(json& j, const Quaternion& v)
	{
		j["x"] = v.x;
		j["y"] = v.y;
		j["z"] = v.z;
		j["w"] = v.w;
	}

	static void from_json(const json& j, Quaternion& v)
	{
		v.x = j.at("x");
		v.y = j.at("y");
		v.z = j.at("z");
		v.w = j.at("w");
	}
};
template <>
struct adl_serializer<Matrix>
{
	static void to_json(json& j, const Matrix& v)
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				j.push_back(v.m[x][y]);
			}
		}
	}

	static void from_json(const json& j, Matrix& v)
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				v.m[x][y] = j[x * 4 + y];
			}
		}
	}
};
template <>
struct adl_serializer<BoundingBox>
{
	static void to_json(json& j, const BoundingBox& v)
	{
		j["center"] = (Vector3)v.Center;
		j["extents"] = (Vector3)v.Extents;
	}

	static void from_json(const json& j, BoundingBox& v)
	{
		v.Center = (Vector3)j.value("center", Vector3::Zero);
		v.Extents = (Vector3)j.value("extents", Vector3 { 0.5f, 0.5f, 0.5f });
	}
};
}

#include <functional>
/// std::function
template <class T>
using Function = std::function<T>;

#include "imgui.h"
