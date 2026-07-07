#include <windows.h>
#include <stdio.h>
#include <string.h>
static HKEY g_hVirtualKey = NULL;
static int g_regMounted = 0;
static void SetRegStr(HKEY hKey, const char* subKey, const char* valName, const char* value) {
    HKEY hSub;
    if (RegCreateKeyExA(hKey, subKey, 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hSub, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hSub, valName, 0, REG_SZ, (const BYTE*)value, strlen(value) + 1);
        RegCloseKey(hSub);
    }
}
static void SetRegDword(HKEY hKey, const char* subKey, const char* valName, DWORD value) {
    HKEY hSub;
    if (RegCreateKeyExA(hKey, subKey, 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hSub, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hSub, valName, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));
        RegCloseKey(hSub);
    }
}
static void SetRegBin(HKEY hKey, const char* subKey, const char* valName, const BYTE* value, DWORD size) {
    HKEY hSub;
    if (RegCreateKeyExA(hKey, subKey, 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hSub, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hSub, valName, 0, REG_BINARY, value, size);
        RegCloseKey(hSub);
    }
}
static void WriteIBMBranch(HKEY hVirtual, const char* prefix, const char* baseDir) {
    char keyBuf[512];
    char pathBuf[MAX_PATH];
    #define MK_KEY(sub) (snprintf(keyBuf, sizeof(keyBuf), "%s\\%s", prefix, sub), keyBuf)
    #define MK_PATH(file) (snprintf(pathBuf, sizeof(pathBuf), "%s\\%s", baseDir, file), pathBuf)
    SetRegDword(hVirtual, MK_KEY("IBM ViaVoice Outloud Runtime\\Install\\Counters\\Zh_CN"), "ProductCtr", 1);
    SetRegStr(hVirtual, MK_KEY("IBM ViaVoice Outloud Runtime\\Install\\Directories\\Zh_CN"), "Bin", baseDir);
    SetRegStr(hVirtual, MK_KEY("IBM ViaVoice Outloud Runtime\\Install\\Version\\Zh_CN"), "Release", "6.0.0.0");
    SetRegDword(hVirtual, MK_KEY("ViaVoice"), "TTSInstallRC", 0);
    SetRegStr(hVirtual, MK_KEY("ViaVoice"), "TTSInfo", "Installation completed successfully.");
    SetRegStr(hVirtual, MK_KEY("ViaVoice\\Outloud Runtime\\Zh_CN"), "Version", "6.0.0.0");
    SetRegStr(hVirtual, MK_KEY("ViaVoice\\Outloud Runtime\\Zh_CN"), "Path", baseDir);
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\ECIINI\\6.0"), "Path_Rom", MK_PATH("chsrom.dll"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\ECIINI\\6.0"), "Path", MK_PATH("chs50.syn"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\ECIINI\\6.0"), "Version", "5.0");
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\ECIINI\\6.0"), "Concatenative", "1.0");
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\ECIINI\\6.0"), "CallbackFlag", "0x33f");
    SetRegStr(hVirtual, keyBuf, "Voice1", "0 50 69 30 0 0 50 92");
    SetRegStr(hVirtual, keyBuf, "Voice2", "1 50 81 30 0 50 50 100");
    SetRegStr(hVirtual, keyBuf, "Voice3", "1 22 93 35 0 0 50 90");
    SetRegStr(hVirtual, keyBuf, "Voice4", "0 86 56 47 0 0 50 93");
    SetRegStr(hVirtual, keyBuf, "Voice5", "0 50 69 34 0 0 70 92");
    SetRegStr(hVirtual, keyBuf, "Voice6", "1 56 89 35 0 40 70 95");
    SetRegStr(hVirtual, keyBuf, "Voice7", "1 45 68 30 3 40 50 90");
    SetRegStr(hVirtual, keyBuf, "Voice8", "0 30 61 44 18 20 50 90");
    {
        static const struct { int id; const char* val; } s_phonemes[] = {
            {0, "65 0 0 0 0 0 0 75 150 128 100 130 175 200 125"}, {1, "66 0 0 0 0 0 0 100 125 128 200 150 130 150 150"},
            {2, "67 0 0 0 0 0 0 75 125 128 100 130 130 0 200"},  {3, "68 0 0 0 0 0 0 50 100 128 25 130 130 180 125"},
            {4, "69 0 0 0 0 0 0 50 100 128 75 130 175 0 125"},   {5, "70 0 0 0 0 0 0 75 150 128 50 130 130 255 0"},
            {6, "71 0 0 0 0 0 0 50 100 128 75 130 130 0 0"},     {7, "72 0 0 0 0 0 0 50 75 128 100 150 130 0 100"},
            {8, "73 0 0 0 0 0 0 50 100 128 50 130 175 0 125"},   {9, "74 0 0 0 0 0 0 75 125 128 100 130 130 0 200"},
            {10, "79 0 0 0 0 0 0 25 0 130 150 60 60 0 255"},     {11, "82 0 0 0 0 0 0 75 100 128 75 130 0 0 150"},
            {12, "83 0 0 0 0 0 0 75 125 128 100 130 130 0 200"}, {13, "84 0 0 0 0 0 0 50 100 128 25 130 130 180 125"},
            {14, "85 0 0 0 0 0 0 25 0 130 90 60 60 0 255"},      {15, "89 0 0 0 0 0 0 100 125 128 200 150 130 150 150"},
            {16, "90 0 0 0 0 0 0 75 125 128 100 130 130 0 200"}, {17, "97 0 0 0 0 0 0 100 125 128 200 150 130 150 150"},
            {18, "98 0 0 0 0 0 0 0 100 128 0 0 0 0 200"},        {19, "99 0 0 0 0 0 0 50 160 0 50 128 128 160 0"},
            {20, "100 0 0 0 0 0 0 75 150 128 50 130 130 255 0"}, {21, "101 0 0 0 0 0 0 50 100 128 75 130 175 0 255"},
            {22, "102 0 0 0 0 0 0 10 100 128 25 130 0 0 200"},   {23, "103 0 0 0 0 0 0 50 100 128 75 130 130 0 0"},
            {24, "104 0 0 0 0 0 0 50 100 128 75 130 130 0 0"},   {25, "105 0 0 0 0 0 0 25 150 128 50 130 130 0 255"},
            {26, "107 0 0 0 0 0 0 50 100 128 75 130 130 0 0"},   {27, "108 0 0 0 0 0 0 75 150 128 50 130 130 255 0"},
            {28, "109 0 0 0 0 0 0 0 100 128 0 0 0 0 200"},       {29, "110 0 0 0 0 0 0 75 150 128 50 130 130 255 0"},
            {30, "111 0 0 0 0 0 0 25 0 130 130 60 60 0 255"},    {31, "112 0 0 0 0 0 0 0 100 128 0 0 0 0 200"},
            {32, "114 0 0 0 0 0 0 75 50 128 75 130 0 0 175"},    {33, "115 0 0 0 0 0 0 50 100 128 75 130 130 150 0"},
            {34, "116 0 0 0 0 0 0 75 150 128 50 130 130 255 0"}, {35, "117 0 0 0 0 0 0 25 0 130 90 60 60 0 255"},
            {36, "118 0 0 0 0 0 0 10 100 128 25 130 0 0 200"},   {37, "119 0 0 0 0 0 0 25 0 130 90 60 60 0 255"},
            {38, "120 0 0 0 0 0 0 10 50 128 75 130 0 0 0"},      {39, "121 0 0 0 0 0 0 25 150 128 50 130 130 0 255"},
            {40, "122 0 0 0 0 0 0 50 100 128 75 130 130 150 0"}, {83, "64 0 0 0 0 0 0 75 50 128 75 130 0 0 175"},
            {84, "164 0 0 0 0 0 0 0 170 128 0 0 0 0 0"}
        };
        char pname[32], pval[128];
        size_t count = sizeof(s_phonemes) / sizeof(s_phonemes[0]);
        for (size_t i = 0; i < count; ++i) {
            snprintf(pname, sizeof(pname), "Phoneme%d", s_phonemes[i].id);
            SetRegStr(hVirtual, keyBuf, pname, s_phonemes[i].val);
            if (s_phonemes[i].id <= 40) {
                snprintf(pname, sizeof(pname), "Phoneme%d", s_phonemes[i].id + 41);
                snprintf(pval, sizeof(pval), "95 %s", s_phonemes[i].val);
                SetRegStr(hVirtual, keyBuf, pname, pval);
            }
        }
    }
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\ECIINI\\LanguageIndependent"), "Path_Cmm", MK_PATH("ibmcmm.dll"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\ECIINI\\LanguageIndependent"), "Path_Cat", MK_PATH("torrent.dll"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\Romanizers\\6\\0"), "Path_Rom", MK_PATH("chsrom.dll"));
    {
        static const BYTE v1[] = {0x01,0,0,0,0x01,0,0,0,0x1e,0,0,0,0x56,0x6f,0x69,0x63,0x65,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0,0,0,0,0x32,0,0,0,0x41,0,0,0,0x1e,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x5c,0,0,0};
        static const BYTE v2[] = {0x01,0,0,0,0x02,0,0,0,0x1e,0,0,0,0x56,0x6f,0x69,0x63,0x65,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0,0,0,0,0x32,0,0,0,0x45,0,0,0,0x1e,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x5c,0,0,0};
        static const BYTE v3[] = {0x01,0,0,0,0x03,0,0,0,0x1e,0,0,0,0x56,0x6f,0x69,0x63,0x65,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0,0,0,0,0x32,0,0,0,0x4a,0,0,0,0x23,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x5c,0,0,0};
        static const BYTE v4[] = {0x01,0,0,0,0x01,0,0,0,0x1e,0,0,0,0x56,0x6f,0x69,0x63,0x65,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,1,0,0,0,0x32,0,0,0,0x51,0,0,0,0x1e,0,0,0,0,0,0,0,0x32,0,0,0,0x32,0,0,0,0x64,0,0,0};
        static const BYTE v5[] = {0x01,0,0,0,0x01,0,0,0,0x06,0,0,0,0x56,0x6f,0x69,0x63,0x65,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,1,0,0,0,0x16,0,0,0,0x5d,0,0,0,0x23,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x5a,0,0,0};
        static const BYTE v6[] = {0x01,0,0,0,0x01,0,0,0,0x46,0,0,0,0x56,0x6f,0x69,0x63,0x65,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0,0,0,0,0x1e,0,0,0,0x3d,0,0,0,0x2c,0,0,0,0x12,0,0,0,0x14,0,0,0,0x32,0,0,0,0x5a,0,0,0};
        static const BYTE v7[] = {0x01,0,0,0,0x01,0,0,0,0x46,0,0,0,0x56,0x6f,0x69,0x63,0x65,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,1,0,0,0,0x2d,0,0,0,0x44,0,0,0,0x1e,0,0,0,0x03,0,0,0,0x28,0,0,0,0x32,0,0,0,0x5a,0,0,0};
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SAPIstandardVoices"), "0300000001", v1, sizeof(v1));
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SAPIstandardVoices"), "0300000002", v2, sizeof(v2));
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SAPIstandardVoices"), "0300000003", v3, sizeof(v3));
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SAPIstandardVoices"), "0300100001", v4, sizeof(v4));
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SAPIstandardVoices"), "0060100001", v5, sizeof(v5));
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SAPIstandardVoices"), "0700000001", v6, sizeof(v6));
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SAPIstandardVoices"), "0700100001", v7, sizeof(v7));
    }
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SynEngines\\6\\0"), "Version Number", "5.0");
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SynEngines\\6\\0"), "Concatenative", "1.0");
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SynEngines\\6\\0"), "Test Phrase", "This is a test.");
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SynEngines\\6\\0"), "Path", MK_PATH("chs50.syn"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SynEngines\\6\\0"), "CallbackFlag", "0x33f");
    {
        static const BYTE vRefs[] = {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0};
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SynEngines\\6\\0"), "VoiceReferences", vRefs, sizeof(vRefs));
        static const BYTE voices[] = {
            0x4c,0x69,0x33,0x20,0x4a,0x69,0x6e,0x67,0x34,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x45,0,0,0,0x1e,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x5c,0,0,0,
            0x57,0x61,0x6e,0x67,0x32,0x20,0x59,0x61,0x6e,0x34,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0x32,0,0,0,0x51,0,0,0,0x1e,0,0,0,0,0,0,0,0x32,0,0,0,0x32,0,0,0,0x64,0,0,0,
            0x4c,0x69,0x33,0x20,0x44,0x6f,0x6e,0x67,0x31,0x20,0x44,0x6f,0x6e,0x67,0x31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0x16,0,0,0,0x5d,0,0,0,0x23,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x5a,0,0,0,
            0x7a,0x68,0x61,0x6e,0x67,0x31,0x74,0x69,0x65,0x33,0x7a,0x68,0x75,0x34,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x56,0,0,0,0x38,0,0,0,0x2f,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x5d,0,0,0,
            0x77,0x61,0x6e,0x67,0x31,0x68,0x61,0x69,0x33,0x73,0x68,0x65,0x6e,0x67,0x31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x32,0,0,0,0x45,0,0,0,0x22,0,0,0,0,0,0,0,0,0,0,0,0x46,0,0,0,0x5c,0,0,0,
            0x7a,0x68,0x61,0x6f,0x34,0x77,0x65,0x69,0x31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0x38,0,0,0,0x59,0,0,0,0x23,0,0,0,0,0,0,0,0x28,0,0,0,0x46,0,0,0,0x5f,0,0,0,
            0x4e,0x61,0x69,0x33,0x20,0x4e,0x61,0x69,0x30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0x2d,0,0,0,0x44,0,0,0,0x1e,0,0,0,0x03,0,0,0,0x28,0,0,0,0x32,0,0,0,0x5a,0,0,0,
            0x59,0x65,0x32,0x20,0x59,0x65,0x30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x1e,0,0,0,0x3d,0,0,0,0x2c,0,0,0,0x12,0,0,0,0x14,0,0,0,0x32,0,0,0,0x5a,0,0,0
        };
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\SynEngines\\6\\0"), "Voices", voices, sizeof(voices));
    }
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\UserInterfaceLanguages\\6\\0"), "Path", MK_PATH("chs.uil"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\UserInterfaceLanguages\\6\\0"), "SAPI Help", MK_PATH("chssapi.hlp"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\UserInterfaceLanguages\\6\\0"), "Ref Help", MK_PATH("chsref.hlp"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\UserInterfaceLanguages\\6\\0"), "Gen Help", MK_PATH("chsgen.hlp"));
    SetRegStr(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\UserInterfaceLanguages\\6\\0"), "Lex Help", MK_PATH("chslex.hlp"));
    {
        static const BYTE lang[] = {0x04, 0x08};
        SetRegBin(hVirtual, MK_KEY("ViaVoice Outloud 5.0\\UserInterfaceLanguages\\6\\0"), "LANGID", lang, sizeof(lang));
    }
    SetRegDword(hVirtual, MK_KEY("ViaVoice TTS"), "TTSInstallRC", 0);
    SetRegStr(hVirtual, MK_KEY("ViaVoice TTS"), "TTSInfo", "Installation completed successfully.");
    SetRegStr(hVirtual, MK_KEY("ViaVoice TTS"), "Path", baseDir);
    SetRegStr(hVirtual, MK_KEY("ViaVoice TTS\\Zh_CN"), "Version", "6.0.0.0");
    SetRegStr(hVirtual, MK_KEY("ViaVoice TTS\\Zh_CN"), "Path", baseDir);
    SetRegDword(hVirtual, MK_KEY("ViaVoice TTS\\Zh_CN"), "ProductCtr", 1);
}
void VVTTS_InitPortableRegistry(const char* baseDir) {
    if (!g_hVirtualKey) {
        if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\ViaVoiceTTS_Portable", 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &g_hVirtualKey, NULL) == ERROR_SUCCESS) {
            WriteIBMBranch(g_hVirtualKey, "SOFTWARE\\IBM", baseDir);
            WriteIBMBranch(g_hVirtualKey, "SOFTWARE\\WOW6432Node\\IBM", baseDir);
        }
    }
}
void VVTTS_MountReg() {
    if (g_hVirtualKey && !g_regMounted) {
        RegOverridePredefKey(HKEY_LOCAL_MACHINE, g_hVirtualKey);
        g_regMounted = 1;
    }
}
void VVTTS_UnmountReg() {
    if (g_hVirtualKey && g_regMounted) {
        RegOverridePredefKey(HKEY_LOCAL_MACHINE, NULL);
        g_regMounted = 0;
    }
}
void VVTTS_FreePortableRegistry() {
    if (g_hVirtualKey) {
        VVTTS_UnmountReg();
        RegCloseKey(g_hVirtualKey);
        g_hVirtualKey = NULL;
    }
}
