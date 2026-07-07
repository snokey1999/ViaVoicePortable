#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"
#include "../ViaVoiceTTS.h" 
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "../ViaVoiceTTS.lib") 
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
HINSTANCE g_hInst = NULL;
int g_bInit = 0;
void UpdateSliderLabel(HWND hDlg, int sliderId, int labelId) {
    HWND hSlider = GetDlgItem(hDlg, sliderId);
    int pos = (int)SendMessage(hSlider, TBM_GETPOS, 0, 0);
    char buf[32];
    sprintf(buf, "%d", pos);
    SetDlgItemTextA(hDlg, labelId, buf);
}
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG: {
            int rc = VVTTS_Init("ViaVoiceTTS");
            if (rc != VVTTS_OK) {
                char cwd[256];
                GetCurrentDirectoryA(sizeof(cwd), cwd);
                char errMsg[1024];
                sprintf(errMsg, "Init Failed! rc=%d\nCWD: %s\nError: %s", rc, cwd, VVTTS_GetLastError());
                MessageBoxA(hDlg, errMsg, "TTS Init Failed", MB_ICONERROR);
            } else {
                g_bInit = 1;
            }
            SetDlgItemTextA(hDlg, IDC_EDIT_TEXT, "Welcome to ViaVoice TTS Portable!\r\nYou can adjust rate, pitch, and volume here.");
            HWND hCombo = GetDlgItem(hDlg, IDC_COMBO_VOICE);
            const char* voiceNames[] = {
                "Voice 0: Adult Male 1",
                "Voice 1: Adult Male 1",
                "Voice 2: Adult Female 1",
                "Voice 3: Child 1",
                "Voice 4: Adult Male 2",
                "Voice 5: Adult Male 3",
                "Voice 6: Adult Female 2",
                "Voice 7: Elderly Female 1"
            };
            for (int i = 0; i < 8; i++) {
                SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM)voiceNames[i]);
            }
            SendMessage(hCombo, CB_SETCURSEL, 0, 0); 
            HWND hRate = GetDlgItem(hDlg, IDC_SLIDER_RATE);
            SendMessage(hRate, TBM_SETRANGE, TRUE, MAKELPARAM(0, 250));
            SendMessage(hRate, TBM_SETPOS, TRUE, VVTTS_GetRate());
            HWND hPitch = GetDlgItem(hDlg, IDC_SLIDER_PITCH);
            SendMessage(hPitch, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
            SendMessage(hPitch, TBM_SETPOS, TRUE, VVTTS_GetPitch());
            HWND hVol = GetDlgItem(hDlg, IDC_SLIDER_VOLUME);
            SendMessage(hVol, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
            SendMessage(hVol, TBM_SETPOS, TRUE, VVTTS_GetVolume());
            HWND hHead = GetDlgItem(hDlg, IDC_SLIDER_HEADSIZE);
            SendMessage(hHead, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
            SendMessage(hHead, TBM_SETPOS, TRUE, VVTTS_GetHeadSize());
            HWND hRough = GetDlgItem(hDlg, IDC_SLIDER_ROUGHNESS);
            SendMessage(hRough, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
            SendMessage(hRough, TBM_SETPOS, TRUE, VVTTS_GetRoughness());
            HWND hBreath = GetDlgItem(hDlg, IDC_SLIDER_BREATHINESS);
            SendMessage(hBreath, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
            SendMessage(hBreath, TBM_SETPOS, TRUE, VVTTS_GetBreathiness());
            UpdateSliderLabel(hDlg, IDC_SLIDER_RATE, IDC_LBL_RATE);
            UpdateSliderLabel(hDlg, IDC_SLIDER_PITCH, IDC_LBL_PITCH);
            UpdateSliderLabel(hDlg, IDC_SLIDER_VOLUME, IDC_LBL_VOLUME);
            UpdateSliderLabel(hDlg, IDC_SLIDER_HEADSIZE, IDC_LBL_HEADSIZE);
            UpdateSliderLabel(hDlg, IDC_SLIDER_ROUGHNESS, IDC_LBL_ROUGHNESS);
            UpdateSliderLabel(hDlg, IDC_SLIDER_BREATHINESS, IDC_LBL_BREATHINESS);
            SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_COMBO_VOICE, CBN_SELCHANGE), (LPARAM)hCombo);
            return TRUE;
        }
        case WM_HSCROLL: {
            UpdateSliderLabel(hDlg, IDC_SLIDER_RATE, IDC_LBL_RATE);
            UpdateSliderLabel(hDlg, IDC_SLIDER_PITCH, IDC_LBL_PITCH);
            UpdateSliderLabel(hDlg, IDC_SLIDER_VOLUME, IDC_LBL_VOLUME);
            UpdateSliderLabel(hDlg, IDC_SLIDER_HEADSIZE, IDC_LBL_HEADSIZE);
            UpdateSliderLabel(hDlg, IDC_SLIDER_ROUGHNESS, IDC_LBL_ROUGHNESS);
            UpdateSliderLabel(hDlg, IDC_SLIDER_BREATHINESS, IDC_LBL_BREATHINESS);
            return TRUE;
        }
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);
            if (wmId == IDC_COMBO_VOICE && wmEvent == CBN_SELCHANGE) {
                HWND hCombo = GetDlgItem(hDlg, IDC_COMBO_VOICE);
                int voiceId = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                if (voiceId >= 0 && voiceId < 8) {
                    VVTTS_SetVoice(voiceId);
                    SendMessage(GetDlgItem(hDlg, IDC_SLIDER_PITCH), TBM_SETPOS, TRUE, VVTTS_GetPitch());
                    SendMessage(GetDlgItem(hDlg, IDC_SLIDER_RATE),  TBM_SETPOS, TRUE, VVTTS_GetRate());
                    SendMessage(GetDlgItem(hDlg, IDC_SLIDER_HEADSIZE), TBM_SETPOS, TRUE, VVTTS_GetHeadSize());
                    SendMessage(GetDlgItem(hDlg, IDC_SLIDER_ROUGHNESS), TBM_SETPOS, TRUE, VVTTS_GetRoughness());
                    SendMessage(GetDlgItem(hDlg, IDC_SLIDER_BREATHINESS), TBM_SETPOS, TRUE, VVTTS_GetBreathiness());
                    UpdateSliderLabel(hDlg, IDC_SLIDER_RATE, IDC_LBL_RATE);
                    UpdateSliderLabel(hDlg, IDC_SLIDER_PITCH, IDC_LBL_PITCH);
                    UpdateSliderLabel(hDlg, IDC_SLIDER_HEADSIZE, IDC_LBL_HEADSIZE);
                    UpdateSliderLabel(hDlg, IDC_SLIDER_ROUGHNESS, IDC_LBL_ROUGHNESS);
                    UpdateSliderLabel(hDlg, IDC_SLIDER_BREATHINESS, IDC_LBL_BREATHINESS);
                }
                return TRUE;
            }
            if (wmId == IDC_BTN_SPEAK) {
                if (!g_bInit) {
                    MessageBoxA(hDlg, "Engine not initialized! Check DLLs.", "Error", MB_ICONERROR);
                    return TRUE;
                }
                HWND hCombo = GetDlgItem(hDlg, IDC_COMBO_VOICE);
                int voiceId = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                if (voiceId == CB_ERR) voiceId = 0;
                int rate = (int)SendMessage(GetDlgItem(hDlg, IDC_SLIDER_RATE), TBM_GETPOS, 0, 0);
                int pitch = (int)SendMessage(GetDlgItem(hDlg, IDC_SLIDER_PITCH), TBM_GETPOS, 0, 0);
                int vol = (int)SendMessage(GetDlgItem(hDlg, IDC_SLIDER_VOLUME), TBM_GETPOS, 0, 0);
                int head = (int)SendMessage(GetDlgItem(hDlg, IDC_SLIDER_HEADSIZE), TBM_GETPOS, 0, 0);
                int rough = (int)SendMessage(GetDlgItem(hDlg, IDC_SLIDER_ROUGHNESS), TBM_GETPOS, 0, 0);
                int breath = (int)SendMessage(GetDlgItem(hDlg, IDC_SLIDER_BREATHINESS), TBM_GETPOS, 0, 0);
                VVTTS_SetVoice(voiceId);
                VVTTS_SetRate(rate);
                VVTTS_SetPitch(pitch);
                VVTTS_SetVolume(vol);
                VVTTS_SetHeadSize(head);
                VVTTS_SetRoughness(rough);
                VVTTS_SetBreathiness(breath);
                HWND hEdit = GetDlgItem(hDlg, IDC_EDIT_TEXT);
                int len = GetWindowTextLength(hEdit);
                if (len > 0) {
                    char* buf = (char*)malloc(len + 1);
                    GetWindowTextA(hEdit, buf, len + 1);
                    VVTTS_Stop();
                    VVTTS_SpeakAsync(buf);
                    free(buf);
                }
                return TRUE;
            }
            else if (wmId == IDC_BTN_STOP) {
                if (g_bInit) VVTTS_Stop();
                return TRUE;
            }
            else if (wmId == IDCANCEL) { 
                if (g_bInit) VVTTS_Uninit();
                EndDialog(hDlg, 0);
                return TRUE;
            }
            break;
        }
        case WM_CLOSE:
            if (g_bInit) VVTTS_Uninit();
            EndDialog(hDlg, 0);
            return TRUE;
    }
    return FALSE;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    g_hInst = hInstance;
    InitCommonControls(); 
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
    return 0;
}
