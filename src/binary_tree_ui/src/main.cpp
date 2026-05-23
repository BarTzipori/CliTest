#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <errno.h>
#include "..\include\Tree.h"

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#define ID_INPUT 101
#define ID_BUILD 102
#define ID_EXPORT 103

BinaryTree g_tree;
HWND g_hEdit = nullptr;
ULONG_PTR g_gdiplusToken = 0;

void ParseInput(const std::wstring& s, std::vector<int>& out) {
    std::wstring token;
    std::wistringstream iss(s);
    while (std::getline(iss, token, L',')) {
        // trim whitespace (space, tab, CR, LF)
        size_t start = token.find_first_not_of(L" \t\r\n");
        if (start == std::wstring::npos) continue;
        size_t end = token.find_last_not_of(L" \t\r\n");
        std::wstring t = token.substr(start, end - start + 1);

        wchar_t* endptr = nullptr;
        errno = 0;
        long val = wcstol(t.c_str(), &endptr, 10);
        if (endptr == t.c_str()) {
            // no conversion; skip token
            continue;
        }
        if (errno == ERANGE) {
            // out of range; skip token
            continue;
        }
        out.push_back(static_cast<int>(val));
    }
}

void DrawNodeRec(Graphics& g, TreeNode* node, Font& font, Pen& penLine, SolidBrush& brushNode, SolidBrush& brushText) {
    if (!node) return;
    if (node->left) {
        g.DrawLine(&penLine, (REAL)node->x, (REAL)node->y, (REAL)node->left->x, (REAL)node->left->y);
        DrawNodeRec(g, node->left, font, penLine, brushNode, brushText);
    }
    if (node->right) {
        g.DrawLine(&penLine, (REAL)node->x, (REAL)node->y, (REAL)node->right->x, (REAL)node->right->y);
        DrawNodeRec(g, node->right, font, penLine, brushNode, brushText);
    }
    int r = 18;
    g.FillEllipse(&brushNode, node->x - r, node->y - r, r*2, r*2);
    g.DrawEllipse(&penLine, node->x - r, node->y - r, r*2, r*2);
    std::wstring text = std::to_wstring(node->val);
    RectF rectF((REAL)(node->x - r), (REAL)(node->y - r), (REAL)(r*2), (REAL)(r*2));
    StringFormat fmt;
    fmt.SetAlignment(StringAlignmentCenter);
    fmt.SetLineAlignment(StringAlignmentCenter);
    g.DrawString(text.c_str(), -1, &font, rectF, &fmt, &brushText);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_hEdit = CreateWindowEx(0, L"EDIT", L"1,2,3,4,5,6", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 10, 520, 24, hwnd, (HMENU)ID_INPUT, GetModuleHandle(NULL), NULL);
        CreateWindowEx(0, L"BUTTON", L"Build Tree", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 540, 8, 120, 28, hwnd, (HMENU)ID_BUILD, GetModuleHandle(NULL), NULL);
        CreateWindowEx(0, L"BUTTON", L"Export PNG", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 670, 8, 120, 28, hwnd, (HMENU)ID_EXPORT, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        if (id == ID_BUILD && code == BN_CLICKED) {
            int len = GetWindowTextLengthW(g_hEdit) + 1;
            std::wstring buf; buf.resize(len);
            GetWindowTextW(g_hEdit, &buf[0], len);
            if(!buf.empty()) buf.resize(len-1);
            std::vector<int> vals;
            ParseInput(buf, vals);
            g_tree.clear();
            for (int v : vals) g_tree.insert(v);
            g_tree.assignPositions();
            InvalidateRect(hwnd, NULL, TRUE);
        } else if (id == ID_EXPORT && code == BN_CLICKED) {
            RECT rc; GetClientRect(hwnd, &rc);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;
            if (width <= 0 || height <= 0) {
                MessageBox(hwnd, L"Cannot export: window has invalid size.", L"Export Error", MB_OK | MB_ICONERROR);
                break;
            }
            Bitmap bmp(width, height, PixelFormat32bppARGB);
            Graphics g(&bmp);
            g.Clear(Color::White);
            FontFamily ff(L"Segoe UI");
            Font font(&ff, 12, FontStyleRegular, UnitPixel);
            Pen pen(Color(255,0,0,0), 2);
            SolidBrush brushNode(Color(255,200,200,240));
            SolidBrush brushText(Color::Black);
            if (g_tree.getRoot()) DrawNodeRec(g, g_tree.getRoot(), font, pen, brushNode, brushText);
            UINT numEncoders = 0; UINT size = 0;
            GetImageEncodersSize(&numEncoders, &size);
            if (numEncoders == 0 || size == 0) {
                MessageBox(hwnd, L"No image encoders available.", L"Export Error", MB_OK | MB_ICONERROR);
                break;
            }
            std::vector<BYTE> bufEnc(size);
            ImageCodecInfo* pImageCodecInfo = reinterpret_cast<ImageCodecInfo*>(bufEnc.data());
            GetImageEncoders(numEncoders, size, pImageCodecInfo);
            CLSID pngClsid = {0};
            bool foundPng = false;
            for (UINT j = 0; j < numEncoders; ++j) {
                if (pImageCodecInfo[j].MimeType && wcscmp(pImageCodecInfo[j].MimeType, L"image/png") == 0) {
                    pngClsid = pImageCodecInfo[j].Clsid;
                    foundPng = true;
                    break;
                }
            }
            if (!foundPng) {
                MessageBox(hwnd, L"PNG encoder not found.", L"Export Error", MB_OK | MB_ICONERROR);
                break;
            }
            std::wstring file = L"binary_tree_export.png";
            Status saveStatus = bmp.Save(file.c_str(), &pngClsid, NULL);
            if (saveStatus != Ok) {
                MessageBox(hwnd, L"Failed to save PNG.", L"Export Error", MB_OK | MB_ICONERROR);
            } else {
                MessageBox(hwnd, L"Exported to binary_tree_export.png", L"Export", MB_OK);
            }
        }
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Graphics g(hdc);
        g.Clear(Color::White);
        FontFamily ff(L"Segoe UI");
        Font font(&ff, 12, FontStyleRegular, UnitPixel);
        Pen pen(Color(255,0,0,0), 2);
        SolidBrush brushNode(Color(255,200,200,240));
        SolidBrush brushText(Color::Black);
        if (g_tree.getRoot()) {
            DrawNodeRec(g, g_tree.getRoot(), font, pen, brushNode, brushText);
        }
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    GdiplusStartupInput gdiplusStartupInput;
    Status gdStat = GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
    if (gdStat != Ok) {
        MessageBox(NULL, L"Failed to initialize GDI+.", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    const wchar_t CLASS_NAME[] = L"BinaryTreeWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        GdiplusShutdown(g_gdiplusToken);
        MessageBox(NULL, L"RegisterClass failed", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Binary Tree UI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 700, NULL, NULL, hInstance, NULL);
    if (!hwnd) {
        GdiplusShutdown(g_gdiplusToken);
        MessageBox(NULL, L"CreateWindowEx failed", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(g_gdiplusToken);
    return 0;
}
