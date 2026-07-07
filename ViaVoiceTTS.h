#ifndef __VIAVOICETTS_H
#define __VIAVOICETTS_H
#ifdef __cplusplus
extern "C" {
#endif
#ifdef VVTTS_EXPORTS
    #define VVTTS_API __declspec(dllexport)
#else
    #define VVTTS_API __declspec(dllimport)
#endif
#define VVTTS_OK                 0   
#define VVTTS_ERR_NOT_INIT      -1   
#define VVTTS_ERR_ALREADY_INIT  -2   
#define VVTTS_ERR_DLL_NOT_FOUND -3   
#define VVTTS_ERR_FUNC_MISSING  -4   
#define VVTTS_ERR_ENGINE_FAIL   -5   
#define VVTTS_ERR_SYNTH_FAIL    -6   
#define VVTTS_ERR_INVALID_PARAM -7   
#define VVTTS_ERR_FILE_FAIL     -8   
#define VVTTS_ERR_BUFFER_SMALL  -9   
#define VVTTS_ERR_BUSY          -10  
#define VVTTS_LANG_EN_US   0x00010000   
#define VVTTS_LANG_EN_GB   0x00010001   
#define VVTTS_LANG_ES_ES   0x00020000   
#define VVTTS_LANG_ES_MX   0x00020001   
#define VVTTS_LANG_FR_FR   0x00030000   
#define VVTTS_LANG_FR_CA   0x00030001   
#define VVTTS_LANG_DE_DE   0x00040000   
#define VVTTS_LANG_IT_IT   0x00050000   
#define VVTTS_LANG_ZH_CN   0x00060000   
#define VVTTS_LANG_ZH_TW   0x00060001   
#define VVTTS_LANG_PT_BR   0x00070000   
#define VVTTS_LANG_JA_JP   0x00080000   
#define VVTTS_LANG_FI_FI   0x00090000   
#define VVTTS_LANG_KO_KR   0x000A0000   
#define VVTTS_VOICE_DEFAULT  0
#define VVTTS_VOICE_1        1
#define VVTTS_VOICE_2        2
#define VVTTS_VOICE_3        3
#define VVTTS_VOICE_4        4
#define VVTTS_VOICE_5        5
#define VVTTS_VOICE_6        6
#define VVTTS_VOICE_7        7
VVTTS_API int __stdcall VVTTS_Init(const char *dllDir);
VVTTS_API int __stdcall VVTTS_InitEx(const char *dllDir, int langId);
VVTTS_API void __stdcall VVTTS_Uninit(void);
VVTTS_API int __stdcall VVTTS_IsInit(void);
VVTTS_API int __stdcall VVTTS_Speak(const char *text);
VVTTS_API int __stdcall VVTTS_SpeakAsync(const char *text);
VVTTS_API int __stdcall VVTTS_Stop(void);
VVTTS_API int __stdcall VVTTS_Pause(int bPause);
VVTTS_API int __stdcall VVTTS_IsSpeaking(void);
VVTTS_API int __stdcall VVTTS_WaitUntilDone(void);
VVTTS_API int __stdcall VVTTS_SetRate(int rate);
VVTTS_API int __stdcall VVTTS_GetRate(void);
VVTTS_API int __stdcall VVTTS_SetPitch(int pitch);
VVTTS_API int __stdcall VVTTS_GetPitch(void);
VVTTS_API int __stdcall VVTTS_SetVolume(int volume);
VVTTS_API int __stdcall VVTTS_GetVolume(void);
VVTTS_API int __stdcall VVTTS_SetHeadSize(int size);
VVTTS_API int __stdcall VVTTS_GetHeadSize(void);
VVTTS_API int __stdcall VVTTS_SetRoughness(int roughness);
VVTTS_API int __stdcall VVTTS_GetRoughness(void);
VVTTS_API int __stdcall VVTTS_SetBreathiness(int breathiness);
VVTTS_API int __stdcall VVTTS_GetBreathiness(void);
VVTTS_API int __stdcall VVTTS_SetVoice(int voiceId);
VVTTS_API int __stdcall VVTTS_SetLanguage(int langId);
VVTTS_API int __stdcall VVTTS_SpeakToFile(const char *text, const char *wavPath);
VVTTS_API int __stdcall VVTTS_SpeakToBuffer(const char *text, void *pBuffer,
                                             int bufSize, int *pWritten);
VVTTS_API int __stdcall VVTTS_GetVersion(char *pBuffer, int bufSize);
VVTTS_API const char* __stdcall VVTTS_GetLastError(void);
VVTTS_API int __stdcall VVTTS_GetStatus(void);
#ifdef __cplusplus
}
#endif
#endif 
