#include "eci_loader.h"
#include <stdio.h>
#include <string.h>
#define LOAD_ECI_FUNC(table, hMod, name, type)                            \
    do {                                                                   \
        (table)->p##name = (type)GetProcAddress((hMod), #name);            \
    } while(0)
#define LOAD_ECI_FUNC_REQ(table, hMod, name, type)                        \
    do {                                                                   \
        (table)->p##name = (type)GetProcAddress((hMod), #name);            \
        if (!(table)->p##name) {                                           \
                           \
            (table)->p##name = (type)GetProcAddress((hMod), "_" #name);    \
        }                                                                  \
        if (!(table)->p##name) { missingCount++; }                         \
    } while(0)
static void GetSelfDir(char *buf, int bufSize)
{
    HMODULE hSelf = NULL;
    char *lastSlash;
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)GetSelfDir,
        &hSelf
    );
    if (hSelf) {
        GetModuleFileNameA(hSelf, buf, bufSize);
    } else {
        GetModuleFileNameA(NULL, buf, bufSize);
    }
    lastSlash = strrchr(buf, '\\');
    if (!lastSlash) lastSlash = strrchr(buf, '/');
    if (lastSlash) *(lastSlash + 1) = '\0';
    else buf[0] = '\0';
}
int ECILoader_Load(const char *dllDir, ECIFuncTable *pTable)
{
    char dllPath[MAX_PATH];
    int missingCount = 0;
    char savedDir[MAX_PATH];
    if (!pTable) return -1;
    memset(pTable, 0, sizeof(ECIFuncTable));
    if (dllDir && dllDir[0]) {
        _snprintf(dllPath, MAX_PATH, "%s\\IBMECI.dll", dllDir);
    } else {
        GetSelfDir(dllPath, MAX_PATH);
        strncat(dllPath, "IBMECI.dll", MAX_PATH - strlen(dllPath) - 1);
    }
    GetCurrentDirectoryA(MAX_PATH, savedDir);
    {
        char eciDir[MAX_PATH];
        char *slash;
        strncpy(eciDir, dllPath, MAX_PATH);
        eciDir[MAX_PATH - 1] = '\0';
        slash = strrchr(eciDir, '\\');
        if (!slash) slash = strrchr(eciDir, '/');
        if (slash) *slash = '\0';
        SetCurrentDirectoryA(eciDir);
        SetDllDirectoryA(eciDir);
    }
    pTable->hModule = LoadLibraryA(dllPath);
    if (!pTable->hModule) {
        SetCurrentDirectoryA(savedDir);
        SetDllDirectoryA(NULL);
        return -2; 
    }
    LOAD_ECI_FUNC_REQ(pTable, pTable->hModule, EciNew,                  pfn_eciNew);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciNewEx,                pfn_eciNewEx);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciGetAvailableLanguages,pfn_eciGetAvailableLanguages);
    LOAD_ECI_FUNC_REQ(pTable, pTable->hModule, EciDelete,               pfn_eciDelete);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciReset,                pfn_eciReset);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciIsBeingReentered,     pfn_eciIsBeingReentered);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciVersion,              pfn_eciVersion);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciProgStatus,           pfn_eciProgStatus);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciErrorMessage,         pfn_eciErrorMessage);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciClearErrors,          pfn_eciClearErrors);
    LOAD_ECI_FUNC_REQ(pTable, pTable->hModule, EciAddText,              pfn_eciAddText);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciInsertIndex,          pfn_eciInsertIndex);
    LOAD_ECI_FUNC_REQ(pTable, pTable->hModule, EciSynthesize,           pfn_eciSynthesize);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSynthesizeFile,       pfn_eciSynthesizeFile);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciClearInput,           pfn_eciClearInput);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciGeneratePhonemes,     pfn_eciGeneratePhonemes);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciGetIndex,             pfn_eciGetIndex);
    LOAD_ECI_FUNC_REQ(pTable, pTable->hModule, EciStop,                 pfn_eciStop);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSpeaking,             pfn_eciSpeaking);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSynchronize,          pfn_eciSynchronize);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciPause,                pfn_eciPause);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSetOutputBuffer,      pfn_eciSetOutputBuffer);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSetOutputFilename,    pfn_eciSetOutputFilename);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSetOutputDevice,      pfn_eciSetOutputDevice);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciGetParam,             pfn_eciGetParam);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSetParam,             pfn_eciSetParam);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciGetDefaultParam,      pfn_eciGetDefaultParam);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSetDefaultParam,      pfn_eciSetDefaultParam);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciCopyVoice,            pfn_eciCopyVoice);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciGetVoiceName,         pfn_eciGetVoiceName);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSetVoiceName,         pfn_eciSetVoiceName);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciGetVoiceParam,        pfn_eciGetVoiceParam);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSetVoiceParam,        pfn_eciSetVoiceParam);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciTestPhrase,           pfn_eciTestPhrase);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSpeakText,            pfn_eciSpeakText);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSpeakTextEx,          pfn_eciSpeakTextEx);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciRegisterCallback,     pfn_eciRegisterCallback);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciNewDict,              pfn_eciNewDict);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciGetDict,              pfn_eciGetDict);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSetDict,              pfn_eciSetDict);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciDeleteDict,           pfn_eciDeleteDict);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciLoadDict,             pfn_eciLoadDict);
    LOAD_ECI_FUNC    (pTable, pTable->hModule, EciSaveDict,             pfn_eciSaveDict);
    SetCurrentDirectoryA(savedDir);
    if (missingCount > 0) {
        missingCount = 0;
        if (!pTable->pEciNew) {
            pTable->pEciNew = (pfn_eciNew)GetProcAddress(pTable->hModule, "eciNew");
            if (!pTable->pEciNew) missingCount++;
        }
        if (!pTable->pEciDelete) {
            pTable->pEciDelete = (pfn_eciDelete)GetProcAddress(pTable->hModule, "eciDelete");
            if (!pTable->pEciDelete) missingCount++;
        }
        if (!pTable->pEciAddText) {
            pTable->pEciAddText = (pfn_eciAddText)GetProcAddress(pTable->hModule, "eciAddText");
            if (!pTable->pEciAddText) missingCount++;
        }
        if (!pTable->pEciSynthesize) {
            pTable->pEciSynthesize = (pfn_eciSynthesize)GetProcAddress(pTable->hModule, "eciSynthesize");
            if (!pTable->pEciSynthesize) missingCount++;
        }
        if (!pTable->pEciStop) {
            pTable->pEciStop = (pfn_eciStop)GetProcAddress(pTable->hModule, "eciStop");
            if (!pTable->pEciStop) missingCount++;
        }
        if (!pTable->pEciNewEx)
            pTable->pEciNewEx = (pfn_eciNewEx)GetProcAddress(pTable->hModule, "eciNewEx");
        if (!pTable->pEciReset)
            pTable->pEciReset = (pfn_eciReset)GetProcAddress(pTable->hModule, "eciReset");
        if (!pTable->pEciVersion)
            pTable->pEciVersion = (pfn_eciVersion)GetProcAddress(pTable->hModule, "eciVersion");
        if (!pTable->pEciProgStatus)
            pTable->pEciProgStatus = (pfn_eciProgStatus)GetProcAddress(pTable->hModule, "eciProgStatus");
        if (!pTable->pEciErrorMessage)
            pTable->pEciErrorMessage = (pfn_eciErrorMessage)GetProcAddress(pTable->hModule, "eciErrorMessage");
        if (!pTable->pEciClearErrors)
            pTable->pEciClearErrors = (pfn_eciClearErrors)GetProcAddress(pTable->hModule, "eciClearErrors");
        if (!pTable->pEciSpeaking)
            pTable->pEciSpeaking = (pfn_eciSpeaking)GetProcAddress(pTable->hModule, "eciSpeaking");
        if (!pTable->pEciSynchronize)
            pTable->pEciSynchronize = (pfn_eciSynchronize)GetProcAddress(pTable->hModule, "eciSynchronize");
        if (!pTable->pEciPause)
            pTable->pEciPause = (pfn_eciPause)GetProcAddress(pTable->hModule, "eciPause");
        if (!pTable->pEciSetOutputBuffer)
            pTable->pEciSetOutputBuffer = (pfn_eciSetOutputBuffer)GetProcAddress(pTable->hModule, "eciSetOutputBuffer");
        if (!pTable->pEciSetOutputFilename)
            pTable->pEciSetOutputFilename = (pfn_eciSetOutputFilename)GetProcAddress(pTable->hModule, "eciSetOutputFilename");
        if (!pTable->pEciSetOutputDevice)
            pTable->pEciSetOutputDevice = (pfn_eciSetOutputDevice)GetProcAddress(pTable->hModule, "eciSetOutputDevice");
        if (!pTable->pEciGetParam)
            pTable->pEciGetParam = (pfn_eciGetParam)GetProcAddress(pTable->hModule, "eciGetParam");
        if (!pTable->pEciSetParam)
            pTable->pEciSetParam = (pfn_eciSetParam)GetProcAddress(pTable->hModule, "eciSetParam");
        if (!pTable->pEciGetVoiceParam)
            pTable->pEciGetVoiceParam = (pfn_eciGetVoiceParam)GetProcAddress(pTable->hModule, "eciGetVoiceParam");
        if (!pTable->pEciSetVoiceParam)
            pTable->pEciSetVoiceParam = (pfn_eciSetVoiceParam)GetProcAddress(pTable->hModule, "eciSetVoiceParam");
        if (!pTable->pEciRegisterCallback)
            pTable->pEciRegisterCallback = (pfn_eciRegisterCallback)GetProcAddress(pTable->hModule, "eciRegisterCallback");
        if (!pTable->pEciClearInput)
            pTable->pEciClearInput = (pfn_eciClearInput)GetProcAddress(pTable->hModule, "eciClearInput");
        if (!pTable->pEciInsertIndex)
            pTable->pEciInsertIndex = (pfn_eciInsertIndex)GetProcAddress(pTable->hModule, "eciInsertIndex");
        if (!pTable->pEciSynthesizeFile)
            pTable->pEciSynthesizeFile = (pfn_eciSynthesizeFile)GetProcAddress(pTable->hModule, "eciSynthesizeFile");
    }
    if (missingCount > 0) {
        FreeLibrary(pTable->hModule);
        memset(pTable, 0, sizeof(ECIFuncTable));
        return -3;
    }
    return 0; 
}
void ECILoader_Unload(ECIFuncTable *pTable)
{
    if (!pTable) return;
    if (pTable->hModule) {
        FreeLibrary(pTable->hModule);
    }
    memset(pTable, 0, sizeof(ECIFuncTable));
    SetDllDirectoryA(NULL);
}
