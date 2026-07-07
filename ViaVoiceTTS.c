#ifndef VVTTS_EXPORTS
#define VVTTS_EXPORTS
#endif
#include "ViaVoiceTTS.h"
#include "eci_loader.h"
#include "wav_writer.h"
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static ECIFuncTable g_eci;                
static ECIHand      g_hEngine = NULL;     
static int g_bInit = 0;
static int          g_currentVoice = 0;   
static char         g_lastError[512];     
static CRITICAL_SECTION g_cs;             
#define AUDIO_CHUNK_SIZE     4096         
#define AUDIO_MAX_COLLECT    (16*1024*1024) 
typedef struct {
    short  *pData;             
    int     dataSize;          
    int     dataCapacity;      
    int     sampleRate;        
} AudioCollector;
static AudioCollector g_audioCollector;
static short g_eciOutBuf[AUDIO_CHUNK_SIZE]; 
extern void VVTTS_InitPortableRegistry(const char* baseDir);
extern void VVTTS_MountReg(void);
extern void VVTTS_UnmountReg(void);
extern void VVTTS_FreePortableRegistry(void);
static void SetError(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _vsnprintf(g_lastError, sizeof(g_lastError) - 1, fmt, args);
    g_lastError[sizeof(g_lastError) - 1] = '\0';
    va_end(args);
}
static void ClearError(void)
{
    g_lastError[0] = '\0';
}
static int ExtractDatPackage(const char* datPath, const char* targetDir) {
    FILE* fp = fopen(datPath, "rb");
    if (!fp) return -1; 
    CreateDirectoryA(targetDir, NULL);
    while (1) {
        unsigned int nameLen = 0;
        if (fread(&nameLen, 1, 4, fp) != 4) break; 
        if (nameLen == 0) break; 
        char name[256] = {0};
        fread(name, 1, nameLen, fp);
        unsigned int size = 0;
        fread(&size, 1, 4, fp);
        char outPath[MAX_PATH];
        snprintf(outPath, MAX_PATH, "%s\\%s", targetDir, name);
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        if (GetFileAttributesExA(outPath, GetFileExInfoStandard, &fileInfo)) {
            if (fileInfo.nFileSizeLow == size) {
                fseek(fp, size, SEEK_CUR); 
                continue;
            }
        }
        FILE* fout = fopen(outPath, "wb");
        if (fout) {
            char* buf = (char*)malloc(size);
            if (buf) {
                fread(buf, 1, size, fp);
                fwrite(buf, 1, size, fout);
                free(buf);
            }
            fclose(fout);
        } else {
            fseek(fp, size, SEEK_CUR); 
        }
    }
    fclose(fp);
    return 0;
}
static void AudioCollector_Init(AudioCollector *ac, int sampleRate)
{
    ac->pData = NULL;
    ac->dataSize = 0;
    ac->dataCapacity = 0;
    ac->sampleRate = sampleRate;
}
static int AudioCollector_Append(AudioCollector *ac, const short *samples, int numSamples)
{
    int bytesNeeded = numSamples * sizeof(short);
    int newSize = ac->dataSize + bytesNeeded;
    if (newSize > AUDIO_MAX_COLLECT) return -1;
    if (newSize > ac->dataCapacity) {
        int newCap = ac->dataCapacity == 0 ? 65536 : ac->dataCapacity * 2;
        short *pNew;
        while (newCap < newSize) newCap *= 2;
        pNew = (short *)realloc(ac->pData, newCap);
        if (!pNew) return -1;
        ac->pData = pNew;
        ac->dataCapacity = newCap;
    }
    memcpy((char *)ac->pData + ac->dataSize, samples, bytesNeeded);
    ac->dataSize += bytesNeeded;
    return 0;
}
static void AudioCollector_Free(AudioCollector *ac)
{
    if (ac->pData) {
        free(ac->pData);
        ac->pData = NULL;
    }
    ac->dataSize = 0;
    ac->dataCapacity = 0;
}
static enum ECICallbackReturn ECIFNDECLARE BufferCallback(
    ECIHand hEngine, enum ECIMessage Msg, long lParam, void *pData)
{
    AudioCollector *ac = (AudioCollector *)pData;
    if (Msg == eciWaveformBuffer) {
        if (lParam > 0 && ac) {
            AudioCollector_Append(ac, g_eciOutBuf, (int)lParam);
        }
    }
    return eciDataProcessed;
}
static int GetSampleRate(void)
{
    int sr = 11025; 
    if (g_eci.pEciGetParam && g_hEngine) {
        int eciSR = g_eci.pEciGetParam(g_hEngine, eciSampleRate);
        switch (eciSR) {
            case 0: sr = 8000;  break;
            case 1: sr = 11025; break;
            case 2: sr = 22050; break;
            default: sr = 11025; break;
        }
    }
    return sr;
}

