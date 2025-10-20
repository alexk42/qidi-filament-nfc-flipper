#pragma once

typedef struct {
    uint8_t code;
    const char* name;
    uint32_t rgb;
} QidiColor;

static const QidiColor qidi_colors[] = {
    {1, "White", 0xFAFAFA},
    {2, "Black", 0x060606},
    {3, "Light Gray", 0xD9E3ED},
    {4, "Lime Green", 0x5CF30F},
    {5, "Mint Green", 0x63E492},
    {6, "Blue", 0x2850FF},
    {7, "Pink", 0xFE98FE},
    {8, "Yellow", 0xDFD628},
    {9, "Dark Green", 0x228332},
    {10, "Light Blue", 0x99DEFF},
    {11, "Dark Blue", 0x1714B0},
    {12, "Lavender", 0xCEC0FE},
    {13, "Yellow Green", 0xCADE4B},
    {14, "Ocean Blue", 0x1353AB},
    {15, "Sky Blue", 0x5EA9FD},
    {16, "Purple", 0xA878FF},
    {17, "Light Red", 0xFE717A},
    {18, "Red", 0xFF362D},
    {19, "Beige", 0xE2DFCD},
    {20, "Gray", 0x898F9B},
    {21, "Brown", 0x6E3812},
    {22, "Tan", 0xCAC59F},
    {23, "Orange", 0xF28636},
    {24, "Bronze", 0xB87F2B},
};

#define QIDI_COLOR_COUNT (sizeof(qidi_colors) / sizeof(QidiColor))
