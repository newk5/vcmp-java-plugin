#include "common.h"

uint32_t Common::ConvertArgbToRgba(uint32_t argb) {
	return (argb << 8U) | (argb >> 24U);
}

uint32_t Common::ConvertRgbaToArgb(uint32_t rgba) {
	return (rgba >> 8U) | (rgba << 24U);
}
