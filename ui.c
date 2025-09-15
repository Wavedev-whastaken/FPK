#include <windows.h>
#include "fpk.h"
#include <commdlg.h>

char files[FPK_MAX_FILES][FPK_MAX_NAME];
int file_count = 0;
char meta[FPK_MAX_META] = "Created with FPK UI";

void add_file(HWND hwnd) {
    OPENFILENAMEA ofn = {0};
    char fname[FPK_MAX_NAME] = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fname;
    ofn.nMaxFile = FPK_MAX_NAME;
    ofn.Flags = OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn)) {
        if (file_count < FPK_MAX_FILES) {
            strcpy(files[file_count++], fname);
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
}

void save_pack(HWND hwnd) {
    OPENFILENAMEA ofn = {0};
    char fname[FPK_MAX_NAME] = "archive.fpk";
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fname;
    ofn.nMaxFile = FPK_MAX_NAME;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    if (GetSaveFileNameA(&ofn)) {
        const char *file_ptrs[FPK_MAX_FILES];
        for (int i = 0; i < file_count; ++i) file_ptrs[i] = files[i];
        int res = fpk_pack(fname, file_ptrs, file_count, meta);
        MessageBoxA(hwnd, res ? "Failed to pack." : "Archive created!", "FPK UI", MB_OK);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        CreateWindowA("BUTTON", "Add File", WS_CHILD|WS_VISIBLE, 10,10,80,25, hwnd, (HMENU)1, NULL, NULL);
        CreateWindowA("BUTTON", "Save FPK", WS_CHILD|WS_VISIBLE, 100,10,80,25, hwnd, (HMENU)2, NULL, NULL);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) add_file(hwnd);
        if (LOWORD(wParam) == 2) save_pack(hwnd);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOutA(hdc, 10, 50, "Files to pack:", 13);
        for (int i = 0; i < file_count; ++i)
            TextOutA(hdc, 10, 70 + i*16, files[i], strlen(files[i]));
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "FPKUI";
    RegisterClassA(&wc);
    HWND hwnd = CreateWindowA("FPKUI", "FPK GUI", WS_OVERLAPPEDWINDOW|WS_VISIBLE, 100,100,400,400,0,0,hInst,0);
    MSG msg;
    while (GetMessage(&msg,0,0,0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}