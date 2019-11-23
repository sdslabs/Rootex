#pragma once

// Smart pointers
#include <memory>
template <class T>
using Ptr = std::unique_ptr<T>;
template <class T>
using Ref = std::shared_ptr<T>;
template <class T>
using Weak = std::weak_ptr<T>;

// Containers
#include <string>
typedef std::string String;

#include <map>
template <class P, class Q>
using Map = std::map<P, Q>;

#include <unordered_map>
template <class P, class Q>
using HashMap = std::unordered_map<P, Q>;

#include <vector>
template <class T>
using Vector = std::vector<T>;