static void DumpConfigToIni(const char *callerName)
{
    if (!g_bInit) return;
    char iniPath[MAX_PATH];
    char exePath[MAX_PATH];
    HMODULE hSelf = NULL;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)DumpConfigToIni, &hSelf);
    if (hSelf) {
        GetModuleFileNameA(hSelf, exePath, MAX_PATH);
        char *lastSlash = strrchr(exePath, '\\');
        if (!lastSlash) lastSlash = strrchr(exePath, '/');
        if (lastSlash) *(lastSlash) = '\0';
    } else {
        GetCurrentDirectoryA(MAX_PATH, exePath);
    }
    snprintf(iniPath, MAX_PATH, "%s\\vvtts_config.ini", exePath);

    char val[32];
    int rate = VVTTS_GetRate();
    if (rate != -1) { snprintf(val, sizeof(val), "%d", rate); WritePrivateProfileStringA("VoiceConfig", "Rate", val, iniPath); }
    
    int pitch = VVTTS_GetPitch();
    if (pitch != -1) { snprintf(val, sizeof(val), "%d", pitch); WritePrivateProfileStringA("VoiceConfig", "Pitch", val, iniPath); }
    
    int volume = VVTTS_GetVolume();
    if (volume != -1) { snprintf(val, sizeof(val), "%d", volume); WritePrivateProfileStringA("VoiceConfig", "Volume", val, iniPath); }
    
    int headSize = VVTTS_GetHeadSize();
    if (headSize != -1) { snprintf(val, sizeof(val), "%d", headSize); WritePrivateProfileStringA("VoiceConfig", "HeadSize", val, iniPath); }
    
    int roughness = VVTTS_GetRoughness();
    if (roughness != -1) { snprintf(val, sizeof(val), "%d", roughness); WritePrivateProfileStringA("VoiceConfig", "Roughness", val, iniPath); }
    
    int breathiness = VVTTS_GetBreathiness();
    if (breathiness != -1) { snprintf(val, sizeof(val), "%d", breathiness); WritePrivateProfileStringA("VoiceConfig", "Breathiness", val, iniPath); }

    if (callerName) {
        WritePrivateProfileStringA("VoiceConfig", "LastCalledInterface", callerName, iniPath);
    }
}


