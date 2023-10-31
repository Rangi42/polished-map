#include <cctype>
#include <iomanip>

#pragma warning(push, 0)
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "themes.h"
#include "event.h"
#include "main-window.h"

static uchar texture_red16_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x00, 0x00, 0x00, 0x25, 0x3d, 0x6d,
	0x22, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0xff, 0x00, 0x00, 0xcd, 0x00, 0x00, 0xd8,
	0xd4, 0x17, 0x10, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0xf8, 0xff, 0x9f, 0xa1,
	0x81, 0x91, 0x58, 0xf4, 0xff, 0x3f, 0x00, 0x0c, 0xa8, 0x0b, 0x0b, 0xc4, 0x92, 0xc3, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_green16_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x00, 0x00, 0x00, 0x25, 0x3d, 0x6d,
	0x22, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x00, 0xa0, 0x00, 0x00, 0x6e, 0x00, 0x2e,
	0x3e, 0x60, 0x70, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0xf8, 0xff, 0x9f, 0xa1,
	0x81, 0x91, 0x58, 0xf4, 0xff, 0x3f, 0x00, 0x0c, 0xa8, 0x0b, 0x0b, 0xc4, 0x92, 0xc3, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_blue16_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x00, 0x00, 0x00, 0x25, 0x3d, 0x6d,
	0x22, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x30, 0x30, 0xff, 0x00, 0x00, 0xcd, 0x3b,
	0xec, 0xbe, 0x18, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0xf8, 0xff, 0x9f, 0xa1,
	0x81, 0x91, 0x58, 0xf4, 0xff, 0x3f, 0x00, 0x0c, 0xa8, 0x0b, 0x0b, 0xc4, 0x92, 0xc3, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_purple16_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x00, 0x00, 0x00, 0x25, 0x3d, 0x6d,
	0x22, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0xa0, 0x00, 0xa0, 0x6e, 0x00, 0x6e, 0x19,
	0x53, 0x1b, 0x51, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0xf8, 0xff, 0x9f, 0xa1,
	0x81, 0x91, 0x58, 0xf4, 0xff, 0x3f, 0x00, 0x0c, 0xa8, 0x0b, 0x0b, 0xc4, 0x92, 0xc3, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_orange16_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x00, 0x00, 0x00, 0x25, 0x3d, 0x6d,
	0x22, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0xff, 0x80, 0x00, 0xcd, 0x4e, 0x00, 0x07,
	0xc8, 0xc5, 0x09, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0xf8, 0xff, 0x9f, 0xa1,
	0x81, 0x91, 0x58, 0xf4, 0xff, 0x3f, 0x00, 0x0c, 0xa8, 0x0b, 0x0b, 0xc4, 0x92, 0xc3, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_azure16_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x00, 0x00, 0x00, 0x25, 0x3d, 0x6d,
	0x22, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x00, 0x80, 0xff, 0x00, 0x4e, 0xcd, 0x41,
	0xab, 0xd3, 0x2a, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0xf8, 0xff, 0x9f, 0xa1,
	0x81, 0x91, 0x58, 0xf4, 0xff, 0x3f, 0x00, 0x0c, 0xa8, 0x0b, 0x0b, 0xc4, 0x92, 0xc3, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_red32_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x49, 0xb4, 0xe8,
	0xb7, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0xcd, 0x00, 0x00, 0xff, 0x00, 0x00, 0xb6,
	0x89, 0x80, 0x48, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0x00, 0x83, 0xfa, 0xff,
	0xff, 0xff, 0x0d, 0x3e, 0x02, 0x04, 0x00, 0x8d, 0x4d, 0x68, 0x6b, 0x67, 0xaf, 0xc3, 0x69, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_green32_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x49, 0xb4, 0xe8,
	0xb7, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x00, 0x6e, 0x00, 0x00, 0xa0, 0x00, 0x2d,
	0x62, 0xcc, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0x00, 0x83, 0xfa, 0xff,
	0xff, 0xff, 0x0d, 0x3e, 0x02, 0x04, 0x00, 0x8d, 0x4d, 0x68, 0x6b, 0x67, 0xaf, 0xc3, 0x69, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_blue32_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x49, 0xb4, 0xe8,
	0xb7, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x00, 0x00, 0xcd, 0x30, 0x30, 0xff, 0xf7,
	0x6f, 0xb3, 0xfa, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0x00, 0x83, 0xfa, 0xff,
	0xff, 0xff, 0x0d, 0x3e, 0x02, 0x04, 0x00, 0x8d, 0x4d, 0x68, 0x6b, 0x67, 0xaf, 0xc3, 0x69, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_purple32_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x49, 0xb4, 0xe8,
	0xb7, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x6e, 0x00, 0x6e, 0xa0, 0x00, 0xa0, 0x43,
	0xce, 0xa9, 0x5d, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0x00, 0x83, 0xfa, 0xff,
	0xff, 0xff, 0x0d, 0x3e, 0x02, 0x04, 0x00, 0x8d, 0x4d, 0x68, 0x6b, 0x67, 0xaf, 0xc3, 0x69, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_orange32_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x49, 0xb4, 0xe8,
	0xb7, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0xcd, 0x4e, 0x00, 0xff, 0x80, 0x00, 0x6a,
	0xc9, 0xfe, 0x2b, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0x00, 0x83, 0xfa, 0xff,
	0xff, 0xff, 0x0d, 0x3e, 0x02, 0x04, 0x00, 0x8d, 0x4d, 0x68, 0x6b, 0x67, 0xaf, 0xc3, 0x69, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static uchar texture_azure32_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x49, 0xb4, 0xe8,
	0xb7, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x00, 0x4e, 0xcd, 0x00, 0x80, 0xff, 0xd0,
	0x02, 0x1e, 0xfa, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x80, 0x80, 0xa0, 0xa8, 0xd6,
	0x53, 0x00, 0x00, 0x00, 0x12, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0x00, 0x83, 0xfa, 0xff,
	0xff, 0xff, 0x0d, 0x3e, 0x02, 0x04, 0x00, 0x8d, 0x4d, 0x68, 0x6b, 0x67, 0xaf, 0xc3, 0x69, 0x00,
	0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static Fl_PNG_Image texture_images[NUM_EVENT_TEXTURES] = {
	{NULL, texture_red16_png_buffer,    sizeof(texture_red16_png_buffer)},    // TX_RED
	{NULL, texture_green16_png_buffer,  sizeof(texture_green16_png_buffer)},  // TX_GREEN
	{NULL, texture_blue16_png_buffer,   sizeof(texture_blue16_png_buffer)},   // TX_BLUE
	{NULL, texture_purple16_png_buffer, sizeof(texture_purple16_png_buffer)}, // TX_PURPLE
	{NULL, texture_orange16_png_buffer, sizeof(texture_orange16_png_buffer)}, // TX_ORANGE
	{NULL, texture_azure16_png_buffer,  sizeof(texture_azure16_png_buffer)}   // TX_AZURE
};

