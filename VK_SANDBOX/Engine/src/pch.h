#pragma once

// C++
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>
#include <optional>
#include <cstring>
#include <vector>
#include <array>
#include <set>
#include <unordered_map>
#include <unordered_set>

// GLFW & VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <chrono>