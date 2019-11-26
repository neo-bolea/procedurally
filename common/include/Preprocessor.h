#pragma once

#define REQUIRES(...) typename = typename std::enable_if_t<__VA_ARGS__>