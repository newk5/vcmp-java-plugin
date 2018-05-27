#pragma once

#include <stdint.h>

class Common {
public:
	static uint32_t ConvertArgbToRgba(uint32_t argb);
	static uint32_t ConvertRgbaToArgb(uint32_t rgba);
};
