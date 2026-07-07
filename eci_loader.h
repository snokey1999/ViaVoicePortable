#ifndef __ECI_LOADER_H
#define __ECI_LOADER_H
#include <windows.h>
#include "eci.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef ECIHand     (__stdcall *pfn_eciNew)(void);
typedef ECIHand     (__stdcall *pfn_eciNewEx)(enum ECILanguageDialect Value);
typedef int         (__stdcall *pfn_eciGetAvailableLanguages)(enum ECILanguageDialect *aLanguages, int *nLanguages);
typedef ECIHand     (__stdcall *pfn_eciDelete)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciReset)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciIsBeingReentered)(ECIHand hEngine);
typedef void        (__stdcall *pfn_eciVersion)(char *pBuffer);
typedef int         (__stdcall *pfn_eciProgStatus)(ECIHand hEngine);
typedef void        (__stdcall *pfn_eciErrorMessage)(ECIHand hEngine, void *buffer);
typedef void        (__stdcall *pfn_eciClearErrors)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciTestPhrase)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciSpeakText)(ECIInputText pText, Boolean bAnnot);
typedef Boolean     (__stdcall *pfn_eciSpeakTextEx)(ECIInputText pText, Boolean bAnnot, enum ECILanguageDialect Value);
typedef int         (__stdcall *pfn_eciGetParam)(ECIHand hEngine, enum ECIParam Param);
typedef int         (__stdcall *pfn_eciSetParam)(ECIHand hEngine, enum ECIParam Param, int iValue);
typedef int         (__stdcall *pfn_eciGetDefaultParam)(enum ECIParam parameter);
typedef int         (__stdcall *pfn_eciSetDefaultParam)(enum ECIParam parameter, int value);
typedef Boolean     (__stdcall *pfn_eciCopyVoice)(ECIHand hEngine, int iVoiceFrom, int iVoiceTo);
typedef Boolean     (__stdcall *pfn_eciGetVoiceName)(ECIHand hEngine, int iVoice, void *pBuffer);
typedef Boolean     (__stdcall *pfn_eciSetVoiceName)(ECIHand hEngine, int iVoice, const void *pBuffer);
typedef int         (__stdcall *pfn_eciGetVoiceParam)(ECIHand hEngine, int iVoice, enum ECIVoiceParam Param);
typedef int         (__stdcall *pfn_eciSetVoiceParam)(ECIHand hEngine, int iVoice, enum ECIVoiceParam Param, int iValue);
typedef Boolean     (__stdcall *pfn_eciAddText)(ECIHand hEngine, ECIInputText pText);
typedef Boolean     (__stdcall *pfn_eciInsertIndex)(ECIHand hEngine, int iIndex);
typedef Boolean     (__stdcall *pfn_eciSynthesize)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciSynthesizeFile)(ECIHand hEngine, const void *pFilename);
typedef Boolean     (__stdcall *pfn_eciClearInput)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciGeneratePhonemes)(ECIHand hEngine, int iSize, void *pBuffer);
typedef int         (__stdcall *pfn_eciGetIndex)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciStop)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciSpeaking)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciSynchronize)(ECIHand hEngine);
typedef Boolean     (__stdcall *pfn_eciSetOutputBuffer)(ECIHand hEngine, int iSize, short *psBuffer);
typedef Boolean     (__stdcall *pfn_eciSetOutputFilename)(ECIHand hEngine, const void *pFilename);
typedef Boolean     (__stdcall *pfn_eciSetOutputDevice)(ECIHand hEngine, int iDevNum);
typedef Boolean     (__stdcall *pfn_eciPause)(ECIHand hEngine, Boolean On);
typedef void        (__stdcall *pfn_eciRegisterCallback)(ECIHand hEngine, ECICallback Callback, void *pData);
typedef ECIDictHand      (__stdcall *pfn_eciNewDict)(ECIHand hEngine);
typedef ECIDictHand      (__stdcall *pfn_eciGetDict)(ECIHand hEngine);
typedef enum ECIDictError(__stdcall *pfn_eciSetDict)(ECIHand hEngine, ECIDictHand hDict);
typedef ECIDictHand      (__stdcall *pfn_eciDeleteDict)(ECIHand hEngine, ECIDictHand hDict);
typedef enum ECIDictError(__stdcall *pfn_eciLoadDict)(ECIHand hEngine, ECIDictHand hDict, enum ECIDictVolume DictVol, const void *pFilename);
typedef enum ECIDictError(__stdcall *pfn_eciSaveDict)(ECIHand hEngine, ECIDictHand hDict, enum ECIDictVolume DictVol, const void *pFilename);
typedef struct {
    HMODULE                     hModule;            
    pfn_eciNew                  pEciNew;
    pfn_eciNewEx                pEciNewEx;
    pfn_eciGetAvailableLanguages pEciGetAvailableLanguages;
    pfn_eciDelete               pEciDelete;
    pfn_eciReset                pEciReset;
    pfn_eciIsBeingReentered     pEciIsBeingReentered;
    pfn_eciVersion              pEciVersion;
    pfn_eciProgStatus           pEciProgStatus;
    pfn_eciErrorMessage         pEciErrorMessage;
    pfn_eciClearErrors          pEciClearErrors;
    pfn_eciAddText              pEciAddText;
    pfn_eciInsertIndex          pEciInsertIndex;
    pfn_eciSynthesize           pEciSynthesize;
    pfn_eciSynthesizeFile       pEciSynthesizeFile;
    pfn_eciClearInput           pEciClearInput;
    pfn_eciGeneratePhonemes     pEciGeneratePhonemes;
    pfn_eciGetIndex             pEciGetIndex;
    pfn_eciStop                 pEciStop;
    pfn_eciSpeaking             pEciSpeaking;
    pfn_eciSynchronize          pEciSynchronize;
    pfn_eciPause                pEciPause;
    pfn_eciSetOutputBuffer      pEciSetOutputBuffer;
    pfn_eciSetOutputFilename    pEciSetOutputFilename;
    pfn_eciSetOutputDevice      pEciSetOutputDevice;
    pfn_eciGetParam             pEciGetParam;
    pfn_eciSetParam             pEciSetParam;
    pfn_eciGetDefaultParam      pEciGetDefaultParam;
    pfn_eciSetDefaultParam      pEciSetDefaultParam;
    pfn_eciCopyVoice            pEciCopyVoice;
    pfn_eciGetVoiceName         pEciGetVoiceName;
    pfn_eciSetVoiceName         pEciSetVoiceName;
    pfn_eciGetVoiceParam        pEciGetVoiceParam;
    pfn_eciSetVoiceParam        pEciSetVoiceParam;
    pfn_eciTestPhrase           pEciTestPhrase;
    pfn_eciSpeakText            pEciSpeakText;
    pfn_eciSpeakTextEx          pEciSpeakTextEx;
    pfn_eciRegisterCallback     pEciRegisterCallback;
    pfn_eciNewDict              pEciNewDict;
    pfn_eciGetDict              pEciGetDict;
    pfn_eciSetDict              pEciSetDict;
    pfn_eciDeleteDict           pEciDeleteDict;
    pfn_eciLoadDict             pEciLoadDict;
    pfn_eciSaveDict             pEciSaveDict;
} ECIFuncTable;
int ECILoader_Load(const char *dllDir, ECIFuncTable *pTable);
void ECILoader_Unload(ECIFuncTable *pTable);
#ifdef __cplusplus
}
#endif
#endif 
