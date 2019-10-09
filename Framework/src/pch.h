#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cinttypes>
#include <vector>
#include <array>
#include <unordered_map>
#include <fstream>

// GLM
#pragma warning (push)
#pragma warning (disable: 4201) // nonstandard extension used: nameless struct/union
#define GLM_FORCE_INLINE
#define GLM_FORCE_NO_CTOR_INIT
#define GLM_FORCE_EXPLICIT_CTOR
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/common.hpp>
#pragma warning (pop)

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

// Macros
#if _WIN32
#define XBREAK() __debugbreak()
#else
#define XBREAK() __builtin_trap()
#endif

#define __STRINGIZE__1(x) #x
#define __STRINGIZE__2(x) __STRINGIZE__1(x)
#define __LOCATION__ __FILE__ " : " __STRINGIZE__2(__LINE__)
#define XASSERT(what) if(!(what)){ std::cerr << "Assert failed: "#what; XBREAK(); throw std::runtime_error("Assert failed at " __LOCATION__);}
#define XERROR(what) {std::cerr << "Error: " << what; XBREAK(); throw std::runtime_error("Error at " __LOCATION__); }

