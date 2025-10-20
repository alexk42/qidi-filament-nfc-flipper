#pragma once

typedef enum {
    MaterialPLA = 0x01,        // Wiki: 1
    MaterialPLA_Matte = 0x02,  // Wiki: 2
    MaterialPLA_Metal = 0x03,  // Wiki: 3
    MaterialPLA_Silk = 0x04,   // Wiki: 4
    MaterialPLA_CF = 0x05,     // Wiki: 5
    MaterialPLA_Wood = 0x06,   // Wiki: 6
    MaterialABS = 0x0B,        // Wiki: 11
    MaterialABS_GF = 0x0C,     // Wiki: 12
    MaterialABS_Metal = 0x0D,  // Wiki: 13
    MaterialASA = 0x12,        // Wiki: 18
    MaterialASA_AERO = 0x13,   // Wiki: 19
    MaterialPA = 0x18,         // Wiki: 24
    MaterialPA_CF = 0x19,      // Wiki: 25
    MaterialPAHT_CF = 0x1E,    // Wiki: 30
    MaterialPAHT_GF = 0x1F,    // Wiki: 31
    MaterialPC_ABS_FR = 0x22,  // Wiki: 34
    MaterialPET_CF = 0x25,     // Wiki: 37
    MaterialPET_GF = 0x26,     // Wiki: 38
    MaterialPETG = 0x29,       // Wiki: 41
    MaterialPPS_CF = 0x2C,     // Wiki: 44
    MaterialPVA = 0x2F,        // Wiki: 47
    MaterialTPU = 0x32,        // Wiki: 50
} QidiMaterial;

typedef struct {
    QidiMaterial code;
    const char* name;
} QidiMaterialInfo;

static const QidiMaterialInfo qidi_materials[] = {
    {MaterialPLA, "PLA"},
    {MaterialPLA_Matte, "PLA Matte"},
    {MaterialPLA_Metal, "PLA Metal"},
    {MaterialPLA_Silk, "PLA Silk"},
    {MaterialPLA_CF, "PLA-CF"},
    {MaterialPLA_Wood, "PLA-Wood"},
    {MaterialABS, "ABS"},
    {MaterialABS_GF, "ABS-GF"},
    {MaterialABS_Metal, "ABS-Metal"},
    {MaterialASA, "ASA"},
    {MaterialASA_AERO, "ASA-AERO"},
    {MaterialPA, "PA"},
    {MaterialPA_CF, "PA-CF"},
    {MaterialPAHT_CF, "PAHT-CF"},
    {MaterialPAHT_GF, "PAHT-GF"},
    {MaterialPC_ABS_FR, "PC/ABS-FR"},
    {MaterialPET_CF, "PET-CF"},
    {MaterialPET_GF, "PET-GF"},
    {MaterialPETG, "PETG"},
    {MaterialPPS_CF, "PPS-CF"},
    {MaterialPVA, "PVA"},
    {MaterialTPU, "TPU"},
};

#define QIDI_MATERIAL_COUNT (sizeof(qidi_materials) / sizeof(QidiMaterialInfo))