static Fl_PNG_Image zoomed_texture_images[NUM_EVENT_TEXTURES] = {
	{NULL, texture_red32_png_buffer,    sizeof(texture_red32_png_buffer)},    // TX_RED
	{NULL, texture_green32_png_buffer,  sizeof(texture_green32_png_buffer)},  // TX_GREEN
	{NULL, texture_blue32_png_buffer,   sizeof(texture_blue32_png_buffer)},   // TX_BLUE
	{NULL, texture_purple32_png_buffer, sizeof(texture_purple32_png_buffer)}, // TX_PURPLE
	{NULL, texture_orange32_png_buffer, sizeof(texture_orange32_png_buffer)}, // TX_ORANGE
	{NULL, texture_azure32_png_buffer,  sizeof(texture_azure32_png_buffer)}   // TX_AZURE
};

static const uchar warp_digits_png_buffer[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x00, 0x00, 0x00, 0xa5, 0x54, 0x58,
	0xa1, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x55,
	0xc2, 0xd3, 0x7e, 0x00, 0x00, 0x00, 0x41, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0x63, 0x40, 0x02,
	0xc5, 0x32, 0x4f, 0x2d, 0xcf, 0x15, 0x77, 0x1c, 0x36, 0x3c, 0xc7, 0x10, 0xc8, 0xa2, 0xaa, 0xc8,
	0x14, 0x34, 0x65, 0x89, 0x26, 0x07, 0x43, 0xa0, 0x4c, 0xba, 0xe5, 0xa1, 0xe2, 0x39, 0x47, 0x34,
	0x7b, 0x18, 0x02, 0x05, 0x14, 0x39, 0x5d, 0x02, 0xc0, 0xa2, 0xc5, 0x73, 0x1e, 0x5a, 0x1e, 0x29,
	0x98, 0x72, 0xd8, 0xf0, 0x04, 0x92, 0x01, 0x00, 0xf3, 0x8c, 0x14, 0x1d, 0x65, 0x4c, 0x16, 0x38,
	0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static Fl_PNG_Image warp_digits_image(NULL, warp_digits_png_buffer, sizeof(warp_digits_png_buffer));

Event::Event(size_t line, const std::string &prelude, const std::string &macro, Event_Meta meta, const std::string &tip_, int warp_id) :
	Fl_Box(0, 0, 0, 0), _line(line), _meta(meta), _event_x(0), _event_y(0), _prelude(prelude), _macro(macro), _prefix(),
	_suffix(), _tip(tip_), _warp_id(warp_id), _warp_to(NULL), _prefixed(false), _suffixed(false), _hex_coords(false) {
	user_data(NULL);
	trim(_tip);
	tip(_tip);
}

static int parse_value(std::string &s, bool *hex = NULL) {
	int n = 0;
	trim(s);
	if (!s.empty()) {
		if (s[0] == '$') {
			s.erase(0, 1);
			n = strtol(s.c_str(), NULL, 16);
			if (hex) { *hex = true; }
		}
		else if (s[0] == '%') {
			s.erase(0, 1);
			n = strtol(s.c_str(), NULL, 2);
		}
		else {
			n = strtol(s.c_str(), NULL, 10);
		}
	}
	return n;
}

std::pair<std::string, int> Event::warp_destination() const {
	std::string name;
	int id = 0;
	if (!_meta.is_warp()) { return std::make_pair(name, id); }

	std::istringstream ss(_suffix);
	std::string map_token, id_token;

	// Skip leading comma
	if (!ss.good()) { return std::make_pair(name, id); }
	std::getline(ss, map_token, ',');

	// Get destination map and ID
	if (!ss.good()) { return std::make_pair(name, id); }
	std::getline(ss, map_token, ',');
	if (!ss.good()) { return std::make_pair(name, id); }
	std::getline(ss, id_token, ',');
	if (_meta.id_map) { std::swap(map_token, id_token); }

	remove_comment(map_token);
	trim(map_token);
	bool letter = false, digit = false, digit_underscore = false;
	for (char c : map_token) {
		if (c != '_') {
			if (digit_underscore && isdigit(c)) {
				name += '_'; // ...1__2... -> ...1_2...
			}
			if (letter) {
				c = (char)tolower(c); // ...ABC_DEF_B1F... -> ...AbcDefB1F...
			}
			name += c;
			digit_underscore = false;
		}
		else if (digit) {
			digit_underscore = true;
		}
		letter = isalpha(c);
		digit = isdigit(c);
	}

	id = parse_value(id_token);

	return std::make_pair(name, id);
}

static int16_t parse_coord(std::string s, bool *hex) {
	int n = parse_value(s, hex);
	n = (n + EVENT_MARGIN) % 0x100 - EVENT_MARGIN;
	return (int16_t)n;
}

void Event::parse(std::istringstream &lss) {
	std::string token;
	_prefixed = _meta.skip > 0;
	for (uint8_t i = 0; i < _meta.skip; i++) {
		std::getline(lss, token, ',');
		_prefix += token + ',';
	}
	std::getline(lss, token, ',');
	_event_x = parse_coord(token, &_hex_coords);
	std::getline(lss, token);
	size_t sep = std::min(token.find(','), token.find(';'));
	_suffixed = sep != std::string::npos;
	if (_suffixed) {
		_suffix = token.substr(sep);
		token.erase(sep);
	}
	_event_y = parse_coord(token, &_hex_coords);
	if (_meta.yx) {
		std::swap(_event_x, _event_y);
	}
}

void Event::update_tooltip() {
	std::ostringstream ss;
	ss << _macro << ' ';
	if (_prefixed) { ss << _prefix << ' '; }
	ss << (_hex_coords ? std::hex : std::dec);
	if (_hex_coords) { ss << '$'; }
	else { ss << std::right << std::setw(2); }
	ss << (int)(_meta.yx ? _event_y : _event_x);
	ss << ", ";
	if (_hex_coords) { ss << '$'; }
	else { ss << std::right << std::setw(2); }
	ss << (int)(_meta.yx ? _event_x : _event_y);
	ss << std::setw(1);
	if (_suffixed) { ss << _suffix; }
	tip(ss.str());
}

void Event::draw_warp_id(int x, int y) const {
	if (_hex_coords) {
		warp_digits_image.draw(x+10, y+8, 5, 7, 5 * (_warp_id % 0x10), 0);
		if (_warp_id >= 0x10) {
			warp_digits_image.draw(x+6, y+8, 5, 7, 5 * ((_warp_id / 0x10) % 0x10), 0);
		}
	}
	else {
		warp_digits_image.draw(x+10, y+8, 5, 7, 5 * (_warp_id % 10), 0);
		if (_warp_id >= 10) {
			warp_digits_image.draw(x+6, y+8, 5, 7, 5 * ((_warp_id / 10) % 10), 0);
			if (_warp_id >= 100) {
				warp_digits_image.draw(x+2, y+8, 5, 7, 5 * ((_warp_id / 100) % 10), 0);
			}
		}
	}
}

void Event::draw_warp_id_zoomed(int x, int y) const {
	fl_font(FL_COURIER, OS::is_consolas() ? 12 : 11);
	char warp_id[4] = {};
#if defined(_MSC_VER) && _MSC_VER < 1900
	_snprintf_s(warp_id, sizeof(warp_id), _hex_coords ? "%X" : "%d", _warp_id);
#else
	snprintf(warp_id, sizeof(warp_id), _hex_coords ? "%X" : "%d", _warp_id);
#endif
	draw_outlined_text(warp_id, x-4, y-2, w(), h(), FL_ALIGN_BOTTOM_RIGHT | FL_ALIGN_INSIDE, FL_BLACK, FL_WHITE);
}

void Event::draw() {
	Main_Window *mw = (Main_Window *)user_data();
	if (mw->mode() != Mode::EVENTS && !mw->show_events()) { return; }
	int X = x(), Y = y(), W = w(), H = h();
	const Event *source = dynamic_cast<Event *>(Fl::belowmouse());
	bool warped_from = source && source->warp_to() == this;
	bool zoom = mw->zoom();
	(zoom ? zoomed_texture_images : texture_images)[(int)_meta.texture].draw(X, Y, W, H);
	fl_font(FL_HELVETICA_BOLD, zoom ? 24 : 14);
	fl_color(FL_WHITE);
	char buffer[] = {_meta.symbol, '\0'};
	fl_draw(buffer, X, Y, W, H, FL_ALIGN_CENTER);
	if (_warp_id > 0 && mw->show_warp_ids()) {
		if (zoom) {
			draw_warp_id_zoomed(X, Y);
		}
		else {
			draw_warp_id(X, Y);
		}
	}
	fl_font(OS_FONT, OS_FONT_SIZE);
	if (Fl::belowmouse() != this && !warped_from) { return; }
	Fl_Color c = warped_from ? FL_YELLOW : FL_WHITE;
	fl_rect(X, Y, W, H, FL_BLACK);
	fl_rect(X+1, Y+1, W-2, H-2, c);
	if (zoom) {
		fl_rect(X+2, Y+2, W-4, H-4, c);
		fl_rect(X+3, Y+3, W-6, H-6, FL_BLACK);
	}
	else {
		fl_rect(X+2, Y+2, W-4, H-4, FL_BLACK);
	}
}

void Event::print(bool print_warp_ids) const {
	int X = (int)_event_x * EVENT_PX_SIZE, Y = (int)_event_y * EVENT_PX_SIZE;
	texture_images[(int)_meta.texture].draw(X, Y, EVENT_PX_SIZE, EVENT_PX_SIZE);
	fl_font(FL_HELVETICA_BOLD, 14);
	fl_color(FL_WHITE);
	char buffer[] = {_meta.symbol, '\0'};
	fl_draw(buffer, X, Y, EVENT_PX_SIZE, EVENT_PX_SIZE, FL_ALIGN_CENTER);
	if (_warp_id > 0 && print_warp_ids) {
		draw_warp_id(X, Y);
	}
	fl_font(OS_FONT, OS_FONT_SIZE);
}

int Event::handle(int event) {
	Main_Window *mw = (Main_Window *)user_data();
	if (mw->mode() != Mode::EVENTS) { return 0; }
	switch (event) {
	case FL_ENTER:
		if (_warp_to) {
			_warp_to->redraw();
		}
		mw->update_status(this);
		mw->update_gameboy_screen(this);
		redraw();
		return 1;
	case FL_LEAVE:
		if (_warp_to) {
			_warp_to->redraw();
		}
		mw->update_status((Block *)NULL);
		mw->update_gameboy_screen((Block *)NULL);
		redraw();
		return 1;
	case FL_MOVE:
		mw->redraw_map();
		return 1;
	case FL_PUSH:
		return 1;
	case FL_RELEASE:
		if (Fl::event_shift()) {
			do_callback();
		}
		else {
			do_callback();
			update_tooltip();
			mw->update_event_cursor(this);
			mw->update_gameboy_screen(this);
		}
		return 1;
	case FL_DRAG:
		if (Fl::event_button() == FL_LEFT_MOUSE && !Fl::event_shift()) {
			do_callback();
		}
		return 1;
	}
	return 0;
}
