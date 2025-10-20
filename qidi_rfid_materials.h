#pragma once

typedef enum {
    MaterialPLA = 1,
    MaterialPLA_Matte = 2,
    MaterialPLA_Metal = 3,
    MaterialPLA_Silk = 4,
    MaterialPLA_CF = 5,
    MaterialPLA_Wood = 6,
    MaterialABS = 11,
    MaterialABS_GF = 12,
    MaterialABS_Metal = 13,
    MaterialASA = 18,
    MaterialASA_AERO = 19,
    MaterialPA = 24,
    MaterialPA_CF = 25,
    MaterialPAHT_CF = 30,
    MaterialPAHT_GF = 31,
    MaterialPC_ABS_FR = 34,
    MaterialPET_CF = 37,
    MaterialPET_GF = 38,
    MaterialPETG = 41,
    MaterialPPS_CF = 44,
    MaterialPVA = 47,
    MaterialTPU = 50,
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
