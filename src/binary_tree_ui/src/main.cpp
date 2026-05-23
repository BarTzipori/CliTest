#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <sstream>
#include <vector>
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
        size_t start = token.find_first_not_of(L" \t");
        if (start == std::wstring::npos) continue;
        size_t end = token.find_last_not_of(L" \t");
        std::wstring t = token.substr(start, end - start + 1);
        int v = _wtoi(t.c_str());
        out.push_back(v);
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
            if (size == 0) break;
            std::vector<BYTE> bufEnc(size);
            ImageCodecInfo* pImageCodecInfo = reinterpret_cast<ImageCodecInfo*>(bufEnc.data());
            GetImageEncoders(numEncoders, size, pImageCodecInfo);
            CLSID pngClsid = {0};
            for (UINT j = 0; j < numEncoders; ++j) {
                if (wcscmp(pImageCodecInfo[j].MimeType, L"image/png") == 0) {
                    pngClsid = pImageCodecInfo[j].Clsid;
                    break;
                }
            }
            std::wstring file = L"binary_tree_export.png";
            bmp.Save(file.c_str(), &pngClsid, NULL);
            MessageBox(hwnd, L"Exported to binary_tree_export.png", L"Export", MB_OK);
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
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    const wchar_t CLASS_NAME[] = L"BinaryTreeWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Binary Tree UI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 700, NULL, NULL, hInstance, NULL);
    if (!hwnd) return 0;
    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(g_gdiplusToken);
    return 0;
}