VVTTS_API int __stdcall VVTTS_Init(const char *dllDir)
{
    return VVTTS_InitEx(dllDir, 0);
}
VVTTS_API int __stdcall VVTTS_InitEx(const char *dllDir, int langId)
{
    int rc;
    char absDllDir[MAX_PATH];
    char tempRuntimeDir[MAX_PATH];
    char datFilePath[MAX_PATH];
    HMODULE hSelf = NULL;
    char *lastSlash;
    EnterCriticalSection(&g_cs);
    if (g_bInit) {
        SetError("Already initialized");
        LeaveCriticalSection(&g_cs);
        return VVTTS_ERR_ALREADY_INIT;
    }
    ClearError();
    GetTempPathA(MAX_PATH, tempRuntimeDir);
    strcat(tempRuntimeDir, "ViaVoice_Runtime");
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)VVTTS_InitEx, &hSelf);
    if (hSelf) {
        GetModuleFileNameA(hSelf, absDllDir, MAX_PATH);
        lastSlash = strrchr(absDllDir, '\\');
        if (!lastSlash) lastSlash = strrchr(absDllDir, '/');
        if (lastSlash) *(lastSlash) = '\0';
    } else {
        GetCurrentDirectoryA(MAX_PATH, absDllDir);
    }
    snprintf(datFilePath, MAX_PATH, "%s\\VVTTS_Data.dat", absDllDir);
    if (ExtractDatPackage(datFilePath, tempRuntimeDir) != 0) {
        SetError("找不到数据包: %s", datFilePath);
        LeaveCriticalSection(&g_cs);
        return VVTTS_ERR_DLL_NOT_FOUND;
    }
    VVTTS_InitPortableRegistry(tempRuntimeDir);
    VVTTS_MountReg();
    rc = ECILoader_Load(tempRuntimeDir, &g_eci);
    if (rc != 0) {
        VVTTS_UnmountReg();
        if (rc == -2) {
            SetError("IBMECI.dll not found in temp directory: %s", tempRuntimeDir);
            LeaveCriticalSection(&g_cs);
            return VVTTS_ERR_DLL_NOT_FOUND;
        } else {
            SetError("Required ECI functions missing in IBMECI.dll (rc=%d)", rc);
            LeaveCriticalSection(&g_cs);
            return VVTTS_ERR_FUNC_MISSING;
        }
    }
    if (langId != 0 && g_eci.pEciNewEx) {
        g_hEngine = g_eci.pEciNewEx((enum ECILanguageDialect)langId);
    } else if (g_eci.pEciNew) {
        g_hEngine = g_eci.pEciNew();
    }
    VVTTS_UnmountReg();
    if (g_hEngine == NULL || g_hEngine == (ECIHand)(intptr_t)NULL_ECI_HAND) {
        SetError("Failed to create ECI engine instance");
        ECILoader_Unload(&g_eci);
        LeaveCriticalSection(&g_cs);
        return VVTTS_ERR_ENGINE_FAIL;
    }
    if (g_eci.pEciCopyVoice) {
        g_eci.pEciCopyVoice(g_hEngine, 0, 8);
    }
    g_bInit = 1;
    DumpConfigToIni("VVTTS_InitEx");
    LeaveCriticalSection(&g_cs);
    return VVTTS_OK;
}
VVTTS_API void __stdcall VVTTS_Uninit(void)
{
    if (!g_bInit) return;
    EnterCriticalSection(&g_cs);
    if (g_eci.pEciStop && g_hEngine)
        g_eci.pEciStop(g_hEngine);
    if (g_eci.pEciDelete && g_hEngine)
        g_eci.pEciDelete(g_hEngine);
    g_hEngine = NULL;
    AudioCollector_Free(&g_audioCollector);
    ECILoader_Unload(&g_eci);
    VVTTS_UnmountReg();
    VVTTS_FreePortableRegistry();
    g_bInit = 0;
    LeaveCriticalSection(&g_cs);
}
VVTTS_API int __stdcall VVTTS_IsInit(void)
{
    return g_bInit;
}
VVTTS_API int __stdcall VVTTS_Speak(const char *text)
{
    int rc = VVTTS_OK;
    if (!g_bInit) { SetError("Not initialized"); return VVTTS_ERR_NOT_INIT; }
    DumpConfigToIni("VVTTS_Speak");
    if (!text || !text[0]) { SetError("Empty text"); return VVTTS_ERR_INVALID_PARAM; }
    EnterCriticalSection(&g_cs);
    ClearError();
    if (g_eci.pEciSetOutputDevice)
        g_eci.pEciSetOutputDevice(g_hEngine, 0);
    if (!g_eci.pEciAddText(g_hEngine, (ECIInputText)text)) {
        SetError("eciAddText failed");
        rc = VVTTS_ERR_SYNTH_FAIL;
        goto cleanup;
    }
    if (!g_eci.pEciSynthesize(g_hEngine)) {
        SetError("eciSynthesize failed");
        rc = VVTTS_ERR_SYNTH_FAIL;
        goto cleanup;
    }
    if (g_eci.pEciSynchronize)
        g_eci.pEciSynchronize(g_hEngine);
cleanup:
    LeaveCriticalSection(&g_cs);
    return rc;
}
VVTTS_API int __stdcall VVTTS_SpeakAsync(const char *text)
{
    int rc = VVTTS_OK;
    if (!g_bInit) { SetError("Not initialized"); return VVTTS_ERR_NOT_INIT; }
    DumpConfigToIni("VVTTS_SpeakAsync");
    if (!text || !text[0]) { SetError("Empty text"); return VVTTS_ERR_INVALID_PARAM; }
    EnterCriticalSection(&g_cs);
    ClearError();
    if (g_eci.pEciSetOutputDevice)
        g_eci.pEciSetOutputDevice(g_hEngine, 0);
    if (!g_eci.pEciAddText(g_hEngine, (ECIInputText)text)) {
        SetError("eciAddText failed");
        rc = VVTTS_ERR_SYNTH_FAIL;
        goto cleanup;
    }
    if (!g_eci.pEciSynthesize(g_hEngine)) {
        SetError("eciSynthesize failed");
        rc = VVTTS_ERR_SYNTH_FAIL;
        goto cleanup;
    }
cleanup:
    LeaveCriticalSection(&g_cs);
    return rc;
}
VVTTS_API int __stdcall VVTTS_Stop(void)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciStop)
        g_eci.pEciStop(g_hEngine);
    if (g_eci.pEciClearInput)
        g_eci.pEciClearInput(g_hEngine);
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_Pause(int bPause)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciPause)
        g_eci.pEciPause(g_hEngine, bPause ? ECITrue : ECIFalse);
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_IsSpeaking(void)
{
    if (!g_bInit) return 0;
    if (g_eci.pEciSpeaking)
        return g_eci.pEciSpeaking(g_hEngine) ? 1 : 0;
    return 0;
}
VVTTS_API int __stdcall VVTTS_WaitUntilDone(void)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciSynchronize)
        g_eci.pEciSynchronize(g_hEngine);
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_SetRate(int rate)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciSetVoiceParam)
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciSpeed, rate);
    DumpConfigToIni("VVTTS_SetRate");
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_GetRate(void)
{
    if (!g_bInit) return -1;
    if (g_eci.pEciGetVoiceParam)
        return g_eci.pEciGetVoiceParam(g_hEngine, 0, eciSpeed);
    return -1;
}
VVTTS_API int __stdcall VVTTS_SetPitch(int pitch)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciSetVoiceParam)
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciPitchBaseline, pitch);
    DumpConfigToIni("VVTTS_SetPitch");
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_GetPitch(void)
{
    if (!g_bInit) return -1;
    if (g_eci.pEciGetVoiceParam)
        return g_eci.pEciGetVoiceParam(g_hEngine, 0, eciPitchBaseline);
    return -1;
}
VVTTS_API int __stdcall VVTTS_SetVolume(int volume)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciSetVoiceParam)
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciVolume, volume);
    DumpConfigToIni("VVTTS_SetVolume");
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_GetVolume(void)
{
    if (!g_bInit) return -1;
    if (g_eci.pEciGetVoiceParam)
        return g_eci.pEciGetVoiceParam(g_hEngine, 0, eciVolume);
    return -1;
}
VVTTS_API int __stdcall VVTTS_SetHeadSize(int size)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciSetVoiceParam)
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciHeadSize, size);
    DumpConfigToIni("VVTTS_SetHeadSize");
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_GetHeadSize(void)
{
    if (!g_bInit) return -1;
    if (g_eci.pEciGetVoiceParam)
        return g_eci.pEciGetVoiceParam(g_hEngine, 0, eciHeadSize);
    return -1;
}
VVTTS_API int __stdcall VVTTS_SetRoughness(int roughness)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciSetVoiceParam)
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciRoughness, roughness);
    DumpConfigToIni("VVTTS_SetRoughness");
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_GetRoughness(void)
{
    if (!g_bInit) return -1;
    if (g_eci.pEciGetVoiceParam)
        return g_eci.pEciGetVoiceParam(g_hEngine, 0, eciRoughness);
    return -1;
}
VVTTS_API int __stdcall VVTTS_SetBreathiness(int breathiness)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciSetVoiceParam)
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciBreathiness, breathiness);
    DumpConfigToIni("VVTTS_SetBreathiness");
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_GetBreathiness(void)
{
    if (!g_bInit) return -1;
    if (g_eci.pEciGetVoiceParam)
        return g_eci.pEciGetVoiceParam(g_hEngine, 0, eciBreathiness);
    return -1;
}
VVTTS_API int __stdcall VVTTS_SetVoice(int voiceId)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (voiceId < 0 || voiceId >= 8) {
        SetError("Voice ID must be 0-7");
        return VVTTS_ERR_INVALID_PARAM;
    }
    if (g_eci.pEciSetVoiceParam) {
        static const int presetGender[8] = {0, 0, 1, 1, 0, 0, 1, 1};
        static const int presetHead[8]   = {30, 50, 50, 22, 86, 50, 56, 45};
        static const int presetPitch[8]  = {61, 69, 81, 93, 56, 69, 89, 68};
        static const int presetFluc[8]   = {44, 30, 30, 35, 47, 34, 35, 30};
        static const int presetRough[8]  = {18, 0, 0, 0, 0, 0, 0, 3};
        static const int presetBreath[8] = {20, 0, 50, 0, 0, 0, 40, 40};
        static const int presetSpeed[8]  = {50, 50, 50, 50, 50, 70, 70, 50};
        static const int presetVol[8]    = {90, 92, 100, 90, 93, 92, 95, 90};
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciGender, presetGender[voiceId]);
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciHeadSize, presetHead[voiceId]);
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciPitchBaseline, presetPitch[voiceId]);
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciPitchFluctuation, presetFluc[voiceId]);
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciRoughness, presetRough[voiceId]);
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciBreathiness, presetBreath[voiceId]);
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciSpeed, presetSpeed[voiceId]);
        g_eci.pEciSetVoiceParam(g_hEngine, 0, eciVolume, presetVol[voiceId]);
    }
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_SetLanguage(int langId)
{
    if (!g_bInit) return VVTTS_ERR_NOT_INIT;
    if (g_eci.pEciSetParam)
        g_eci.pEciSetParam(g_hEngine, eciLanguageDialect, langId);
    DumpConfigToIni("VVTTS_SetLanguage");
    return VVTTS_OK;
}
VVTTS_API int __stdcall VVTTS_SpeakToFile(const char *text, const char *wavPath)
{
    int sampleRate;
    int rc = VVTTS_OK;
    if (!g_bInit) { SetError("Not initialized"); return VVTTS_ERR_NOT_INIT; }
    DumpConfigToIni("VVTTS_SpeakToFile");
    if (!text || !text[0]) { SetError("Empty text"); return VVTTS_ERR_INVALID_PARAM; }
    if (!wavPath || !wavPath[0]) { SetError("Empty file path"); return VVTTS_ERR_INVALID_PARAM; }
    EnterCriticalSection(&g_cs);
    ClearError();
    sampleRate = GetSampleRate();
    AudioCollector_Init(&g_audioCollector, sampleRate);
    if (g_eci.pEciSetOutputBuffer)
        g_eci.pEciSetOutputBuffer(g_hEngine, AUDIO_CHUNK_SIZE, g_eciOutBuf);
    if (g_eci.pEciRegisterCallback)
        g_eci.pEciRegisterCallback(g_hEngine, (ECICallback)BufferCallback, &g_audioCollector);
    if (!g_eci.pEciAddText(g_hEngine, (ECIInputText)text)) {
        SetError("eciAddText failed");
        rc = VVTTS_ERR_SYNTH_FAIL;
        goto cleanup;
    }
    if (!g_eci.pEciSynthesize(g_hEngine)) {
        SetError("eciSynthesize failed");
        rc = VVTTS_ERR_SYNTH_FAIL;
        goto cleanup;
    }
    if (g_eci.pEciSynchronize)
        g_eci.pEciSynchronize(g_hEngine);
    if (g_audioCollector.dataSize > 0) {
        if (WAV_WriteFile(wavPath,
                          g_audioCollector.pData,
                          g_audioCollector.dataSize,
                          sampleRate,
                          1,     
                          16     
                         ) != 0)
        {
            SetError("Failed to write WAV file: %s", wavPath);
            rc = VVTTS_ERR_FILE_FAIL;
            goto cleanup;
        }
    } else {
        SetError("No audio data generated");
        rc = VVTTS_ERR_SYNTH_FAIL;
    }
cleanup:
    if (g_eci.pEciRegisterCallback)
        g_eci.pEciRegisterCallback(g_hEngine, NULL, NULL);
    if (g_eci.pEciSetOutputDevice)
        g_eci.pEciSetOutputDevice(g_hEngine, 0);
    AudioCollector_Free(&g_audioCollector);
    LeaveCriticalSection(&g_cs);
    return rc;
}
VVTTS_API int __stdcall VVTTS_SpeakToBuffer(const char *text, void *pBuffer,
                                             int bufSize, int *pWritten)
{
    int sampleRate;
    int rc = VVTTS_OK;
    if (pWritten) *pWritten = 0;
    if (!g_bInit) { SetError("Not initialized"); return VVTTS_ERR_NOT_INIT; }
    DumpConfigToIni("VVTTS_SpeakToBuffer");
    if (!text || !text[0]) { SetError("Empty text"); return VVTTS_ERR_INVALID_PARAM; }
    if (!pBuffer || bufSize <= 0) { SetError("Invalid buffer"); return VVTTS_ERR_INVALID_PARAM; }
    EnterCriticalSection(&g_cs);
    ClearError();
    sampleRate = GetSampleRate();
    AudioCollector_Init(&g_audioCollector, sampleRate);
    if (g_eci.pEciSetOutputBuffer)
        g_eci.pEciSetOutputBuffer(g_hEngine, AUDIO_CHUNK_SIZE, g_eciOutBuf);
    if (g_eci.pEciRegisterCallback)
        g_eci.pEciRegisterCallback(g_hEngine, (ECICallback)BufferCallback, &g_audioCollector);
    if (!g_eci.pEciAddText(g_hEngine, (ECIInputText)text)) {
        SetError("eciAddText failed");
        rc = VVTTS_ERR_SYNTH_FAIL;
        goto cleanup;
    }
    if (!g_eci.pEciSynthesize(g_hEngine)) {
        SetError("eciSynthesize failed");
        rc = VVTTS_ERR_SYNTH_FAIL;
        goto cleanup;
    }
    if (g_eci.pEciSynchronize)
        g_eci.pEciSynchronize(g_hEngine);
    if (g_audioCollector.dataSize > 0) {
        int copySize = g_audioCollector.dataSize;
        if (copySize > bufSize) {
            copySize = bufSize;
            rc = VVTTS_ERR_BUFFER_SMALL;
            SetError("Buffer too small: need %d bytes, have %d",
                     g_audioCollector.dataSize, bufSize);
        }
        memcpy(pBuffer, g_audioCollector.pData, copySize);
        if (pWritten) *pWritten = copySize;
    }
cleanup:
    if (g_eci.pEciRegisterCallback)
        g_eci.pEciRegisterCallback(g_hEngine, NULL, NULL);
    if (g_eci.pEciSetOutputDevice)
        g_eci.pEciSetOutputDevice(g_hEngine, 0);
    AudioCollector_Free(&g_audioCollector);
    LeaveCriticalSection(&g_cs);
    return rc;
}
VVTTS_API int __stdcall VVTTS_GetVersion(char *pBuffer, int bufSize)
{
    char verBuf[256];
    if (!pBuffer || bufSize <= 0) return VVTTS_ERR_INVALID_PARAM;
    pBuffer[0] = '\0';
    if (!g_bInit) {
        _snprintf(pBuffer, bufSize, "ViaVoiceTTS Wrapper v1.0 (not initialized)");
        pBuffer[bufSize - 1] = '\0';
        return VVTTS_OK;
    }
    if (g_eci.pEciVersion) {
        memset(verBuf, 0, sizeof(verBuf));
        g_eci.pEciVersion(verBuf);
        _snprintf(pBuffer, bufSize, "ViaVoiceTTS Wrapper v1.0 | ECI: %s", verBuf);
    } else {
        _snprintf(pBuffer, bufSize, "ViaVoiceTTS Wrapper v1.0 | ECI: (version unavailable)");
    }
    pBuffer[bufSize - 1] = '\0';
    return VVTTS_OK;
}
VVTTS_API const char* __stdcall VVTTS_GetLastError(void)
{
    return g_lastError;
}
VVTTS_API int __stdcall VVTTS_GetStatus(void)
{
    if (!g_bInit) return -1;
    if (g_eci.pEciProgStatus)
        return g_eci.pEciProgStatus(g_hEngine);
    return -1;
}
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hInstDLL;
    (void)lpvReserved;
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        InitializeCriticalSection(&g_cs);
        g_bInit = 0;
        g_hEngine = NULL;
        g_lastError[0] = '\0';
        memset(&g_eci, 0, sizeof(g_eci));
        memset(&g_audioCollector, 0, sizeof(g_audioCollector));
        break;
    case DLL_PROCESS_DETACH:
        if (g_bInit) {
            VVTTS_Uninit();
        }
        DeleteCriticalSection(&g_cs);
        break;
    }
    return TRUE;
}
