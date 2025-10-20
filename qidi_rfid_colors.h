#pragma once

typedef struct {
    uint8_t code;
    const char* name;
    uint32_t rgb;
} QidiColor;

static const QidiColor qidi_colors[] = {
    {0x01, "White", 0xFAFAFA},         // Wiki: 1
    {0x02, "Black", 0x060606},         // Wiki: 2
    {0x03, "Light Gray", 0xD9E3ED},    // Wiki: 3
    {0x04, "Lime Green", 0x5CF30F},    // Wiki: 4
    {0x05, "Mint Green", 0x63E492},    // Wiki: 5
    {0x06, "Blue", 0x2850FF},          // Wiki: 6
    {0x07, "Pink", 0xFE98FE},          // Wiki: 7
    {0x08, "Yellow", 0xDFD628},        // Wiki: 8
    {0x09, "Dark Green", 0x228332},    // Wiki: 9
    {0x0A, "Light Blue", 0x99DEFF},    // Wiki: 10
    {0x0B, "Dark Blue", 0x1714B0},     // Wiki: 11
    {0x0C, "Lavender", 0xCEC0FE},      // Wiki: 12
    {0x0D, "Yellow Green", 0xCADE4B},  // Wiki: 13
    {0x0E, "Ocean Blue", 0x1353AB},    // Wiki: 14
    {0x0F, "Sky Blue", 0x5EA9FD},      // Wiki: 15
    {0x10, "Purple", 0xA878FF},        // Wiki: 16
    {0x11, "Light Red", 0xFE717A},     // Wiki: 17
    {0x12, "Red", 0xFF362D},           // Wiki: 18
    {0x13, "Beige", 0xE2DFCD},         // Wiki: 19
    {0x14, "Gray", 0x898F9B},          // Wiki: 20
    {0x15, "Brown", 0x6E3812},         // Wiki: 21
    {0x16, "Tan", 0xCAC59F},           // Wiki: 22
    {0x17, "Orange", 0xF28636},        // Wiki: 23
    {0x18, "Bronze", 0xB87F2B},        // Wiki: 24
};

#define QIDI_COLOR_COUNT (sizeof(qidi_colors) / sizeof(QidiColor))
