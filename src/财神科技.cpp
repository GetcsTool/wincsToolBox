// 财神科技.cpp : 定义应用程序的入口点。
// 财神工具箱 - 暗黑主题工具箱
//

#include "framework.h"
#include "财神科技.h"

#include <commctrl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <ifdef.h>
#include <iprtrmib.h>
#include <iphlpapi.h>
#include <urlmon.h>
#include <wininet.h>
#include <gdiplus.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <cstdio>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comdlg32.lib")

using namespace Gdiplus;

#define MAX_LOADSTRING 100

#define WINDOW_WIDTH   1080
#define WINDOW_HEIGHT 685

#define SIDEBAR_WIDTH    170
#define MARGIN           10
#define MENU_ITEM_HEIGHT 38
#define MENU_TOP_OFFSET  12
#define STATUSBAR_H      30

#define MENUBAR_H        26
#define MENUBAR_ITEM_W   56

#define ICON_SIZE   32
#define ITEM_W      94
#define ITEM_H      84
#define GRID_TOP    10

#define MENU_ICON_SIZE  18

#define DOCK_VISIBLE     4
#define DOCK_THRESHOLD   10
#define DOCK_TIMER_ID    100
#define DOCK_TIMER_ELAPSE 50
#define DOCK_ANIM_STEP   60

#define STATUS_TIMER_ID  101
#define STATUS_TIMER_MS  1000

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR 35
#endif
#ifndef DWMWA_TEXT_COLOR
#define DWMWA_TEXT_COLOR 36
#endif
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

#define CLR_BG          RGB(0x1A, 0x1B, 0x1E)
#define CLR_SIDEBAR     RGB(0x1A, 0x1B, 0x1E)
#define CLR_MENU_HOVER  RGB(0x2A, 0x2B, 0x30)
#define CLR_MENU_SEL    RGB(0x2D, 0x2E, 0x33)
#define CLR_CONTENT     RGB(0x1A, 0x1B, 0x1E)
#define CLR_STATUSBAR   RGB(0x16, 0x17, 0x19)
#define CLR_TEXT        RGB(0xE8, 0xE8, 0xE8)
#define CLR_TEXT_DIM    RGB(0x9A, 0x9A, 0x9A)
#define CLR_LINE        RGB(0x2D, 0x2E, 0x33)
#define CLR_ACCENT      RGB(0x0A, 0x85, 0xD9)
#define CLR_NET_UP      RGB(0x4C, 0xAF, 0x50)
#define CLR_NET_DOWN    RGB(0x42, 0xA5, 0xF5)

#define POP_ITEM_H  32
#define POP_MENU_W  190
#define POP_CLASS   L"CaishenPopupMenu"
#define IDM_TRAY_SHOW  10001
#define IDM_TRAY_EXIT  10002
#define POP_OPEN         1
#define POP_COPY_PROG    2
#define POP_ADMIN        3
#define POP_COPY         4
#define POP_FOLDER       5
#define POP_ADD_FAVOR    6
#define POP_REMOVE_FAVOR 7
#define POP_DELETE       8
#define POP_PROPERTIES   9

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
ULONG_PTR g_gdiToken = 0;

HWND g_hWndMain      = nullptr;
HWND g_hStcPlaceholder = nullptr;
HFONT g_hFontUI      = nullptr;
HFONT g_hFontMenu    = nullptr;
HFONT g_hFontStatus  = nullptr;

#define WM_TRAYICON (WM_APP + 1)
static NOTIFYICONDATAW g_nid = { 0 };

struct ToolItem {
    std::wstring name;
    std::wstring path;
    std::wstring params;
    HICON icon;
    bool isSubFolder;
    ToolItem() : icon(nullptr), isSubFolder(false) {}
};
std::vector<ToolItem> g_tools;
int g_selectedTool = -1;
int g_hoverTool    = -1;

int  g_siScroll    = 0;
int  g_siScrollMax = 0;
BOOL g_siDragBar   = FALSE;
int  g_siDragStartY = 0;
int  g_siDragStartScroll = 0;

int  g_toolScroll    = 0;
int  g_toolScrollMax = 0;
BOOL g_toolDragBar   = FALSE;
int  g_toolDragStartY = 0;
int  g_toolDragStartScroll = 0;

struct SysCard {
    const WCHAR* title;
    std::vector<std::pair<std::wstring, std::wstring>> items;
};
std::vector<SysCard> g_siCards;
BOOL g_siCardsBuilt = FALSE;

HBRUSH g_brBg       = nullptr;
HBRUSH g_brSidebar  = nullptr;
HBRUSH g_brContent  = nullptr;
HBRUSH g_brStatus   = nullptr;

HDC    g_memDC      = nullptr;
HBITMAP g_memBmp    = nullptr;
HBITMAP g_memOldBmp = nullptr;
int    g_memCx      = 0;
int    g_memCy      = 0;

HFONT  g_hFontBold  = nullptr;
HPEN   g_penLine    = nullptr;
HBRUSH g_brMenuSel  = nullptr;
HBRUSH g_brMenuHover = nullptr;
HBRUSH g_brAccent   = nullptr;
HBRUSH g_brCardBg   = nullptr;
HPEN   g_penCardBd  = nullptr;
HBRUSH g_brThumb    = nullptr;
HBRUSH g_brTrack    = nullptr;

int  g_selectedMenu = IDC_MENU_UNINSTALL;
int  g_hoverMenu    = 0;
BOOL g_trackingMouse = FALSE;

int  g_dockSide    = 0;
BOOL g_dockHidden  = TRUE;
RECT g_dockRect    = {0};
BOOL g_dockAnim    = FALSE;
int  g_dockAnimTX  = 0;
int  g_dockAnimTY  = 0;

ULONG64 g_lastInBytes  = 0;
ULONG64 g_lastOutBytes = 0;
ULONG64 g_netUp        = 0;
ULONG64 g_netDown      = 0;
WCHAR   g_extIP[64]      = { 0 };
WCHAR   g_extRegion[128] = { 0 };
BOOL    g_ipFetched      = FALSE;

struct PopMenuItem { const WCHAR* text; int id; BOOL separator; };
static PopMenuItem g_popItems[] = {
    { L"打开",              POP_OPEN,         FALSE },
    { L"复制",              POP_COPY_PROG,    FALSE },
    { L"以管理员身份运行",  POP_ADMIN,        FALSE },
    { L"复制路径",          POP_COPY,         FALSE },
    { L"打开所在文件夹",    POP_FOLDER,       FALSE },
    { L"添加到个人常用",    POP_ADD_FAVOR,    FALSE },
    { L"删除",              POP_DELETE,       FALSE },
    { L"属性",              POP_PROPERTIES,   FALSE },
};
static int g_popCount = ARRAYSIZE(g_popItems);
HWND g_hPopMenu      = nullptr;
int  g_popHover      = -1;
std::wstring g_popTarget;
BOOL g_isMenuBarPop  = FALSE;

struct MenuBarItem { const WCHAR* text; };
static MenuBarItem g_menuBar[] = {
    { L"关于" },
};
static const int g_menuBarCount = ARRAYSIZE(g_menuBar);

struct MenuBarCmd { const WCHAR* text; int id; BOOL separator; };
static MenuBarCmd g_menuBarCmds[][8] = {
    { {L"关于...", IDM_ABOUT, FALSE} },
};
static const int g_menuBarCmdCounts[] = { 1 };

int g_hoverMenuBar  = -1;
int g_openMenuBar   = -1;
int g_pressedMenuBar = -1;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AboutWndProc(HWND, UINT, WPARAM, LPARAM);
ATOM  RegisterAboutClass(HINSTANCE hInstance);
void  ShowAboutBox(HWND hParent);
void                CreateControls(HWND hWnd);
void                LayoutControls();
void                ShowCurrentPage();
void                BuildSysInfoCards();
void                LoadToolsForMenu(int menuId);
void                LoadFavorites(std::vector<ToolItem>& out);
void                AddFavorite(const std::wstring& path);
void                DrawSysInfoPage(HDC hdc, int cx, int cy);
void                OnPaint(HWND hWnd);
void                DrawSideBar(HDC hdc, int cx, int cy);
void                DrawMenuItem(HDC hdc, int index, const RECT& rcItem);
void                DrawStatusBar(HDC hdc, int cx, int cy);
void                DrawToolGrid(HDC hdc, int cx, int cy);
int                 HitTestMenu(int x, int y);
int                 HitTestTool(int x, int y);
void                CalcToolRect(int idx, int contentX, int contentW, RECT& rc);
void                ApplyDarkTitleBar(HWND hWnd);
void                DockCheckOnMove(HWND hWnd);
void                DockGetHidePos(HWND hWnd, int& x, int& y);
void                DockStartAnim(HWND hWnd, int tx, int ty);
void                DockOnTimer(HWND hWnd);
ATOM                RegisterPopMenuClass(HINSTANCE hInstance);
LRESULT CALLBACK    PopMenuProc(HWND, UINT, WPARAM, LPARAM);
void                ShowPopupMenu(HWND hOwner, int x, int y, const std::wstring& path);
void                ShowMenuBarPopup(HWND hOwner, int x, int y, int menuIdx);
void                ExecutePopCommand(HWND hOwner, int cmd, const std::wstring& path);
void                CopyTextToClipboard(const std::wstring& text);
void                CopyFilesToClipboard(const std::vector<std::wstring>& paths);
void                UpdateNetworkSpeed();
void                FormatSpeed(WCHAR* buf, size_t len, ULONG64 bps);
DWORD WINAPI        FetchExtIpThread(LPVOID);

struct MenuItem {
    const WCHAR* name;
    int id;
    HICON icon;
};

static MenuItem g_menus[] = {
    { L"系统信息",  IDC_MENU_SYSINFO,  nullptr },

    { L"个人常用",  IDC_MENU_FAVORITE, nullptr },
    { L"清理工具",  IDC_MENU_CLEAN,    nullptr },
    { L"卸载工具",  IDC_MENU_UNINSTALL,nullptr },
    { L"优化工具",  IDC_MENU_OPTIMIZE, nullptr },
    { L"CPU工具",   IDC_MENU_CPU,      nullptr },
    { L"烤机工具",  IDC_MENU_STRESS,   nullptr },

    { L"内存工具",  IDC_MENU_MEMORY,   nullptr },
    { L"显卡工具",  IDC_MENU_GPU,      nullptr },
    { L"磁盘工具",  IDC_MENU_DISK,     nullptr },
    { L"屏幕工具",  IDC_MENU_SCREEN,   nullptr },
    { L"美化工具",  IDC_MENU_BEAUTIFY, nullptr },
    { L"游戏工具",  IDC_MENU_GAME,     nullptr },
    { L"系统工具",  IDC_MENU_SYSTEM,   nullptr },
    { L"综合工具",  IDC_MENU_MISC,     nullptr },
};
static const int g_menuCount = ARRAYSIZE(g_menus);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    GdiplusStartupInput gdiInput;
    GdiplusStartup(&g_gdiToken, &gdiInput, nullptr);

    INITCOMMONCONTROLSEX icc = { 0 };
    icc.dwSize = sizeof(icc);
    icc.dwICC  = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    RegisterPopMenuClass(hInstance);
    RegisterAboutClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        GdiplusShutdown(g_gdiToken);
        return FALSE;
    }

    MSG msg;
    BOOL ret;
    while ((ret = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        if (ret == -1) break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(g_gdiToken);
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = nullptr;
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

void ApplyDarkTitleBar(HWND hWnd)
{
    BOOL dark = TRUE;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    COLORREF c1 = CLR_BG;  DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &c1, sizeof(c1));
    COLORREF c2 = CLR_TEXT; DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR, &c2, sizeof(c2));
    COLORREF c3 = CLR_BG;  DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &c3, sizeof(c3));
}

void DockGetHidePos(HWND hWnd, int& x, int& y)
{
    RECT rc; GetWindowRect(hWnd, &rc);
    int wW = rc.right - rc.left, wH = rc.bottom - rc.top;
    int sw = GetSystemMetrics(SM_CXSCREEN);
    x = g_dockRect.left; y = g_dockRect.top;
    if (g_dockSide == 1)      x = -(wW - DOCK_VISIBLE);
    else if (g_dockSide == 2) x = sw - DOCK_VISIBLE;
    else if (g_dockSide == 3) y = -(wH - DOCK_VISIBLE);
}

void DockStartAnim(HWND hWnd, int tx, int ty)
{
    g_dockAnimTX = tx; g_dockAnimTY = ty; g_dockAnim = TRUE;
    SetTimer(hWnd, DOCK_TIMER_ID, DOCK_TIMER_ELAPSE, nullptr);
}

void DockCheckOnMove(HWND hWnd)
{
    RECT rc; GetWindowRect(hWnd, &rc);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    g_dockSide = 0;
    if (rc.left < -DOCK_THRESHOLD && rc.right > 0) g_dockSide = 1;
    else if (rc.right > sw + DOCK_THRESHOLD && rc.left < sw) g_dockSide = 2;
    else if (rc.top < -DOCK_THRESHOLD && rc.bottom > 0) g_dockSide = 3;
    if (g_dockSide != 0)
    {
        g_dockRect = rc; g_dockHidden = TRUE;
        int hx, hy; DockGetHidePos(hWnd, hx, hy);
        DockStartAnim(hWnd, hx, hy);
    }
}

void DockOnTimer(HWND hWnd)
{
    if (g_dockSide == 0) { KillTimer(hWnd, DOCK_TIMER_ID); return; }
    if (g_dockAnim)
    {
        RECT rc; GetWindowRect(hWnd, &rc);
        int cx = rc.left, cy = rc.top;
        int dx = g_dockAnimTX - cx, dy = g_dockAnimTY - cy;
        if (abs(dx) <= DOCK_ANIM_STEP && abs(dy) <= DOCK_ANIM_STEP)
        {
            SetWindowPos(hWnd, nullptr, g_dockAnimTX, g_dockAnimTY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            g_dockAnim = FALSE;
        }
        else
        {
            cx += (dx > 0) ? min(DOCK_ANIM_STEP, dx) : max(-DOCK_ANIM_STEP, dx);
            cy += (dy > 0) ? min(DOCK_ANIM_STEP, dy) : max(-DOCK_ANIM_STEP, dy);
            SetWindowPos(hWnd, nullptr, cx, cy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        return;
    }
    POINT pt; GetCursorPos(&pt);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    if (g_dockHidden)
    {
        BOOL show = FALSE;
        if (g_dockSide == 1 && pt.x <= DOCK_VISIBLE + 2 && pt.y >= g_dockRect.top && pt.y <= g_dockRect.bottom) show = TRUE;
        if (g_dockSide == 2 && pt.x >= sw - DOCK_VISIBLE - 2 && pt.y >= g_dockRect.top && pt.y <= g_dockRect.bottom) show = TRUE;
        if (g_dockSide == 3 && pt.y <= DOCK_VISIBLE + 2 && pt.x >= g_dockRect.left && pt.x <= g_dockRect.right) show = TRUE;
        if (show) { g_dockHidden = FALSE; DockStartAnim(hWnd, g_dockRect.left, g_dockRect.top); }
    }
    else
    {
        RECT rc; GetWindowRect(hWnd, &rc);
        if (!PtInRect(&rc, pt))
        {
            g_dockHidden = TRUE;
            int hx, hy; DockGetHidePos(hWnd, hx, hy);
            DockStartAnim(hWnd, hx, hy);
        }
    }
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;
   DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle,
      CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd) return FALSE;
   ApplyDarkTitleBar(hWnd);
   RECT rc; GetWindowRect(hWnd, &rc);
   int x = (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2;
   int y = (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2;
   SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

void LoadMenuIcons()
{
    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    for (int i = 0; i < g_menuCount; i++)
    {

        WCHAR path[MAX_PATH];
        swprintf_s(path, L"%s\\icons\\%s.png", exePath, g_menus[i].name);
        DWORD attr = GetFileAttributesW(path);
        if (attr == INVALID_FILE_ATTRIBUTES) continue;

        FILE* fp = nullptr;
        if (_wfopen_s(&fp, path, L"rb") == 0 && fp)
        {
            BYTE header[4] = { 0 };
            fread(header, 1, 4, fp);
            fclose(fp);
            if (header[0] == 0x00 && header[1] == 0x00 && header[2] == 0x01 && header[3] == 0x00)
            {
                HICON hIcon = (HICON)LoadImageW(nullptr, path, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
                if (hIcon) { g_menus[i].icon = hIcon; continue; }
            }
        }

        Bitmap* bmp = Bitmap::FromFile(path, FALSE);
        if (!bmp || bmp->GetLastStatus() != Ok) { delete bmp; continue; }
        HICON hIcon = nullptr;
        bmp->GetHICON(&hIcon);
        delete bmp;
        if (hIcon) g_menus[i].icon = hIcon;
    }
}

void CreateControls(HWND hWnd)
{
    g_hFontUI = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    g_hFontMenu = CreateFontW(-13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    g_hFontStatus = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");

    g_brBg      = CreateSolidBrush(CLR_BG);
    g_brSidebar = CreateSolidBrush(CLR_SIDEBAR);
    g_brContent = CreateSolidBrush(CLR_CONTENT);
    g_brStatus  = CreateSolidBrush(CLR_STATUSBAR);

    g_hFontBold = CreateFontW(-15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    g_penLine   = CreatePen(PS_SOLID, 1, CLR_LINE);
    g_brMenuSel = CreateSolidBrush(CLR_MENU_SEL);
    g_brMenuHover = CreateSolidBrush(CLR_MENU_HOVER);
    g_brAccent  = CreateSolidBrush(CLR_ACCENT);
    g_brCardBg  = CreateSolidBrush(RGB(0x21, 0x22, 0x26));
    g_penCardBd = CreatePen(PS_SOLID, 1, RGB(0x2D, 0x2E, 0x33));
    g_brThumb   = CreateSolidBrush(RGB(0x4A, 0x4B, 0x50));
    g_brTrack   = CreateSolidBrush(CLR_LINE);

    LoadMenuIcons();

    g_hStcPlaceholder = CreateWindowW(L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 0, 0, 0, hWnd, (HMENU)IDC_STC_PLACEHOLDER, hInst, nullptr);
    SendMessageW(g_hStcPlaceholder, WM_SETFONT, (WPARAM)g_hFontUI, TRUE);

    SetTimer(hWnd, STATUS_TIMER_ID, STATUS_TIMER_MS, nullptr);
    UpdateNetworkSpeed();
    CreateThread(nullptr, 0, FetchExtIpThread, nullptr, 0, nullptr);

    LayoutControls();
    ShowCurrentPage();
}

void LayoutControls()
{
    RECT rc;
    GetClientRect(g_hWndMain, &rc);
    int cx = rc.right;
    int cy = rc.bottom;
    int contentX = SIDEBAR_WIDTH;
    int contentW = cx - SIDEBAR_WIDTH;
    int contentH = cy - STATUSBAR_H - MENUBAR_H;
    MoveWindow(g_hStcPlaceholder, contentX, MENUBAR_H, contentW, contentH, TRUE);
}

void CalcToolRect(int idx, int contentX, int contentW, RECT& rc)
{
    int usableW = contentW - MARGIN * 2 - 10;
    int cols = usableW / ITEM_W;
    if (cols < 1) cols = 1;
    int row = idx / cols;
    int col = idx % cols;
    rc.left   = contentX + MARGIN + col * ITEM_W;
    rc.top    = GRID_TOP + row * ITEM_H - g_toolScroll;
    rc.right  = rc.left + ITEM_W;
    rc.bottom = rc.top + ITEM_H;
}

int HitTestTool(int x, int y)
{
    RECT rc; GetClientRect(g_hWndMain, &rc);
    int contentX = SIDEBAR_WIDTH;
    int contentW = rc.right - SIDEBAR_WIDTH;
    for (size_t i = 0; i < g_tools.size(); i++)
    {
        RECT rcItem;
        CalcToolRect((int)i, contentX, contentW, rcItem);
        if (PtInRect(&rcItem, POINT{ x, y })) return (int)i;
    }
    return -1;
}

void GetFavoritesPath(WCHAR* path)
{
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    PathRemoveFileSpecW(path);
    wcscat_s(path, MAX_PATH, L"\\favorites.txt");
}

void LoadFavorites(std::vector<ToolItem>& out)
{
    WCHAR favPath[MAX_PATH];
    GetFavoritesPath(favPath);
    FILE* f = nullptr;
    _wfopen_s(&f, favPath, L"r, ccs=UTF-8");
    if (!f) return;
    WCHAR line[MAX_PATH];
    while (fgetws(line, MAX_PATH, f))
    {
        size_t len = wcslen(line);
        while (len > 0 && (line[len-1] == L'\n' || line[len-1] == L'\r')) line[--len] = 0;
        if (len == 0) continue;
        DWORD attr = GetFileAttributesW(line);
        if (attr == INVALID_FILE_ATTRIBUTES) continue;
        ToolItem ti;
        ti.path = line;
        WCHAR name[MAX_PATH];
        wcscpy_s(name, line);
        PathStripPathW(name);
        PathRemoveExtensionW(name);
        ti.name = name;
        ti.icon = ExtractIconW(hInst, line, 0);
        if (ti.icon == (HICON)1) ti.icon = nullptr;
        out.push_back(ti);
    }
    fclose(f);
}

void AddFavorite(const std::wstring& path)
{
    WCHAR favPath[MAX_PATH];
    GetFavoritesPath(favPath);
    std::vector<ToolItem> existing;
    LoadFavorites(existing);
    for (auto& t : existing) if (t.icon) DestroyIcon(t.icon);
    for (auto& t : existing)
    {
        if (_wcsicmp(t.path.c_str(), path.c_str()) == 0) return;
    }
    FILE* f = nullptr;
    _wfopen_s(&f, favPath, L"a, ccs=UTF-8");
    if (!f) return;
    fwprintf_s(f, L"%s\n", path.c_str());
    fclose(f);
}

BOOL IsFavorite(const std::wstring& path)
{
    std::vector<ToolItem> favs;
    LoadFavorites(favs);
    for (auto& t : favs) if (t.icon) DestroyIcon(t.icon);
    for (auto& t : favs)
    {
        if (_wcsicmp(t.path.c_str(), path.c_str()) == 0) return TRUE;
    }
    return FALSE;
}

void RemoveFavorite(const std::wstring& path)
{
    WCHAR favPath[MAX_PATH];
    GetFavoritesPath(favPath);
    std::vector<ToolItem> favs;
    LoadFavorites(favs);
    FILE* f = nullptr;
    _wfopen_s(&f, favPath, L"w, ccs=UTF-8");
    if (!f) { for (auto& t : favs) if (t.icon) DestroyIcon(t.icon); return; }
    for (auto& t : favs)
    {
        if (_wcsicmp(t.path.c_str(), path.c_str()) != 0)
            fwprintf_s(f, L"%s\n", t.path.c_str());
    }
    fclose(f);
    for (auto& t : favs) if (t.icon) DestroyIcon(t.icon);
}

#define WM_FAVICON_DOWNLOADED (WM_APP + 2)

bool GetFaviconCachePath(const std::wstring& url, WCHAR* outPath, int outPathLen)
{
    WCHAR host[256] = { 0 };
    URL_COMPONENTSW uc = { 0 };
    uc.dwStructSize = sizeof(uc);
    uc.lpszHostName = host;
    uc.dwHostNameLength = 255;
    if (!InternetCrackUrlW(url.c_str(), 0, 0, &uc)) return false;
    if (uc.nScheme != INTERNET_SCHEME_HTTP && uc.nScheme != INTERNET_SCHEME_HTTPS) return false;

    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    WCHAR cacheDir[MAX_PATH];
    swprintf_s(cacheDir, L"%s\\icons\\favicons", exePath);
    CreateDirectoryW(cacheDir, nullptr);
    swprintf_s(outPath, outPathLen, L"%s\\%s.ico", cacheDir, host);
    return true;
}

HICON LoadCachedFavicon(const std::wstring& url)
{
    WCHAR icoPath[MAX_PATH];
    if (!GetFaviconCachePath(url, icoPath, MAX_PATH)) return nullptr;
    if (GetFileAttributesW(icoPath) == INVALID_FILE_ATTRIBUTES) return nullptr;
    return (HICON)LoadImageW(nullptr, icoPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
}

HICON LoadCachedIconByPath(const WCHAR* icoPath)
{
    if (GetFileAttributesW(icoPath) == INVALID_FILE_ATTRIBUTES) return nullptr;
    return (HICON)LoadImageW(nullptr, icoPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
}

void GetIconCacheDir(WCHAR* outDir, int outDirLen)
{
    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    swprintf_s(outDir, outDirLen, L"%s\\icons\\favicons", exePath);
    CreateDirectoryW(outDir, nullptr);
}

static HICON g_edgeIcon = nullptr;
HICON GetDefaultUrlIcon()
{
    if (g_edgeIcon) return g_edgeIcon;
    WCHAR edgePath[MAX_PATH] = { 0 };
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD sz = sizeof(edgePath);
        RegQueryValueExW(hKey, nullptr, nullptr, nullptr, (LPBYTE)edgePath, &sz);
        RegCloseKey(hKey);
    }
    if (edgePath[0] == 0)
    {
        DWORD len = GetEnvironmentVariableW(L"ProgramFiles(x86)", edgePath, MAX_PATH);
        if (len == 0) GetEnvironmentVariableW(L"ProgramFiles", edgePath, MAX_PATH);
        wcscat_s(edgePath, MAX_PATH, L"\\Microsoft\\Edge\\Application\\msedge.exe");
    }
    if (GetFileAttributesW(edgePath) != INVALID_FILE_ATTRIBUTES)
    {
        g_edgeIcon = ExtractIconW(hInst, edgePath, 0);
        if (g_edgeIcon == (HICON)1) g_edgeIcon = nullptr;
    }
    if (!g_edgeIcon) g_edgeIcon = LoadIconW(nullptr, IDI_APPLICATION);
    return g_edgeIcon;
}

void HashToName(const std::wstring& s, WCHAR* outName, int outNameLen)
{
    DWORD h = 5381;
    for (auto c : s) h = ((h << 5) + h) + c;
    swprintf_s(outName, outNameLen, L"%lu", h);
}

struct IconDownloadItem {
    std::wstring iconUrl;
    std::wstring cachePath;
};

struct IconDownloadParams {
    std::vector<IconDownloadItem> items;
    HWND hWnd;
};

DWORD WINAPI IconDownloadThread(LPVOID lpParam)
{
    IconDownloadParams* p = (IconDownloadParams*)lpParam;
    for (auto& item : p->items)
    {
        if (GetFileAttributesW(item.cachePath.c_str()) != INVALID_FILE_ATTRIBUTES) continue;
        HRESULT hr = URLDownloadToFileW(nullptr, item.iconUrl.c_str(), item.cachePath.c_str(), 0, nullptr);
        if (FAILED(hr) || GetFileAttributesW(item.cachePath.c_str()) == INVALID_FILE_ATTRIBUTES)
        {
            HANDLE hF = CreateFileW(item.cachePath.c_str(), GENERIC_WRITE, 0, nullptr,
                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hF != INVALID_HANDLE_VALUE) CloseHandle(hF);
        }
    }
    PostMessageW(p->hWnd, WM_FAVICON_DOWNLOADED, 0, 0);
    delete p;
    return 0;
}

void QueueIconDownload(std::vector<IconDownloadItem>& queue, const std::wstring& iconUrl, const std::wstring& cachePath)
{
    IconDownloadItem item;
    item.iconUrl = iconUrl;
    item.cachePath = cachePath;
    queue.push_back(item);
}

void QueueFaviconDownload(std::vector<IconDownloadItem>& queue, const std::wstring& pageUrl)
{
    WCHAR host[256] = { 0 };
    URL_COMPONENTSW uc = { 0 };
    uc.dwStructSize = sizeof(uc);
    uc.lpszHostName = host;
    uc.dwHostNameLength = 255;
    if (!InternetCrackUrlW(pageUrl.c_str(), 0, 0, &uc)) return;
    if (uc.nScheme != INTERNET_SCHEME_HTTP && uc.nScheme != INTERNET_SCHEME_HTTPS) return;
    WCHAR cacheDir[MAX_PATH], icoPath[MAX_PATH], favUrl[512];
    GetIconCacheDir(cacheDir, MAX_PATH);
    swprintf_s(icoPath, L"%s\\%s.ico", cacheDir, host);
    swprintf_s(favUrl, L"https://%s/favicon.ico", host);
    QueueIconDownload(queue, favUrl, icoPath);
}

std::wstring GetIconCachePathForUrl(const std::wstring& iconUrl)
{
    WCHAR cacheDir[MAX_PATH], name[32], path[MAX_PATH];
    GetIconCacheDir(cacheDir, MAX_PATH);
    HashToName(iconUrl, name, 32);
    swprintf_s(path, L"%s\\%s.ico", cacheDir, name);
    return path;
}

bool ParseCsurlFile(const WCHAR* filePath, std::wstring& outUrl, std::wstring& outIcon)
{
    HANDLE hF = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hF == INVALID_HANDLE_VALUE) return false;
    DWORD size = GetFileSize(hF, nullptr);
    if (size == 0 || size > 8192) { CloseHandle(hF); return false; }
    char* raw = new char[size + 1];
    DWORD read = 0;
    ReadFile(hF, raw, size, &read, nullptr);
    raw[read] = 0;
    CloseHandle(hF);
    int start = 0;
    if (read >= 3 && (BYTE)raw[0] == 0xEF && (BYTE)raw[1] == 0xBB && (BYTE)raw[2] == 0xBF) start = 3;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, raw + start, -1, nullptr, 0);
    if (wlen <= 0) { delete[] raw; return false; }
    WCHAR* wbuf = new WCHAR[wlen];
    MultiByteToWideChar(CP_UTF8, 0, raw + start, -1, wbuf, wlen);
    delete[] raw;
    std::wstring json(wbuf);
    delete[] wbuf;

    auto extractField = [&](const std::wstring& field) -> std::wstring {
        std::wstring pat = L"\"" + field + L"\"";
        size_t pos = json.find(pat);
        if (pos == std::wstring::npos) return L"";
        pos = json.find(L':', pos);
        if (pos == std::wstring::npos) return L"";
        pos = json.find(L'"', pos);
        if (pos == std::wstring::npos) return L"";
        size_t end = pos + 1;
        while (end < json.size() && json[end] != L'"') end++;
        if (end >= json.size()) return L"";
        return json.substr(pos + 1, end - pos - 1);
    };

    outUrl = extractField(L"url");
    outIcon = extractField(L"icon");
    return !outUrl.empty();
}

bool TryGetBatUrl(const WCHAR* batPath, std::wstring& outUrl)
{
    HANDLE hF = CreateFileW(batPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hF == INVALID_HANDLE_VALUE) return false;
    DWORD size = GetFileSize(hF, nullptr);
    if (size == 0 || size > 4096) { CloseHandle(hF); return false; }
    char* raw = new char[size + 1];
    DWORD read = 0;
    ReadFile(hF, raw, size, &read, nullptr);
    raw[read] = 0;
    CloseHandle(hF);
    int start = 0;
    if (read >= 3 && (BYTE)raw[0] == 0xEF && (BYTE)raw[1] == 0xBB && (BYTE)raw[2] == 0xBF) start = 3;
    int wlen = MultiByteToWideChar(CP_ACP, 0, raw + start, -1, nullptr, 0);
    if (wlen <= 0) { delete[] raw; return false; }
    WCHAR* wbuf = new WCHAR[wlen];
    MultiByteToWideChar(CP_ACP, 0, raw + start, -1, wbuf, wlen);
    delete[] raw;

    WCHAR* p = wbuf;
    while (*p)
    {
        WCHAR* nl = wcschr(p, L'\n');
        if (nl) *nl = 0;
        size_t len = wcslen(p);
        while (len > 0 && (p[len-1] == L'\r' || p[len-1] == L' ' || p[len-1] == L'\t')) p[--len] = 0;
        if (len > 0)
        {
            WCHAR* sp = wcschr(p, L' ');
            if (sp)
            {
                std::wstring cmd(p, sp - p);
                for (auto& c : cmd) c = towlower(c);
                if (cmd == L"start")
                {
                    WCHAR* arg = sp + 1;
                    while (*arg == L' ' || *arg == L'\t') arg++;
                    if (wcsstr(arg, L"http://") == arg || wcsstr(arg, L"https://") == arg)
                    {
                        outUrl = arg;
                        delete[] wbuf;
                        return true;
                    }
                }
            }
        }
        if (!nl) break;
        p = nl + 1;
    }
    delete[] wbuf;
    return false;
}

void LoadToolsForMenu(int menuId)
{
    for (auto& t : g_tools) if (t.icon) DestroyIcon(t.icon);
    g_tools.clear();
    g_selectedTool = -1;
    g_hoverTool = -1;
    g_toolScroll = 0;

    const WCHAR* menuName = L"";
    for (int i = 0; i < g_menuCount; i++)
    {
        if (g_menus[i].id == menuId) { menuName = g_menus[i].name; break; }
    }

    if (menuId == IDC_MENU_FAVORITE)
    {
        LoadFavorites(g_tools);
        return;
    }

    if (menuId == IDC_MENU_SYSTEM)
    {
        WCHAR sysDir[MAX_PATH]; GetSystemDirectoryW(sysDir, MAX_PATH);
        WCHAR winDir[MAX_PATH]; GetWindowsDirectoryW(winDir, MAX_PATH);
        WCHAR psDir[MAX_PATH]; swprintf_s(psDir, L"%s\\WindowsPowerShell\\v1.0", sysDir);

        struct SysTool { const WCHAR* name; const WCHAR* dir; const WCHAR* exe; const WCHAR* params; };
        SysTool sysTools[] = {
            { L"此电脑",         winDir,  L"explorer.exe",    L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}" },
            { L"注册表编辑器",   winDir,  L"regedit.exe",     L"" },
            { L"控制面板",       sysDir,  L"control.exe",     L"" },
            { L"文档",           winDir,  L"explorer.exe",    L"shell:UsersFilesFolder\\Documents" },
            { L"计算机管理",     sysDir,  L"mmc.exe",         L"compmgmt.msc" },
            { L"设备管理器",     sysDir,  L"mmc.exe",         L"devmgmt.msc" },
            { L"磁盘管理",       sysDir,  L"mmc.exe",         L"diskmgmt.msc" },
            { L"服务",           sysDir,  L"mmc.exe",         L"services.msc" },
            { L"事件查看器",     sysDir,  L"mmc.exe",         L"eventvwr.msc" },
            { L"本地组策略",     sysDir,  L"mmc.exe",         L"gpedit.msc" },
            { L"本地安全策略",   sysDir,  L"mmc.exe",         L"secpol.msc" },
            { L"性能监视器",     sysDir,  L"mmc.exe",         L"perfmon.msc" },
            { L"用户和组",       sysDir,  L"mmc.exe",         L"lusrmgr.msc" },
            { L"共享文件夹",     sysDir,  L"mmc.exe",         L"fsmgmt.msc" },
            { L"证书管理",       sysDir,  L"mmc.exe",         L"certmgr.msc" },
            { L"组件服务",       sysDir,  L"dcomcnfg.exe",    L"" },
            { L"任务管理器",     sysDir,  L"taskmgr.exe",     L"" },
            { L"系统配置",       sysDir,  L"msconfig.exe",    L"" },
            { L"系统信息",       sysDir,  L"msinfo32.exe",    L"" },
            { L"资源监视器",     sysDir,  L"resmon.exe",      L"" },
            { L"DirectX诊断",    sysDir,  L"dxdiag.exe",      L"" },
            { L"远程桌面",       sysDir,  L"mstsc.exe",       L"" },
            { L"音量混合器",     sysDir,  L"sndvol.exe",      L"" },
            { L"计算器",         sysDir,  L"calc.exe",        L"" },
            { L"记事本",         sysDir,  L"notepad.exe",     L"" },
            { L"画图",           sysDir,  L"mspaint.exe",     L"" },
            { L"写字板",         sysDir,  L"write.exe",       L"" },
            { L"字符映射表",     sysDir,  L"charmap.exe",     L"" },
            { L"命令提示符",     sysDir,  L"cmd.exe",         L"" },
            { L"PowerShell",     psDir,   L"powershell.exe",  L"" },
            { L"磁盘清理",       sysDir,  L"cleanmgr.exe",    L"" },
            { L"磁盘碎片整理",   sysDir,  L"dfrgui.exe",      L"" },
            { L"磁盘分区工具",   sysDir,  L"diskpart.exe",    L"" },
            { L"系统文件检查",   sysDir,  L"sfc.exe",         L"/scannow" },
            { L"系统映像修复",   sysDir,  L"dism.exe",        L"" },
            { L"关于Windows",    sysDir,  L"winver.exe",      L"" },
            { L"放大镜",         sysDir,  L"magnify.exe",     L"" },
            { L"屏幕键盘",       sysDir,  L"osk.exe",         L"" },
            { L"讲述人",         sysDir,  L"narrator.exe",    L"" },
            { L"任务计划程序",   sysDir,  L"mmc.exe",         L"taskschd.msc" },
            { L"WMI管理",        sysDir,  L"mmc.exe",         L"wmimgmt.msc" },
            { L"打印管理",       sysDir,  L"mmc.exe",         L"printmanagement.msc" },
            { L"防火墙",         sysDir,  L"mmc.exe",         L"wf.msc" },
            { L"系统还原",       sysDir,  L"rstrui.exe",      L"" },
            { L"备份和还原",     sysDir,  L"sdclt.exe",       L"" },
            { L"创建修复盘",     sysDir,  L"recoverydrive.exe", L"" },
            { L"可靠性监视器",   sysDir,  L"perfmon.exe",     L"/rel" },
            { L"截图工具",       sysDir,  L"SnippingTool.exe", L"" },
            { L"便签",           sysDir,  L"StikyNot.exe",    L"" },
            { L"问题记录器",     sysDir,  L"psr.exe",         L"" },
            { L"ODBC数据源",     sysDir,  L"odbcad32.exe",    L"" },
            { L"IExpress",       sysDir,  L"iexpress.exe",    L"" },
            { L"电话拨号",       sysDir,  L"dialer.exe",      L"" },
            { L"系统属性",       sysDir,  L"control.exe",     L"sysdm.cpl" },
            { L"显示属性",       sysDir,  L"control.exe",     L"desk.cpl" },
            { L"鼠标属性",       sysDir,  L"control.exe",     L"main.cpl" },
            { L"日期和时间",     sysDir,  L"control.exe",     L"timedate.cpl" },
            { L"区域和语言",     sysDir,  L"control.exe",     L"intl.cpl" },
            { L"电源选项",       sysDir,  L"control.exe",     L"powercfg.cpl" },
            { L"声音",           sysDir,  L"control.exe",     L"mmsys.cpl" },
            { L"网络连接",       sysDir,  L"control.exe",     L"ncpa.cpl" },
            { L"用户账户",       sysDir,  L"control.exe",     L"nusrmgr.cpl" },
            { L"程序和功能",     sysDir,  L"control.exe",     L"appwiz.cpl" },
            { L"字体",           sysDir,  L"control.exe",     L"fonts" },
            { L"管理工具",       sysDir,  L"control.exe",     L"admintools" },
            { L"存储空间",       sysDir,  L"StorageSpaces.exe", L"" },
            { L"文件签名验证",   sysDir,  L"sigverif.exe",    L"" },
            { L"内存诊断",       sysDir,  L"mdsched.exe",     L"" },
            { L"步骤记录器",     sysDir,  L"psr.exe",         L"/start" },
            { L"Windows更新",    sysDir,  L"control.exe",     L"wuaucpl.cpl" },
            { L"默认程序",       sysDir,  L"control.exe",     L"control.exe /name Microsoft.DefaultPrograms" },
            { L"自动播放",       sysDir,  L"control.exe",     L"control.exe /name Microsoft.AutoPlay" },
            { L"通知区域",       sysDir,  L"control.exe",     L"control.exe /name Microsoft.NotificationArea" },
            { L"同步中心",       sysDir,  L"mobsync.exe",     L"" },
            { L"系统准备工具",   sysDir,  L"sysprep.exe",     L"" },
            { L"BCDEdit",        sysDir,  L"bcdedit.exe",     L"" },
            { L"IP配置",         sysDir,  L"ipconfig.exe",    L"" },
            { L"网络统计",       sysDir,  L"netstat.exe",     L"" },
            { L"Ping",           sysDir,  L"ping.exe",        L"" },
            { L"路由跟踪",       sysDir,  L"tracert.exe",     L"" },
            { L"系统信息命令",   sysDir,  L"systeminfo.exe",  L"" },
            { L"任务列表",       sysDir,  L"tasklist.exe",    L"" },
            { L"驱动查询",       sysDir,  L"driverquery.exe", L"" },
            { L"Whoami",         sysDir,  L"whoami.exe",      L"" },
            { L"主机名",         sysDir,  L"hostname.exe",    L"" },
            { L"关机",           sysDir,  L"shutdown.exe",    L"/s /t 0" },
            { L"重启",           sysDir,  L"shutdown.exe",    L"/r /t 0" },
            { L"注销",           sysDir,  L"shutdown.exe",    L"/l /t 0" },
            { L"辅助功能",       sysDir,  L"control.exe",     L"access.cpl" },
            { L"Internet选项",   sysDir,  L"control.exe",     L"inetcpl.cpl" },
            { L"游戏控制器",     sysDir,  L"control.exe",     L"joy.cpl" },
            { L"安全和维护",     sysDir,  L"control.exe",     L"wscui.cpl" },
            { L"防火墙设置",     sysDir,  L"control.exe",     L"firewall.cpl" },
            { L"电话和调制解调器", sysDir, L"control.exe",    L"telephon.cpl" },
            { L"颜色管理",       sysDir,  L"colorcpl.exe",    L"" },
            { L"颜色校准",       sysDir,  L"dccw.exe",        L"" },
            { L"ClearType",      sysDir,  L"cttune.exe",      L"" },
            { L"蓝牙",           sysDir,  L"fsquirt.exe",     L"" },
            { L"语音识别",       sysDir,  L"sapisvr.exe",     L"" },
            { L"设置中心",       sysDir,  L"utilman.exe",     L"" },
            { L"iSCSI发起程序",  sysDir,  L"iscsicpl.exe",    L"" },
            { L"Windows功能",    sysDir,  L"optionalfeatures.exe", L"" },
            { L"添加硬件",       sysDir,  L"hdwwiz.exe",      L"" },
            { L"WMI测试",        sysDir,  L"wbemtest.exe",    L"" },
            { L"组策略结果",     sysDir,  L"gpresult.exe",    L"/z" },
            { L"组策略更新",     sysDir,  L"gpupdate.exe",    L"/force" },
            { L"证书工具",       sysDir,  L"certutil.exe",    L"" },
            { L"ACL权限",        sysDir,  L"icacls.exe",      L"" },
            { L"加密",           sysDir,  L"cipher.exe",      L"" },
            { L"压缩",           sysDir,  L"compact.exe",     L"" },
            { L"取得所有权",     sysDir,  L"takeown.exe",     L"" },
            { L"Robocopy",       sysDir,  L"robocopy.exe",    L"" },
            { L"Xcopy",          sysDir,  L"xcopy.exe",       L"" },
            { L"DNS查找",        sysDir,  L"nslookup.exe",    L"" },
            { L"NetBIOS统计",    sysDir,  L"nbtstat.exe",     L"" },
            { L"路径Ping",       sysDir,  L"pathping.exe",    L"" },
            { L"路由",           sysDir,  L"route.exe",       L"" },
            { L"网络Shell",      sysDir,  L"netsh.exe",       L"" },
            { L"MAC地址",        sysDir,  L"getmac.exe",      L"" },
            { L"卷影副本",       sysDir,  L"vssadmin.exe",    L"" },
            { L"时间服务",       sysDir,  L"w32tm.exe",       L"" },
            { L"时区工具",       sysDir,  L"tzutil.exe",      L"" },
            { L"计划任务",       sysDir,  L"schtasks.exe",    L"" },
            { L"注册DLL",        sysDir,  L"regsvr32.exe",    L"" },
            { L"运行身份",       sysDir,  L"runas.exe",       L"" },
            { L"凭据管理",       sysDir,  L"cmdkey.exe",      L"" },
            { L"Windows安装",    sysDir,  L"msiexec.exe",     L"" },
            { L"脚本主机",       sysDir,  L"cscript.exe",     L"" },
            { L"WS脚本",         sysDir,  L"wscript.exe",     L"" },
            { L"WMI命令",        sysDir,  L"wmic.exe",        L"" },
            { L"事件工具",       sysDir,  L"wevtutil.exe",    L"" },
            { L"性能计数器",     sysDir,  L"typeperf.exe",    L"" },
            { L"日志管理",       sysDir,  L"logman.exe",      L"" },
            { L"审核策略",       sysDir,  L"auditpol.exe",    L"" },
            { L"安全编辑",       sysDir,  L"secedit.exe",     L"" },
            { L"BITS管理",       sysDir,  L"bitsadmin.exe",   L"" },
            { L"文件比较",       sysDir,  L"fc.exe",          L"" },
            { L"查找字符串",     sysDir,  L"findstr.exe",     L"" },
            { L"排序",           sysDir,  L"sort.exe",        L"" },
            { L"树形目录",       sysDir,  L"tree.com",        L"" },
            { L"FTP",            sysDir,  L"ftp.exe",         L"" },
            { L"Telnet",         sysDir,  L"telnet.exe",      L"" },
            { L"Defrag",         sysDir,  L"defrag.exe",      L"" },
            { L"检查磁盘",       sysDir,  L"chkntfs.exe",     L"" },
            { L"文件系统工具",   sysDir,  L"fsutil.exe",      L"" },
            { L"挂载卷",         sysDir,  L"mountvol.exe",    L"" },
            { L"打开文件",       sysDir,  L"openfiles.exe",   L"" },
            { L"DVD播放",        sysDir,  L"dvdplay.exe",     L"" },
            { L"Windows播放器",  sysDir,  L"wmplayer.exe",    L"" },
            { L"同步",           sysDir,  L"mobsync.exe",     L"" },
            { L"注销命令",       sysDir,  L"logoff.exe",      L"" },
        };
        for (auto& st : sysTools)
        {
            WCHAR fullPath[MAX_PATH];
            swprintf_s(fullPath, L"%s\\%s", st.dir, st.exe);
            if (GetFileAttributesW(fullPath) == INVALID_FILE_ATTRIBUTES) continue;

            HICON hIcon = nullptr;
            SHFILEINFOW sfi = { 0 };
            if (SHGetFileInfoW(fullPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON))
                hIcon = sfi.hIcon;
            if (!hIcon) hIcon = ExtractIconW(hInst, fullPath, 0);
            if (hIcon == (HICON)1) hIcon = nullptr;

            ToolItem ti;
            ti.name = st.name;
            ti.path = fullPath;
            ti.params = st.params;
            ti.icon = hIcon;
            g_tools.push_back(ti);
        }
        return;
    }



    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);

    WCHAR dataDir[MAX_PATH];
    swprintf_s(dataDir, L"%s\\data\\%s", exePath, menuName);

    std::vector<IconDownloadItem> pendingDownloads;
    const WCHAR* exts[] = { L"*.exe", L"*.lnk", L"*.csurl" };
    for (int ei = 0; ei < (int)ARRAYSIZE(exts); ei++)
    {
        WCHAR search[MAX_PATH];
        swprintf_s(search, L"%s\\%s", dataDir, exts[ei]);

        WIN32_FIND_DATAW fd;
        HANDLE hFind = FindFirstFileW(search, &fd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                WCHAR fullPath[MAX_PATH];
                swprintf_s(fullPath, L"%s\\%s", dataDir, fd.cFileName);

                HICON hIcon = nullptr;
                std::wstring openUrl;
                const WCHAR* ext = PathFindExtensionW(fullPath);
                if (ext && _wcsicmp(ext, L".bat") == 0)
                {
                    std::wstring batUrl;
                    if (TryGetBatUrl(fullPath, batUrl))
                    {
                        openUrl = batUrl;
                        hIcon = LoadCachedFavicon(batUrl);
                        if (!hIcon)
                        {
                            WCHAR favCache[MAX_PATH];
                            if (GetFaviconCachePath(batUrl, favCache, MAX_PATH) &&
                                GetFileAttributesW(favCache) == INVALID_FILE_ATTRIBUTES)
                                QueueFaviconDownload(pendingDownloads, batUrl);
                        }
                    }
                }
                else if (ext && _wcsicmp(ext, L".csurl") == 0)
                {
                    std::wstring csUrl, csIcon;
                    if (ParseCsurlFile(fullPath, csUrl, csIcon))
                    {
                        openUrl = csUrl;
                        if (!csIcon.empty())
                        {
                            std::wstring cachePath = GetIconCachePathForUrl(csIcon);
                            hIcon = LoadCachedIconByPath(cachePath.c_str());
                            if (!hIcon && GetFileAttributesW(cachePath.c_str()) == INVALID_FILE_ATTRIBUTES)
                                QueueIconDownload(pendingDownloads, csIcon, cachePath);
                        }
                        else
                        {
                            hIcon = LoadCachedFavicon(csUrl);
                            if (!hIcon)
                            {
                                WCHAR favCache[MAX_PATH];
                                if (GetFaviconCachePath(csUrl, favCache, MAX_PATH) &&
                                    GetFileAttributesW(favCache) == INVALID_FILE_ATTRIBUTES)
                                    QueueFaviconDownload(pendingDownloads, csUrl);
                            }
                        }
                    }
                }
                if (!hIcon && !openUrl.empty())
                    hIcon = CopyIcon(GetDefaultUrlIcon());
                if (!hIcon)
                {
                    SHFILEINFOW sfi = { 0 };
                    if (SHGetFileInfoW(fullPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON))
                        hIcon = sfi.hIcon;
                    if (!hIcon) hIcon = ExtractIconW(hInst, fullPath, 0);
                    if (hIcon == (HICON)1) hIcon = nullptr;
                }

                WCHAR name[MAX_PATH];
                wcscpy_s(name, fd.cFileName);
                PathRemoveExtensionW(name);

                ToolItem ti;
                ti.name = name;
                ti.path = openUrl.empty() ? fullPath : openUrl;
                ti.icon = hIcon;
                g_tools.push_back(ti);
            } while (FindNextFileW(hFind, &fd));
            FindClose(hFind);
        }
    }

    if (!pendingDownloads.empty() && g_hWndMain)
    {
        IconDownloadParams* p = new IconDownloadParams;
        p->items = std::move(pendingDownloads);
        p->hWnd = g_hWndMain;
        CreateThread(nullptr, 0, IconDownloadThread, p, 0, nullptr);
    }


    WCHAR subSearch[MAX_PATH];
    swprintf_s(subSearch, L"%s\\*", dataDir);
    WIN32_FIND_DATAW fd;
    HANDLE hSub = FindFirstFileW(subSearch, &fd);
    if (hSub != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
            if (fd.cFileName[0] == L'.') continue;

            WCHAR subDir[MAX_PATH];
            swprintf_s(subDir, L"%s\\%s", dataDir, fd.cFileName);

            WCHAR bestPath[MAX_PATH] = { 0 };
            std::wstring displayName;

            WCHAR readmePath[MAX_PATH];
            swprintf_s(readmePath, L"%s\\readme.txt", subDir);
            HANDLE hRf = CreateFileW(readmePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hRf != INVALID_HANDLE_VALUE)
            {
                DWORD fileSize = GetFileSize(hRf, nullptr);
                if (fileSize > 0 && fileSize < 4096)
                {
                    char* raw = new char[fileSize + 1];
                    DWORD bytesRead = 0;
                    ReadFile(hRf, raw, fileSize, &bytesRead, nullptr);
                    raw[bytesRead] = 0;
                    int start = 0;
                    if (bytesRead >= 3 && (BYTE)raw[0] == 0xEF && (BYTE)raw[1] == 0xBB && (BYTE)raw[2] == 0xBF) start = 3;
                    int wlen = MultiByteToWideChar(CP_UTF8, 0, raw + start, -1, nullptr, 0);
                    if (wlen > 0)
                    {
                        WCHAR* wbuf = new WCHAR[wlen];
                        MultiByteToWideChar(CP_UTF8, 0, raw + start, -1, wbuf, wlen);
                        int lineNum = 0;
                        WCHAR* p = wbuf;
                        while (*p)
                        {
                            WCHAR* nl = wcschr(p, L'\n');
                            if (nl) *nl = 0;
                            size_t len = wcslen(p);
                            while (len > 0 && (p[len-1] == L'\r' || p[len-1] == L' ' || p[len-1] == L'\t')) p[--len] = 0;
                            if (len > 0)
                            {
                                lineNum++;
                                if (lineNum == 1)
                                {
                                    WCHAR exeFullPath[MAX_PATH];
                                    swprintf_s(exeFullPath, L"%s\\%s", subDir, p);
                                    if (GetFileAttributesW(exeFullPath) != INVALID_FILE_ATTRIBUTES)
                                        wcscpy_s(bestPath, exeFullPath);
                                }
                                else if (lineNum == 2)
                                {
                                    displayName = p;
                                }
                            }
                            if (!nl) break;
                            p = nl + 1;
                        }
                        delete[] wbuf;
                    }
                    delete[] raw;
                }
                CloseHandle(hRf);
            }

            if (bestPath[0] == 0)
            {
                WCHAR exeSearch[MAX_PATH];
                swprintf_s(exeSearch, L"%s\\*.exe", subDir);
                WIN32_FIND_DATAW fd2;
                HANDLE hExe = FindFirstFileW(exeSearch, &fd2);
                if (hExe != INVALID_HANDLE_VALUE)
                {
                    WCHAR firstPath[MAX_PATH] = { 0 };
                    do
                    {
                        if (fd2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                        WCHAR exeFullPath[MAX_PATH];
                        swprintf_s(exeFullPath, L"%s\\%s", subDir, fd2.cFileName);
                        if (firstPath[0] == 0) wcscpy_s(firstPath, exeFullPath);

                        WCHAR exeNameNoExt[MAX_PATH];
                        wcscpy_s(exeNameNoExt, fd2.cFileName);
                        PathRemoveExtensionW(exeNameNoExt);
                        if (_wcsicmp(exeNameNoExt, fd.cFileName) == 0)
                        {
                            wcscpy_s(bestPath, exeFullPath);
                            break;
                        }
                        if (bestPath[0] == 0) wcscpy_s(bestPath, exeFullPath);
                    } while (FindNextFileW(hExe, &fd2));
                    FindClose(hExe);
                    if (bestPath[0] == 0) wcscpy_s(bestPath, firstPath);
                }
            }

            if (bestPath[0] == 0) continue;

            HICON hIcon = ExtractIconW(hInst, bestPath, 0);
            if (hIcon == (HICON)1) hIcon = nullptr;

            ToolItem ti;
            ti.name = displayName.empty() ? fd.cFileName : displayName;

            ti.path = bestPath;
            ti.icon = hIcon;
            ti.isSubFolder = true;
            g_tools.push_back(ti);
        } while (FindNextFileW(hSub, &fd));
        FindClose(hSub);
    }
}

void ShowCurrentPage()
{
    if (g_selectedMenu == IDC_MENU_SYSINFO)
    {
        if (!g_siCardsBuilt) BuildSysInfoCards();
        for (auto& t : g_tools) if (t.icon) DestroyIcon(t.icon);
        g_tools.clear();
        g_selectedTool = -1;
        g_hoverTool = -1;
        ShowWindow(g_hStcPlaceholder, SW_HIDE);
        InvalidateRect(g_hWndMain, nullptr, FALSE);
        return;
    }
    LoadToolsForMenu(g_selectedMenu);
    BOOL hasTools = !g_tools.empty();
    if (!hasTools)
    {
        SetWindowTextW(g_hStcPlaceholder, L"该分类下暂无工具");
        ShowWindow(g_hStcPlaceholder, SW_SHOW);
    }
    else
    {
        ShowWindow(g_hStcPlaceholder, SW_HIDE);
    }
    InvalidateRect(g_hWndMain, nullptr, TRUE);
}

int HitTestMenu(int x, int y)
{
    if (x >= SIDEBAR_WIDTH || y < MENU_TOP_OFFSET)
        return 0;
    int idx = (y - MENU_TOP_OFFSET) / MENU_ITEM_HEIGHT;
    if (idx < 0 || idx >= g_menuCount) return 0;
    return g_menus[idx].id;
}

void DrawMenuItem(HDC hdc, int index, const RECT& rcItem)
{
    const MenuItem& mi = g_menus[index];
    BOOL selected = (mi.id == g_selectedMenu);
    BOOL hover    = (mi.id == g_hoverMenu);

    if (selected)
    {
        FillRect(hdc, &rcItem, g_brMenuSel);
        RECT rcAccent = { 0, rcItem.top, 4, rcItem.bottom };
        FillRect(hdc, &rcAccent, g_brAccent);
    }
    else if (hover)
    {
        FillRect(hdc, &rcItem, g_brMenuHover);
    }

    if (mi.icon)
    {
        int ix = 8;
        int iy = rcItem.top + (MENU_ITEM_HEIGHT - MENU_ICON_SIZE) / 2;
        DrawIconEx(hdc, ix, iy, mi.icon, MENU_ICON_SIZE, MENU_ICON_SIZE, 0, nullptr, DI_NORMAL);
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, selected ? CLR_TEXT : CLR_TEXT_DIM);
    HFONT old = (HFONT)SelectObject(hdc, g_hFontMenu);
    RECT rcText = rcItem;
    rcText.left += 34;
    DrawTextW(hdc, mi.name, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(hdc, old);
}

void DrawSideBar(HDC hdc, int cx, int cy)
{
    int sideH = cy - STATUSBAR_H;
    RECT rcSide = { 0, 0, SIDEBAR_WIDTH, sideH };
    FillRect(hdc, &rcSide, g_brSidebar);

    int top = MENU_TOP_OFFSET;
    for (int i = 0; i < g_menuCount; i++)
    {
        RECT rcItem = { 0, top + i * MENU_ITEM_HEIGHT, SIDEBAR_WIDTH, top + (i + 1) * MENU_ITEM_HEIGHT };
        DrawMenuItem(hdc, i, rcItem);
    }

    HPEN oldPen = (HPEN)SelectObject(hdc, g_penLine);
    MoveToEx(hdc, SIDEBAR_WIDTH - 1, 0, nullptr);
    LineTo(hdc, SIDEBAR_WIDTH - 1, sideH);
    SelectObject(hdc, oldPen);
}

void DrawToolName(HDC hdc, const std::wstring& name, RECT& rcText)
{
    int maxW = rcText.right - rcText.left;
    int maxH = rcText.bottom - rcText.top;
    HFONT oldFont = (HFONT)SelectObject(hdc, g_hFontUI);
    int fontH = 0;
    TEXTMETRICW tm;
    if (GetTextMetricsW(hdc, &tm)) fontH = tm.tmHeight + tm.tmExternalLeading;
    SelectObject(hdc, oldFont);
    if (fontH <= 0) fontH = 16;

    auto measureW = [&](const std::wstring& s) -> int {
        SIZE sz = { 0 };
        GetTextExtentPoint32W(hdc, s.c_str(), (int)s.size(), &sz);
        return sz.cx;
    };

    if (measureW(name) <= maxW)
    {
        DrawTextW(hdc, name.c_str(), -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }

    std::wstring line1, line2;
    for (size_t i = 0; i < name.size(); i++)
    {
        std::wstring test = line1 + name[i];
        if (measureW(test) > maxW) break;
        line1 = test;
    }
    line2 = name.substr(line1.size());

    if (line1.empty()) line1 = std::wstring(1, name[0]);

    if (measureW(line2) <= maxW && fontH * 2 <= maxH)
    {
        std::wstring two = line1 + L"\n" + line2;
        DrawTextW(hdc, two.c_str(), -1, &rcText, DT_CENTER | DT_WORDBREAK);
        return;
    }

    while (!line2.empty() && measureW(line2 + L"…") > maxW)
        line2.pop_back();
    if (line2.empty()) line2 = L"…";
    else line2 += L"…";

    if (fontH * 2 <= maxH)
    {
        std::wstring two = line1 + L"\n" + line2;
        DrawTextW(hdc, two.c_str(), -1, &rcText, DT_CENTER | DT_WORDBREAK);
    }
    else
    {
        while (!line1.empty() && measureW(line1 + L"…") > maxW)
            line1.pop_back();
        if (line1.empty()) line1 = L"…";
        else line1 += L"…";
        DrawTextW(hdc, line1.c_str(), -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void DrawToolGrid(HDC hdc, int cx, int cy)
{
    int contentX = SIDEBAR_WIDTH;
    int contentW = cx - SIDEBAR_WIDTH;
    int scrollBarW = 6;
    int scrollBarX = cx - scrollBarW - 4;

    int usableW = contentW - MARGIN * 2 - 10;
    int cols = usableW / ITEM_W;
    if (cols < 1) cols = 1;
    int rows = (int)((g_tools.size() + cols - 1) / cols);
    int totalH = GRID_TOP + rows * ITEM_H + 20;
    g_toolScrollMax = max(0, totalH - cy);
    if (g_toolScroll > g_toolScrollMax) g_toolScroll = g_toolScrollMax;
    if (g_toolScroll < 0) g_toolScroll = 0;

    HRGN clipRgn = CreateRectRgn(SIDEBAR_WIDTH, 0, scrollBarX, cy);
    SelectClipRgn(hdc, clipRgn);
    DeleteObject(clipRgn);

    HFONT old = (HFONT)SelectObject(hdc, g_hFontUI);
    SetBkMode(hdc, TRANSPARENT);

    for (size_t i = 0; i < g_tools.size(); i++)
    {
        RECT rcItem;
        CalcToolRect((int)i, contentX, contentW, rcItem);
        if (rcItem.bottom < 0 || rcItem.top > cy) continue;

        COLORREF bg = CLR_CONTENT;
        if ((int)i == g_selectedTool)      bg = CLR_MENU_SEL;
        else if ((int)i == g_hoverTool)    bg = CLR_MENU_HOVER;
        if (bg != CLR_CONTENT)
        {
            HBRUSH br = CreateSolidBrush(bg);
            FillRect(hdc, &rcItem, br);
            DeleteObject(br);
        }
        if (g_tools[i].icon)
        {
            int ix = rcItem.left + (ITEM_W - ICON_SIZE) / 2;
            int iy = rcItem.top + 8;
            DrawIconEx(hdc, ix, iy, g_tools[i].icon, ICON_SIZE, ICON_SIZE, 0, nullptr, DI_NORMAL);
        }
        RECT rcText = { rcItem.left + 2, rcItem.top + 8 + ICON_SIZE + 4, rcItem.right - 2, rcItem.bottom - 2 };
        SetTextColor(hdc, CLR_TEXT);
        DrawToolName(hdc, g_tools[i].name, rcText);
    }
    SelectObject(hdc, old);

    SelectClipRgn(hdc, NULL);

    if (g_toolScrollMax > 0)
    {
        int thumbH = max(30, cy * cy / totalH);
        int thumbY = (int)((double)g_toolScroll / g_toolScrollMax * (cy - thumbH));
        RECT rcTrack = { scrollBarX, 0, scrollBarX + scrollBarW, cy };
        FillRect(hdc, &rcTrack, g_brTrack);
        RECT rcThumb = { scrollBarX, thumbY, scrollBarX + scrollBarW, thumbY + thumbH };
        FillRect(hdc, &rcThumb, g_brThumb);
    }
}

void UpdateNetworkSpeed()
{
    DWORD dwSize = 0;
    GetIfTable(nullptr, &dwSize, FALSE);
    if (dwSize == 0) return;
    MIB_IFTABLE* table = (MIB_IFTABLE*)malloc(dwSize);
    if (!table) return;
    table->dwNumEntries = 0;
    if (GetIfTable(table, &dwSize, FALSE) != NO_ERROR) { free(table); return; }
    ULONG64 inBytes = 0, outBytes = 0;
    for (DWORD i = 0; i < table->dwNumEntries; i++)
    {
        if (table->table[i].dwOperStatus == IF_OPER_STATUS_OPERATIONAL)
        {
            inBytes += table->table[i].dwInOctets;
            outBytes += table->table[i].dwOutOctets;
        }
    }
    free(table);

    g_netDown = (inBytes > g_lastInBytes) ? (inBytes - g_lastInBytes) : 0;
    g_netUp   = (outBytes > g_lastOutBytes) ? (outBytes - g_lastOutBytes) : 0;
    g_lastInBytes = inBytes;
    g_lastOutBytes = outBytes;
}

void FormatSpeed(WCHAR* buf, size_t len, ULONG64 bps)
{
    if (bps < 1024) swprintf_s(buf, len, L"%llu B/s", bps);
    else if (bps < 1024 * 1024) swprintf_s(buf, len, L"%.1f KB/s", bps / 1024.0);
    else swprintf_s(buf, len, L"%.1f MB/s", bps / 1024.0 / 1024.0);
}

std::string ExtractJsonField(const std::string& json, const std::string& field)
{
    std::string key = "\"" + field + "\":\"";
    size_t p = json.find(key);
    if (p == std::string::npos) return "";
    p += key.size();
    size_t end = json.find('"', p);
    if (end == std::string::npos) return "";
    return json.substr(p, end - p);
}

DWORD WINAPI FetchExtIpThread(LPVOID)
{
    WCHAR tempFile[MAX_PATH];
    GetTempPathW(MAX_PATH, tempFile);
    wcscat_s(tempFile, MAX_PATH, L"caishen_ip.json");
    DeleteFileW(tempFile);

    HRESULT hr = URLDownloadToFileW(nullptr, L"http://ip-api.com/json/?lang=zh-CN", tempFile, 0, nullptr);
    if (FAILED(hr)) return 0;

    FILE* f = nullptr;
    _wfopen_s(&f, tempFile, L"rb");
    if (!f) return 0;
    char json[4096] = { 0 };
    fread(json, 1, 4095, f);
    fclose(f);
    DeleteFileW(tempFile);

    std::string ip = ExtractJsonField(json, "query");
    std::string country = ExtractJsonField(json, "country");
    std::string region = ExtractJsonField(json, "regionName");

    if (!ip.empty())
    {
        MultiByteToWideChar(CP_UTF8, 0, ip.c_str(), -1, g_extIP, 64);
        std::string area = country + region;
        MultiByteToWideChar(CP_UTF8, 0, area.c_str(), -1, g_extRegion, 128);
        g_ipFetched = TRUE;
        if (g_hWndMain) {
            RECT rc; GetClientRect(g_hWndMain, &rc);
            RECT rcStatus = { 0, rc.bottom - STATUSBAR_H, rc.right, rc.bottom };
            InvalidateRect(g_hWndMain, &rcStatus, FALSE);
        }
    }
    return 0;
}

void DrawStatusBar(HDC hdc, int cx, int cy)
{
    RECT rcBar = { 0, cy - STATUSBAR_H, cx, cy };
    FillRect(hdc, &rcBar, g_brStatus);

    HPEN oldPen = (HPEN)SelectObject(hdc, g_penLine);
    MoveToEx(hdc, 0, cy - STATUSBAR_H, nullptr);
    LineTo(hdc, cx, cy - STATUSBAR_H);
    SelectObject(hdc, oldPen);

    HFONT old = (HFONT)SelectObject(hdc, g_hFontStatus);
    SetBkMode(hdc, TRANSPARENT);

    int textY = cy - STATUSBAR_H + (STATUSBAR_H - 16) / 2;
    int x = 14;
    WCHAR buf[128];

    SetTextColor(hdc, CLR_TEXT);
    RECT rcVer = { x, textY, x + 90, textY + 16 };
    DrawTextW(hdc, L"V1.0.0.100", -1, &rcVer, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
    x += 100;

    SetTextColor(hdc, CLR_NET_UP);
    FormatSpeed(buf, 128, g_netUp);
    WCHAR upText[160]; swprintf_s(upText, L"↑ %s", buf);
    RECT rcUp = { x, textY, x + 130, textY + 16 };
    DrawTextW(hdc, upText, -1, &rcUp, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
    x += 130;

    SetTextColor(hdc, CLR_NET_DOWN);
    FormatSpeed(buf, 128, g_netDown);
    WCHAR downText[160]; swprintf_s(downText, L"↓ %s", buf);
    RECT rcDown = { x, textY, x + 130, textY + 16 };
    DrawTextW(hdc, downText, -1, &rcDown, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
    x += 140;

    SetTextColor(hdc, CLR_TEXT_DIM);
    WCHAR ipText[200];
    if (g_ipFetched && g_extIP[0])
        swprintf_s(ipText, L"IP: %s  %s", g_extIP, g_extRegion);
    else
        wcscpy_s(ipText, L"IP: 获取中...");
    RECT rcIp = { x, textY, x + 260, textY + 16 };
    DrawTextW(hdc, ipText, -1, &rcIp, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
    x += 270;

    ULONG64 sec = GetTickCount64() / 1000;
    ULONG64 d = sec / 86400, h = (sec % 86400) / 3600, m = (sec % 3600) / 60;
    WCHAR upTime[64]; swprintf_s(upTime, L"运行: %llu天%02llu时%02llu分", d, h, m);
    RECT rcUpT = { x, textY, x + 160, textY + 16 };
    DrawTextW(hdc, upTime, -1, &rcUpT, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
    x += 170;

    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ul;
    ul.LowPart = ft.dwLowDateTime;
    ul.HighPart = ft.dwHighDateTime;
    ul.QuadPart += (ULONGLONG)8 * 3600 * 10000000;
    ft.dwLowDateTime = ul.LowPart;
    ft.dwHighDateTime = ul.HighPart;
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    WCHAR timeText[64]; swprintf_s(timeText, L"%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    SetTextColor(hdc, CLR_TEXT);
    RECT rcTime = { x, textY, x + 170, textY + 16 };
    DrawTextW(hdc, timeText, -1, &rcTime, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

    SelectObject(hdc, old);
}

static void ReadRegString(HKEY root, const WCHAR* path, const WCHAR* value, WCHAR* buf, DWORD bufSize)
{
    buf[0] = 0;
    HKEY hKey;
    if (RegOpenKeyExW(root, path, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        RegQueryValueExW(hKey, value, nullptr, nullptr, (LPBYTE)buf, &bufSize);
        RegCloseKey(hKey);
    }
}


void BuildSysInfoCards()
{
    g_siCards.clear();
    WCHAR buf[512], buf2[512];

    {SysCard c; c.title = L"操作系统";
        ReadRegString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName", buf, sizeof(buf));
        if (buf[0] == 0) wcscpy_s(buf, L"Windows");
        c.items.push_back({L"系统名称", buf});
        ReadRegString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"DisplayVersion", buf, sizeof(buf));
        if (buf[0] == 0) ReadRegString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ReleaseId", buf, sizeof(buf));
        ReadRegString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"CurrentBuild", buf2, sizeof(buf2));
        swprintf_s(buf, L"%s (Build %s)", buf, buf2); c.items.push_back({L"版本号", buf});
        DWORD installDate = 0, sz = sizeof(installDate);
        HKEY hK;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ | KEY_WOW64_64KEY, &hK) == ERROR_SUCCESS)
        { RegQueryValueExW(hK, L"InstallDate", nullptr, nullptr, (LPBYTE)&installDate, &sz); RegCloseKey(hK); }
        time_t it = (time_t)installDate; struct tm tmb; localtime_s(&tmb, &it);
        WCHAR ds[64]; wcsftime(ds, 64, L"%Y-%m-%d %H:%M", &tmb); c.items.push_back({L"安装日期", ds});
        WCHAR sd[MAX_PATH]; GetSystemDirectoryW(sd, MAX_PATH); c.items.push_back({L"系统目录", sd});
        g_siCards.push_back(c);
    }
    {SysCard c; c.title = L"计算机";
        WCHAR cn[MAX_PATH]; DWORD cs = MAX_PATH; GetComputerNameW(cn, &cs); c.items.push_back({L"计算机名", cn});
        WCHAR un[MAX_PATH]; DWORD us = MAX_PATH; GetUserNameW(un, &us); c.items.push_back({L"当前用户", un});
        ULONG64 sec = GetTickCount64() / 1000;
        swprintf_s(buf, L"%llu天 %02llu时 %02llu分", sec/86400, (sec%86400)/3600, (sec%3600)/60);
        c.items.push_back({L"运行时间", buf});
        g_siCards.push_back(c);
    }
    {SysCard c; c.title = L"CPU";
        ReadRegString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString", buf, sizeof(buf));
        c.items.push_back({L"处理器", buf});
        SYSTEM_INFO si; GetSystemInfo(&si);
        DWORD cores = 0, logicals = si.dwNumberOfProcessors, szC = sizeof(cores); HKEY hK;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\kernel", 0, KEY_READ, &hK) == ERROR_SUCCESS)
        { RegQueryValueExW(hK, L"NumberOfPhysicalCores", nullptr, nullptr, (LPBYTE)&cores, &szC); RegCloseKey(hK); }
        if (cores == 0) cores = logicals;
        swprintf_s(buf, L"%u 核 %u 线程", cores, logicals); c.items.push_back({L"核心/线程", buf});
        DWORD mhz = 0, szM = sizeof(mhz);
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hK) == ERROR_SUCCESS)
        { RegQueryValueExW(hK, L"~MHz", nullptr, nullptr, (LPBYTE)&mhz, &szM); RegCloseKey(hK); }
        swprintf_s(buf, L"%.2f GHz", mhz / 1000.0); c.items.push_back({L"最大频率", buf});
        g_siCards.push_back(c);
    }
    {SysCard c; c.title = L"内存";
        MEMORYSTATUSEX ms = { sizeof(ms) }; GlobalMemoryStatusEx(&ms);
        swprintf_s(buf, L"%.2f GB", ms.ullTotalPhys/1024.0/1024.0/1024.0); c.items.push_back({L"总内存", buf});
        swprintf_s(buf, L"%.2f GB", ms.ullAvailPhys/1024.0/1024.0/1024.0); c.items.push_back({L"可用内存", buf});
        swprintf_s(buf, L"%d%%", ms.dwMemoryLoad); c.items.push_back({L"使用率", buf});
        g_siCards.push_back(c);
    }
    {SysCard c; c.title = L"磁盘";
        WCHAR drv[256]; GetLogicalDriveStringsW(256, drv); WCHAR* p = drv;
        while (*p) {
            if (GetDriveTypeW(p) == DRIVE_FIXED) {
                ULARGE_INTEGER tot, fre, avl;
                if (GetDiskFreeSpaceExW(p, &avl, &tot, &fre)) {
                    swprintf_s(buf, L"%.1f GB / %.1f GB (%d%%)", (double)fre.QuadPart/1024/1024/1024, (double)tot.QuadPart/1024/1024/1024, (int)((1.0-(double)fre.QuadPart/tot.QuadPart)*100));
                    WCHAR lbl[8]; swprintf_s(lbl, L"磁盘 %s", p); c.items.push_back({lbl, buf});
                }
            }
            p += wcslen(p) + 1;
        }
        g_siCards.push_back(c);
    }
    {SysCard c; c.title = L"显卡 / 显示";
        DISPLAY_DEVICEW dd; dd.cb = sizeof(dd); int dev = 0;
        while (EnumDisplayDevicesW(nullptr, dev, &dd, 0)) {
            if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) { c.items.push_back({L"主显卡", dd.DeviceString}); break; }
            dev++;
        }
        swprintf_s(buf, L"%d x %d", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); c.items.push_back({L"分辨率", buf});
        g_siCards.push_back(c);
    }
    {SysCard c; c.title = L"网络";
        ULONG obl = 15000; IP_ADAPTER_INFO* ai = (IP_ADAPTER_INFO*)malloc(obl);
        if (ai && GetAdaptersInfo(ai, &obl) == ERROR_SUCCESS) {
            for (IP_ADAPTER_INFO* p = ai; p; p = p->Next) {
                if (p->Type != MIB_IF_TYPE_ETHERNET && p->Type != IF_TYPE_IEEE80211) continue;
                if (p->IpAddressList.IpAddress.String[0] == '0') continue;
                WCHAR nm[256]; MultiByteToWideChar(CP_ACP, 0, p->Description, -1, nm, 256); c.items.push_back({L"适配器", nm});
                swprintf_s(buf, L"%02X-%02X-%02X-%02X-%02X-%02X", p->Address[0],p->Address[1],p->Address[2],p->Address[3],p->Address[4],p->Address[5]); c.items.push_back({L"MAC地址", buf});
                WCHAR ip[32]; MultiByteToWideChar(CP_ACP, 0, p->IpAddressList.IpAddress.String, -1, ip, 32); c.items.push_back({L"IP地址", ip});
                break;
            }
        }
        if (ai) free(ai);
        g_siCards.push_back(c);
    }
    {SysCard c; c.title = L"主板 / BIOS";
        ReadRegString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BaseBoardManufacturer", buf, sizeof(buf)); c.items.push_back({L"主板厂商", buf});
        ReadRegString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BaseBoardProduct", buf, sizeof(buf)); c.items.push_back({L"主板型号", buf});
        ReadRegString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BIOSVersion", buf, sizeof(buf)); c.items.push_back({L"BIOS版本", buf});
        ReadRegString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BIOSReleaseDate", buf, sizeof(buf)); c.items.push_back({L"BIOS日期", buf});
        g_siCards.push_back(c);
    }
    g_siCardsBuilt = TRUE;
}

void DrawSysInfoPage(HDC hdc, int cx, int cy)
{
    int contentX = SIDEBAR_WIDTH + 20;
    int contentW = cx - SIDEBAR_WIDTH - 40;
    int scrollBarW = 6;
    int scrollBarX = cx - scrollBarW - 8;

    int cardPad = 16, cardSpacing = 12, titleH = 30, itemH = 22;
    int totalH = 0;
    for (auto& c : g_siCards) totalH += titleH + (int)c.items.size() * itemH + cardPad * 2 + cardSpacing;
    totalH += 8;

    g_siScrollMax = max(0, totalH - cy);
    if (g_siScroll > g_siScrollMax) g_siScroll = g_siScrollMax;
    if (g_siScroll < 0) g_siScroll = 0;


    HRGN clipRgn = CreateRectRgn(SIDEBAR_WIDTH, 0, scrollBarX, cy);
    SelectClipRgn(hdc, clipRgn);
    DeleteObject(clipRgn);

    HFONT oldFont = (HFONT)SelectObject(hdc, g_hFontUI);
    SetBkMode(hdc, TRANSPARENT);

    int y = 16 - g_siScroll;

    for (auto& c : g_siCards)
    {
        int cardH = titleH + (int)c.items.size() * itemH + cardPad * 2;
        if (y + cardH > 0 && y < cy)
        {
            RECT rcCard = { contentX, y, contentX + contentW, y + cardH };
            FillRect(hdc, &rcCard, g_brCardBg);
            HPEN op = (HPEN)SelectObject(hdc, g_penCardBd);
            HBRUSH ob = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            RoundRect(hdc, rcCard.left, rcCard.top, rcCard.right, rcCard.bottom, 6, 6);
            SelectObject(hdc, op); SelectObject(hdc, ob);

            int ty = y + cardPad;
            SelectObject(hdc, g_hFontBold);
            SetTextColor(hdc, CLR_ACCENT);
            RECT rcT = { contentX + cardPad, ty, contentX + contentW - cardPad, ty + titleH };
            DrawTextW(hdc, c.title, -1, &rcT, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            SelectObject(hdc, g_hFontUI);
            ty += titleH;

            for (auto& it : c.items)
            {
                SetTextColor(hdc, CLR_TEXT_DIM);
                RECT rcL = { contentX + cardPad, ty, contentX + cardPad + 120, ty + itemH };
                DrawTextW(hdc, it.first.c_str(), -1, &rcL, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
                SetTextColor(hdc, CLR_TEXT);
                RECT rcV = { contentX + cardPad + 130, ty, contentX + contentW - cardPad, ty + itemH };
                DrawTextW(hdc, it.second.c_str(), -1, &rcV, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                ty += itemH;
            }
        }
        y += cardH + cardSpacing;
    }

    SelectClipRgn(hdc, NULL);

    if (g_siScrollMax > 0)
    {
        int barH = cy;
        int thumbH = max(30, barH * cy / totalH);
        int thumbY = (int)((double)g_siScroll / g_siScrollMax * (barH - thumbH));
        RECT rcTrack = { scrollBarX, 0, scrollBarX + scrollBarW, barH };
        FillRect(hdc, &rcTrack, g_brTrack);
        RECT rcThumb = { scrollBarX, thumbY, scrollBarX + scrollBarW, thumbY + thumbH };
        FillRect(hdc, &rcThumb, g_brThumb);
    }

    SelectObject(hdc, oldFont);
}

void DrawMenuBar(HDC hdc, int cx)
{
    RECT rcBar = { 0, 0, cx, MENUBAR_H };
    FillRect(hdc, &rcBar, g_brBg);

    HFONT old = (HFONT)SelectObject(hdc, g_hFontMenu);
    SetBkMode(hdc, TRANSPARENT);

    int x = 0;
    for (int i = 0; i < g_menuBarCount; i++)
    {
        RECT rcItem = { x, 0, x + MENUBAR_ITEM_W, MENUBAR_H };
        if (i == g_hoverMenuBar || i == g_openMenuBar)
            FillRect(hdc, &rcItem, g_brMenuHover);
        SetTextColor(hdc, CLR_TEXT);
        DrawTextW(hdc, g_menuBar[i].text, -1, &rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        x += MENUBAR_ITEM_W;
    }
    SelectObject(hdc, old);

    HPEN oldPen = (HPEN)SelectObject(hdc, g_penLine);
    MoveToEx(hdc, 0, MENUBAR_H, nullptr);
    LineTo(hdc, cx, MENUBAR_H);
    SelectObject(hdc, oldPen);
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc; GetClientRect(hWnd, &rc);
    int cx = rc.right, cy = rc.bottom;

    if (!g_memDC || g_memCx != cx || g_memCy != cy)
    {
        if (g_memOldBmp) { SelectObject(g_memDC, g_memOldBmp); g_memOldBmp = nullptr; }
        if (g_memBmp) { DeleteObject(g_memBmp); g_memBmp = nullptr; }
        if (g_memDC) { DeleteDC(g_memDC); g_memDC = nullptr; }
        g_memDC = CreateCompatibleDC(hdc);
        g_memBmp = CreateCompatibleBitmap(hdc, cx, cy);
        g_memOldBmp = (HBITMAP)SelectObject(g_memDC, g_memBmp);
        g_memCx = cx; g_memCy = cy;
    }

    DrawMenuBar(g_memDC, cx);

    POINT oldOrg;
    SetViewportOrgEx(g_memDC, 0, MENUBAR_H, &oldOrg);

    int adjCy = cy - MENUBAR_H;
    int contentH = adjCy - STATUSBAR_H;
    RECT rcContent = { SIDEBAR_WIDTH, 0, cx, contentH };
    FillRect(g_memDC, &rcContent, g_brContent);

    DrawSideBar(g_memDC, cx, adjCy);
    if (g_selectedMenu == IDC_MENU_SYSINFO)
        DrawSysInfoPage(g_memDC, cx, contentH);
    else
        DrawToolGrid(g_memDC, cx, contentH);

    SetViewportOrgEx(g_memDC, oldOrg.x, oldOrg.y, nullptr);

    DrawStatusBar(g_memDC, cx, cy);

    BitBlt(hdc, 0, 0, cx, cy, g_memDC, 0, 0, SRCCOPY);
    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        g_hWndMain = hWnd;
        CreateControls(hWnd);
        ApplyDarkTitleBar(hWnd);
        DragAcceptFiles(hWnd, TRUE);
        {
            g_nid.cbSize = sizeof(NOTIFYICONDATAW);
            g_nid.hWnd = hWnd;
            g_nid.uID = 1;
            g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            g_nid.uCallbackMessage = WM_TRAYICON;
            g_nid.hIcon = LoadIconW(hInst, MAKEINTRESOURCEW(IDI_MY));
            wcscpy_s(g_nid.szTip, L"财神工具箱");
            Shell_NotifyIconW(NIM_ADD, &g_nid);
        }
        break;

    case WM_SIZE:
        LayoutControls();
        break;

    case WM_GETMINMAXINFO:
        {
            MINMAXINFO* pMMI = (MINMAXINFO*)lParam;
            pMMI->ptMinTrackSize.x = WINDOW_WIDTH;
            pMMI->ptMinTrackSize.y = WINDOW_HEIGHT;
            pMMI->ptMaxTrackSize.x = WINDOW_WIDTH;
            pMMI->ptMaxTrackSize.y = WINDOW_HEIGHT;
        }
        break;

    case WM_ENTERSIZEMOVE:
        if (g_dockAnim) { g_dockAnim = FALSE; KillTimer(hWnd, DOCK_TIMER_ID); }
        if (g_dockSide != 0) { g_dockSide = 0; g_dockHidden = TRUE; KillTimer(hWnd, DOCK_TIMER_ID); }
        break;

    case WM_EXITSIZEMOVE:
        DockCheckOnMove(hWnd);
        break;

    case WM_TIMER:
        if (wParam == DOCK_TIMER_ID) DockOnTimer(hWnd);
        else if (wParam == STATUS_TIMER_ID)
        {
            UpdateNetworkSpeed();
            RECT rc; GetClientRect(hWnd, &rc);
            RECT rcStatus = { 0, rc.bottom - STATUSBAR_H, rc.right, rc.bottom };
            InvalidateRect(hWnd, &rcStatus, FALSE);
        }
        break;

    case WM_PAINT:
        OnPaint(hWnd);
        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            if (y < MENUBAR_H)
            {
                int idx = x / MENUBAR_ITEM_W;
                if (idx >= 0 && idx < g_menuBarCount)
                {
                    g_pressedMenuBar = idx;
                    g_openMenuBar = idx;
                    SetCapture(hWnd);
                    RECT rcMB = { 0, 0, g_menuBarCount * MENUBAR_ITEM_W, MENUBAR_H };
                    InvalidateRect(hWnd, &rcMB, FALSE);
                }
                return 0;
            }

            int adjY = y - MENUBAR_H;

            if (g_selectedMenu == IDC_MENU_SYSINFO && g_siScrollMax > 0)
            {
                RECT rc; GetClientRect(hWnd, &rc);
                int scrollBarX = rc.right - 6 - 8;
                if (x >= scrollBarX && x <= scrollBarX + 6)
                {
                    g_siDragBar = TRUE;
                    g_siDragStartY = adjY;
                    g_siDragStartScroll = g_siScroll;
                    SetCapture(hWnd);
                    return 0;
                }
            }

            if (g_selectedMenu != IDC_MENU_SYSINFO && g_toolScrollMax > 0)
            {
                RECT rc; GetClientRect(hWnd, &rc);
                int scrollBarX = rc.right - 6 - 4;
                if (x >= scrollBarX && x <= scrollBarX + 6)
                {
                    g_toolDragBar = TRUE;
                    g_toolDragStartY = adjY;
                    g_toolDragStartScroll = g_toolScroll;
                    SetCapture(hWnd);
                    return 0;
                }
            }

            int menuId = HitTestMenu(x, adjY);
            if (menuId != 0 && menuId != g_selectedMenu)
            {
                g_selectedMenu = menuId;
                ShowCurrentPage();
                RECT rcSide; GetClientRect(hWnd, &rcSide);
                rcSide.right = SIDEBAR_WIDTH;
                rcSide.top = MENUBAR_H;
                InvalidateRect(hWnd, &rcSide, FALSE);
                return 0;
            }

            int toolIdx = HitTestTool(x, adjY);
            if (toolIdx >= 0)
            {
                g_selectedTool = toolIdx;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
        }
        break;

    case WM_LBUTTONDBLCLK:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int toolIdx = HitTestTool(x, y - MENUBAR_H);
            if (toolIdx >= 0 && toolIdx < (int)g_tools.size())
            {

                const WCHAR* p = g_tools[toolIdx].params.empty() ? nullptr : g_tools[toolIdx].params.c_str();
                ShellExecuteW(nullptr, L"open",
                    g_tools[toolIdx].path.c_str(), p, nullptr, SW_SHOWNORMAL);
            }
        }
        break;

    case WM_RBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int toolIdx = HitTestTool(x, y - MENUBAR_H);
            if (toolIdx >= 0 && toolIdx < (int)g_tools.size())
            {
                POINT pt = { x, y };
                ClientToScreen(hWnd, &pt);
                ShowPopupMenu(hWnd, pt.x, pt.y, g_tools[toolIdx].path);
            }
        }
        break;

    case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (g_siDragBar)
            {
                RECT rc; GetClientRect(hWnd, &rc);
                int barH = rc.bottom - STATUSBAR_H - MENUBAR_H;
                int totalH = g_siScrollMax + barH;
                int thumbH = max(30, barH * barH / totalH);
                int dy = y - MENUBAR_H - g_siDragStartY;
                g_siScroll = g_siDragStartScroll + (int)((double)dy * g_siScrollMax / (barH - thumbH));
                if (g_siScroll < 0) g_siScroll = 0;
                if (g_siScroll > g_siScrollMax) g_siScroll = g_siScrollMax;
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (g_toolDragBar)
            {
                RECT rc; GetClientRect(hWnd, &rc);
                int barH = rc.bottom - STATUSBAR_H - MENUBAR_H;
                int totalH = g_toolScrollMax + barH;
                int thumbH = max(30, barH * barH / totalH);
                int dy = y - MENUBAR_H - g_toolDragStartY;
                g_toolScroll = g_toolDragStartScroll + (int)((double)dy * g_toolScrollMax / (barH - thumbH));
                if (g_toolScroll < 0) g_toolScroll = 0;
                if (g_toolScroll > g_toolScrollMax) g_toolScroll = g_toolScrollMax;
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (!g_trackingMouse)
            {
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
                g_trackingMouse = TRUE;
            }
            BOOL needInv = FALSE;
            int newHoverMB = -1;
            if (y < MENUBAR_H) { int idx = x / MENUBAR_ITEM_W; if (idx >= 0 && idx < g_menuBarCount) newHoverMB = idx; }
            if (g_pressedMenuBar >= 0 && newHoverMB >= 0 && newHoverMB != g_openMenuBar)
            {
                g_openMenuBar = newHoverMB;
                g_pressedMenuBar = newHoverMB;
                needInv = TRUE;
            }
            if (newHoverMB != g_hoverMenuBar) { g_hoverMenuBar = newHoverMB; needInv = TRUE; }
            int adjY = y - MENUBAR_H;
            int newHover = HitTestMenu(x, adjY);
            if (newHover != g_hoverMenu) { g_hoverMenu = newHover; needInv = TRUE; }
            int newHoverTool = HitTestTool(x, adjY);
            if (newHoverTool != g_hoverTool) { g_hoverTool = newHoverTool; needInv = TRUE; }
            if (needInv) InvalidateRect(hWnd, nullptr, FALSE);
            if (newHover != 0 || newHoverTool >= 0 || newHoverMB >= 0)
                SetCursor(LoadCursor(nullptr, IDC_HAND));
        }
        break;

    case WM_LBUTTONUP:
        if (g_siDragBar) { g_siDragBar = FALSE; ReleaseCapture(); }
        if (g_toolDragBar) { g_toolDragBar = FALSE; ReleaseCapture(); }
        if (g_pressedMenuBar >= 0)
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int idx = g_pressedMenuBar;
            g_pressedMenuBar = -1;
            ReleaseCapture();
            if (y < MENUBAR_H)
            {
                int checkIdx = x / MENUBAR_ITEM_W;
                if (checkIdx == idx && checkIdx >= 0 && checkIdx < g_menuBarCount)
                {
                    POINT pt = { idx * MENUBAR_ITEM_W, MENUBAR_H };
                    ClientToScreen(hWnd, &pt);
                    ShowMenuBarPopup(hWnd, pt.x, pt.y, idx);
                    return 0;
                }
            }
            g_openMenuBar = -1;
            RECT rcMB = { 0, 0, g_menuBarCount * MENUBAR_ITEM_W, MENUBAR_H };
            InvalidateRect(hWnd, &rcMB, FALSE);
        }
        break;

    case WM_MOUSELEAVE:
        g_hoverMenu = 0;
        g_hoverTool = -1;
        g_hoverMenuBar = -1;
        g_trackingMouse = FALSE;
        g_siDragBar = FALSE;
        g_toolDragBar = FALSE;
        InvalidateRect(hWnd, nullptr, FALSE);
        break;

    case WM_MOUSEWHEEL:
        if (g_selectedMenu == IDC_MENU_SYSINFO && g_siScrollMax > 0)
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA * 40;
            g_siScroll -= delta;
            if (g_siScroll < 0) g_siScroll = 0;
            if (g_siScroll > g_siScrollMax) g_siScroll = g_siScrollMax;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        else if (g_selectedMenu != IDC_MENU_SYSINFO && g_toolScrollMax > 0)
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA * ITEM_H;
            g_toolScroll -= delta;
            if (g_toolScroll < 0) g_toolScroll = 0;
            if (g_toolScroll > g_toolScrollMax) g_toolScroll = g_toolScrollMax;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        break;

    case WM_DROPFILES:
        {
            HDROP hDrop = (HDROP)wParam;
            UINT nFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            for (UINT i = 0; i < nFiles; i++)
            {
                WCHAR srcPath[MAX_PATH];
                DragQueryFileW(hDrop, i, srcPath, MAX_PATH);
                if (g_selectedMenu == IDC_MENU_FAVORITE)
                {
                    AddFavorite(srcPath);
                }
                else
                {
                    const WCHAR* menuName = L"";
                    for (int j = 0; j < g_menuCount; j++)
                    {
                        if (g_menus[j].id == g_selectedMenu) { menuName = g_menus[j].name; break; }
                    }
                    WCHAR exePath[MAX_PATH];
                    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
                    PathRemoveFileSpecW(exePath);
                    WCHAR destDir[MAX_PATH];
                    swprintf_s(destDir, L"%s\\data\\%s", exePath, menuName);
                    CreateDirectoryW(destDir, nullptr);
                    WCHAR fname[MAX_PATH];
                    wcscpy_s(fname, srcPath);
                    PathStripPathW(fname);
                    WCHAR destPath[MAX_PATH];
                    swprintf_s(destPath, L"%s\\%s", destDir, fname);
                    CopyFileW(srcPath, destPath, FALSE);
                }
            }
            DragFinish(hDrop);
            ShowCurrentPage();
        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                ShowAboutBox(hWnd);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_TRAY_SHOW:
                ShowWindow(hWnd, SW_RESTORE);
                SetForegroundWindow(hWnd);
                break;
            case IDM_TRAY_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_REFRESH:
                ShowCurrentPage();
                break;
            case IDM_ADDTOOL:
            case IDM_IMPORT:
                {
                    if (g_selectedMenu == IDC_MENU_SYSINFO || g_selectedMenu == IDC_MENU_FAVORITE)
                    {
                        MessageBoxW(hWnd, L"该页面不支持添加工具", L"提示", MB_OK | MB_ICONINFORMATION);
                        break;
                    }
                    OPENFILENAMEW ofn = { sizeof(ofn) };
                    ofn.hwndOwner = hWnd;
                    ofn.lpstrFilter = L"可执行文件 (*.exe)\0*.exe\0所有文件 (*.*)\0*.*\0";
                    WCHAR szFile[MAX_PATH * 16] = { 0 };
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = MAX_PATH * 16;
                    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
                    if (GetOpenFileNameW(&ofn))
                    {
                        const WCHAR* menuName = L"";
                        for (int j = 0; j < g_menuCount; j++)
                        {
                            if (g_menus[j].id == g_selectedMenu) { menuName = g_menus[j].name; break; }
                        }
                        WCHAR exePath[MAX_PATH];
                        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
                        PathRemoveFileSpecW(exePath);
                        WCHAR destDir[MAX_PATH];
                        swprintf_s(destDir, L"%s\\data\\%s", exePath, menuName);
                        CreateDirectoryW(destDir, nullptr);

                        WCHAR* p = szFile;
                        if (ofn.nFileExtension > 0 && wcschr(p, 0)[1] == 0)
                        {
                            WCHAR fname[MAX_PATH];
                            wcscpy_s(fname, p);
                            PathStripPathW(fname);
                            WCHAR destPath[MAX_PATH];
                            swprintf_s(destPath, L"%s\\%s", destDir, fname);
                            CopyFileW(p, destPath, FALSE);
                        }
                        else
                        {
                            WCHAR dir[MAX_PATH];
                            wcscpy_s(dir, p);
                            p += wcslen(p) + 1;
                            while (*p)
                            {
                                WCHAR srcFull[MAX_PATH * 2];
                                swprintf_s(srcFull, L"%s\\%s", dir, p);
                                WCHAR destPath[MAX_PATH * 2];
                                swprintf_s(destPath, L"%s\\%s", destDir, p);
                                CopyFileW(srcFull, destPath, FALSE);
                                p += wcslen(p) + 1;
                            }
                        }
                        ShowCurrentPage();
                    }
                }
                break;
            case IDM_OPENFOLDER:
                {
                    const WCHAR* menuName = L"";
                    for (int j = 0; j < g_menuCount; j++)
                    {
                        if (g_menus[j].id == g_selectedMenu) { menuName = g_menus[j].name; break; }
                    }
                    WCHAR exePath[MAX_PATH];
                    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
                    PathRemoveFileSpecW(exePath);
                    WCHAR destDir[MAX_PATH];
                    swprintf_s(destDir, L"%s\\data\\%s", exePath, menuName);
                    CreateDirectoryW(destDir, nullptr);
                    ShellExecuteW(nullptr, L"open", L"explorer.exe", destDir, nullptr, SW_SHOWNORMAL);
                }
                break;
            case IDM_SETTINGS:
                MessageBoxW(hWnd, L"设置功能开发中...", L"设置", MB_OK | MB_ICONINFORMATION);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, CLR_TEXT_DIM);
            return (LRESULT)g_brContent;
        }
        break;

    case WM_FAVICON_DOWNLOADED:
        ShowCurrentPage();
        break;

    case WM_SYSCOMMAND:
        if (wParam == SC_MINIMIZE)
        {
            ShowWindow(hWnd, SW_HIDE);
            return 0;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);

    case WM_TRAYICON:
        {
            if (lParam == WM_LBUTTONDBLCLK)
            {
                ShowWindow(hWnd, SW_RESTORE);
                SetForegroundWindow(hWnd);
            }
            else if (lParam == WM_RBUTTONUP)
            {
                POINT pt; GetCursorPos(&pt);
                g_isMenuBarPop = TRUE;
                g_popHover = -1;
                g_popCount = 3;
                g_popItems[0] = { L"显示主窗口", IDM_TRAY_SHOW, FALSE };
                g_popItems[1] = { L"", 0, TRUE };
                g_popItems[2] = { L"退出", IDM_TRAY_EXIT, FALSE };
                int menuH = POP_ITEM_H * g_popCount + 6;
                g_hPopMenu = CreateWindowExW(WS_EX_TOPMOST, POP_CLASS, L"",
                    WS_POPUP | WS_BORDER, pt.x, pt.y, POP_MENU_W, menuH, hWnd, nullptr, hInst, nullptr);
                ShowWindow(g_hPopMenu, SW_SHOWNOACTIVATE);
                SetForegroundWindow(g_hPopMenu);
                SetCapture(g_hPopMenu);
            }
        }
        break;

    case WM_DESTROY:
        KillTimer(hWnd, DOCK_TIMER_ID);
        KillTimer(hWnd, STATUS_TIMER_ID);
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        for (auto& t : g_tools) if (t.icon) DestroyIcon(t.icon);
        g_tools.clear();
        for (int i = 0; i < g_menuCount; i++) { if (g_menus[i].icon) DestroyIcon(g_menus[i].icon); g_menus[i].icon = nullptr; }
        if (g_hFontUI) { DeleteObject(g_hFontUI); g_hFontUI = nullptr; }
        if (g_hFontMenu) { DeleteObject(g_hFontMenu); g_hFontMenu = nullptr; }
        if (g_hFontStatus) { DeleteObject(g_hFontStatus); g_hFontStatus = nullptr; }
        if (g_brBg) { DeleteObject(g_brBg); g_brBg = nullptr; }
        if (g_brSidebar) { DeleteObject(g_brSidebar); g_brSidebar = nullptr; }
        if (g_brContent) { DeleteObject(g_brContent); g_brContent = nullptr; }
        if (g_brStatus) { DeleteObject(g_brStatus); g_brStatus = nullptr; }
        if (g_hFontBold) { DeleteObject(g_hFontBold); g_hFontBold = nullptr; }
        if (g_penLine) { DeleteObject(g_penLine); g_penLine = nullptr; }
        if (g_brMenuSel) { DeleteObject(g_brMenuSel); g_brMenuSel = nullptr; }
        if (g_brMenuHover) { DeleteObject(g_brMenuHover); g_brMenuHover = nullptr; }
        if (g_brAccent) { DeleteObject(g_brAccent); g_brAccent = nullptr; }
        if (g_brCardBg) { DeleteObject(g_brCardBg); g_brCardBg = nullptr; }
        if (g_penCardBd) { DeleteObject(g_penCardBd); g_penCardBd = nullptr; }
        if (g_brThumb) { DeleteObject(g_brThumb); g_brThumb = nullptr; }
        if (g_brTrack) { DeleteObject(g_brTrack); g_brTrack = nullptr; }
        if (g_memOldBmp) { SelectObject(g_memDC, g_memOldBmp); g_memOldBmp = nullptr; }
        if (g_memBmp) { DeleteObject(g_memBmp); g_memBmp = nullptr; }
        if (g_memDC) { DeleteDC(g_memDC); g_memDC = nullptr; }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

#define ABOUT_CLASS L"CaishenAboutDlg"
#define ABOUT_W 460
#define ABOUT_H 360
#define ABOUT_BTN_W  90
#define ABOUT_BTN_H  32
static HWND g_hAboutDlg = nullptr;
static BOOL g_aboutBtnHover = FALSE;
static BOOL g_aboutBtnPress = FALSE;
static int  g_aboutLinkHover = 0;
static HCURSOR g_hHandCursor = nullptr;

ATOM RegisterAboutClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = AboutWndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = g_brBg;
    wcex.lpszClassName = ABOUT_CLASS;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    return RegisterClassExW(&wcex);
}

void ShowAboutBox(HWND hParent)
{
    if (g_hAboutDlg && IsWindow(g_hAboutDlg)) { SetForegroundWindow(g_hAboutDlg); return; }
    RECT rcParent; GetWindowRect(hParent, &rcParent);
    int x = rcParent.left + (rcParent.right - rcParent.left - ABOUT_W) / 2;
    int y = rcParent.top + (rcParent.bottom - rcParent.top - ABOUT_H) / 2;
    g_hAboutDlg = CreateWindowExW(0, ABOUT_CLASS, L"关于",
        WS_POPUP | WS_CAPTION | WS_SYSMENU, x, y, ABOUT_W, ABOUT_H,
        hParent, nullptr, hInst, nullptr);
    ApplyDarkTitleBar(g_hAboutDlg);
    ShowWindow(g_hAboutDlg, SW_SHOW);
    SetForegroundWindow(g_hAboutDlg);
}

LRESULT CALLBACK AboutWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc; GetClientRect(hWnd, &rc);
            int cx = rc.right, cy = rc.bottom;

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBmp = CreateCompatibleBitmap(hdc, cx, cy);
            HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

            FillRect(memDC, &rc, g_brBg);

            HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MY));
            DrawIconEx(memDC, 30, 30, hIcon, 32, 32, 0, nullptr, DI_NORMAL);

            HFONT hTitle = CreateFontW(-18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
            HFONT hNormal = CreateFontW(-13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");

            SetBkMode(memDC, TRANSPARENT);
            SelectObject(memDC, hTitle);
            SetTextColor(memDC, CLR_TEXT);
            RECT rcTitle = { 76, 30, cx - 24, 30 + 28 };
            DrawTextW(memDC, L"财神工具箱", -1, &rcTitle, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

            SelectObject(memDC, hNormal);
            SetTextColor(memDC, CLR_TEXT_DIM);
            RECT rcVer = { 76, 66, cx - 24, 66 + 22 };
            DrawTextW(memDC, L"版本 1.0.0.100", -1, &rcVer, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            RECT rcCopy = { 76, 94, cx - 24, 94 + 22 };
            DrawTextW(memDC, L"版权所有 (C) 2026 财神工具箱", -1, &rcCopy, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            RECT rcTech = { 76, 122, cx - 24, 122 + 22 };
            DrawTextW(memDC, L"技术架构：C++20 · Win32 API · GDI+ · MSVC v145", -1, &rcTech, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            SetTextColor(memDC, g_aboutLinkHover == 1 ? RGB(0x42, 0xA5, 0xF5) : CLR_TEXT_DIM);
            RECT rcSite = { 76, 150, cx - 24, 150 + 22 };
            DrawTextW(memDC, L"官方网站：https://wincstool.cn", -1, &rcSite, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            SetTextColor(memDC, g_aboutLinkHover == 2 ? RGB(0x42, 0xA5, 0xF5) : CLR_TEXT_DIM);
            RECT rcMail = { 76, 178, cx - 24, 178 + 22 };
            DrawTextW(memDC, L"官方邮箱：getcstool@hotmail.com", -1, &rcMail, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            SetTextColor(memDC, CLR_TEXT_DIM);
            RECT rcTags = { 76, 206, cx - 24, 206 + 22 };
            DrawTextW(memDC, L"纯净无广 · 完全免费 · 完全开源", -1, &rcTags, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

            HPEN sepPen = CreatePen(PS_SOLID, 1, CLR_LINE);
            HPEN oldPen = (HPEN)SelectObject(memDC, sepPen);
            MoveToEx(memDC, 30, 236, nullptr);
            LineTo(memDC, cx - 30, 236);
            SelectObject(memDC, oldPen);
            DeleteObject(sepPen);


            int btnX = cx - ABOUT_BTN_W - 20;
            int btnY = cy - ABOUT_BTN_H - 16;
            RECT rcBtn = { btnX, btnY, btnX + ABOUT_BTN_W, btnY + ABOUT_BTN_H };
            COLORREF btnBg = g_aboutBtnPress ? CLR_MENU_SEL : (g_aboutBtnHover ? CLR_MENU_HOVER : CLR_LINE);
            HBRUSH brBtn = CreateSolidBrush(btnBg);
            FillRect(memDC, &rcBtn, brBtn);
            DeleteObject(brBtn);
            SetTextColor(memDC, CLR_TEXT);
            DrawTextW(memDC, L"确定", -1, &rcBtn, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

            DeleteObject(hTitle);
            DeleteObject(hNormal);

            BitBlt(hdc, 0, 0, cx, cy, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBmp);
            DeleteObject(memBmp);
            DeleteDC(memDC);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            RECT rc; GetClientRect(hWnd, &rc);
            int btnX = rc.right - ABOUT_BTN_W - 20;
            int btnY = rc.bottom - ABOUT_BTN_H - 16;
            if (x >= btnX && x < btnX + ABOUT_BTN_W && y >= btnY && y < btnY + ABOUT_BTN_H)
            {
                g_aboutBtnPress = TRUE;
                SetCapture(hWnd);
            }
        }
        break;

    case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            RECT rc; GetClientRect(hWnd, &rc);
            int btnX = rc.right - ABOUT_BTN_W - 20;
            int btnY = rc.bottom - ABOUT_BTN_H - 16;
            if (g_aboutBtnPress)
            {
                g_aboutBtnPress = FALSE;
                ReleaseCapture();
                if (x >= btnX && x < btnX + ABOUT_BTN_W && y >= btnY && y < btnY + ABOUT_BTN_H)
                {
                    DestroyWindow(hWnd);
                    return 0;
                }
            }
            if (x >= 76 && x < rc.right - 24 && y >= 150 && y < 172)
                ShellExecuteW(nullptr, L"open", L"https://wincstool.cn", nullptr, nullptr, SW_SHOWNORMAL);
            else if (x >= 76 && x < rc.right - 24 && y >= 178 && y < 200)
                ShellExecuteW(nullptr, L"open", L"mailto:getcstool@hotmail.com", nullptr, nullptr, SW_SHOWNORMAL);
        }
        break;

    case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            RECT rc; GetClientRect(hWnd, &rc);
            int btnX = rc.right - ABOUT_BTN_W - 20;
            int btnY = rc.bottom - ABOUT_BTN_H - 16;
            BOOL newHover = (x >= btnX && x < btnX + ABOUT_BTN_W && y >= btnY && y < btnY + ABOUT_BTN_H);
            if (newHover != g_aboutBtnHover)
            {
                g_aboutBtnHover = newHover;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            int newLinkHover = 0;
            if (x >= 76 && x < rc.right - 24 && y >= 150 && y < 172) newLinkHover = 1;
            else if (x >= 76 && x < rc.right - 24 && y >= 178 && y < 200) newLinkHover = 2;
            if (newLinkHover != g_aboutLinkHover)
            {
                g_aboutLinkHover = newLinkHover;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            if (!g_hHandCursor) g_hHandCursor = LoadCursor(nullptr, IDC_HAND);
            SetCursor(newLinkHover > 0 ? g_hHandCursor : LoadCursor(nullptr, IDC_ARROW));
        }
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE || wParam == VK_RETURN)
        {
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
        g_hAboutDlg = nullptr;
        break;

    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

ATOM RegisterPopMenuClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = PopMenuProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(CLR_BG);
    wcex.lpszClassName = POP_CLASS;
    return RegisterClassExW(&wcex);
}

void CopyTextToClipboard(const std::wstring& text)
{
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem)
    {
        LPWSTR p = (LPWSTR)GlobalLock(hMem);
        if (p) { wcscpy_s(p, text.size() + 1, text.c_str()); GlobalUnlock(hMem); }
        SetClipboardData(CF_UNICODETEXT, hMem);
    }
    CloseClipboard();
}

void CopyFilesToClipboard(const std::vector<std::wstring>& paths)
{
    if (paths.empty()) return;
    size_t chars = 0;
    for (const auto& p : paths) chars += p.size() + 1;
    chars += 1;
    const size_t dfSize = 20;
    size_t bufSize = dfSize + chars * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bufSize);
    if (!hMem) return;
    LPBYTE pMem = (LPBYTE)GlobalLock(hMem);
    if (!pMem) { GlobalFree(hMem); return; }
    *(DWORD*)(pMem + 0)  = (DWORD)dfSize;
    *(DWORD*)(pMem + 4)  = 0;
    *(DWORD*)(pMem + 8)  = 0;
    *(DWORD*)(pMem + 12) = 0;
    *(DWORD*)(pMem + 16) = 1;
    LPWSTR p = (LPWSTR)(pMem + dfSize);
    for (const auto& path : paths)
    {
        wcscpy_s(p, path.size() + 1, path.c_str());
        p += path.size() + 1;
    }
    *p = 0;
    GlobalUnlock(hMem);
    if (OpenClipboard(nullptr)) { EmptyClipboard(); SetClipboardData(CF_HDROP, hMem); CloseClipboard(); }
    else GlobalFree(hMem);
}

void ExecutePopCommand(HWND hOwner, int cmd, const std::wstring& path)
{
    switch (cmd)
    {
    case POP_OPEN:
        {

            const WCHAR* p = nullptr;
            for (auto& t : g_tools)
            {
                if (_wcsicmp(t.path.c_str(), path.c_str()) == 0 && !t.params.empty())
                { p = t.params.c_str(); break; }
            }
            ShellExecuteW(nullptr, L"open", path.c_str(), p, nullptr, SW_SHOWNORMAL);
        }
        break;
    case POP_COPY_PROG:
        {
            WCHAR dir[MAX_PATH]; wcscpy_s(dir, path.c_str()); PathRemoveFileSpecW(dir);
            std::vector<std::wstring> files;
            bool isSub = false;
            for (auto& t : g_tools)
            {
                if (_wcsicmp(t.path.c_str(), path.c_str()) == 0 && t.isSubFolder)
                {
                    isSub = true;
                    break;
                }
            }
            if (isSub) files.push_back(dir);
            else
            {
                WCHAR dirName[MAX_PATH]; wcscpy_s(dirName, dir); PathStripPathW(dirName);
                WCHAR exeName[MAX_PATH]; wcscpy_s(exeName, path.c_str()); PathStripPathW(exeName); PathRemoveExtensionW(exeName);
                if (_wcsicmp(dirName, exeName) == 0) files.push_back(dir);
                else files.push_back(path);
            }
            CopyFilesToClipboard(files);
        }
        break;
    case POP_ADMIN:
        ShellExecuteW(nullptr, L"runas", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        break;
    case POP_COPY:
        CopyTextToClipboard(path);
        break;
    case POP_FOLDER:
        {
            std::wstring cmdLine = L"/select,\"" + path + L"\"";
            ShellExecuteW(nullptr, L"open", L"explorer.exe", cmdLine.c_str(), nullptr, SW_SHOWNORMAL);
        }
        break;
    case POP_ADD_FAVOR:
        AddFavorite(path);
        ShowCurrentPage();
        break;
    case POP_REMOVE_FAVOR:
        RemoveFavorite(path);
        ShowCurrentPage();
        break;
    case POP_DELETE:
        {
            WCHAR dir[MAX_PATH]; wcscpy_s(dir, path.c_str()); PathRemoveFileSpecW(dir);
            bool isSub = false;
            for (auto& t : g_tools)
            {
                if (_wcsicmp(t.path.c_str(), path.c_str()) == 0 && t.isSubFolder)
                {
                    isSub = true;
                    break;
                }
            }
            if (isSub)
            {
                WCHAR cmd[MAX_PATH * 2];
                swprintf_s(cmd, L"/c rmdir /s /q \"%s\"", dir);
                ShellExecuteW(nullptr, L"open", L"cmd.exe", cmd, nullptr, SW_HIDE);
            }
            else
            {
                WCHAR dirName[MAX_PATH]; wcscpy_s(dirName, dir); PathStripPathW(dirName);
                WCHAR exeName[MAX_PATH]; wcscpy_s(exeName, path.c_str()); PathStripPathW(exeName); PathRemoveExtensionW(exeName);
                if (_wcsicmp(dirName, exeName) == 0)
                {
                    WCHAR cmd[MAX_PATH * 2];
                    swprintf_s(cmd, L"/c rmdir /s /q \"%s\"", dir);
                    ShellExecuteW(nullptr, L"open", L"cmd.exe", cmd, nullptr, SW_HIDE);
                }
                else
                {
                    DeleteFileW(path.c_str());
                }
            }
            ShowCurrentPage();
        }
        break;
    case POP_PROPERTIES:
        {
            SHELLEXECUTEINFOW sei = { 0 };
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_INVOKEIDLIST;
            sei.hwnd = hOwner;
            sei.lpVerb = L"properties";
            sei.lpFile = path.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        }
        break;
    }
}

void ShowMenuBarPopup(HWND hOwner, int x, int y, int menuIdx)
{
    g_isMenuBarPop = TRUE;
    g_popHover = -1;
    g_popCount = g_menuBarCmdCounts[menuIdx];
    for (int i = 0; i < g_popCount; i++)
    {
        g_popItems[i].text = g_menuBarCmds[menuIdx][i].text;
        g_popItems[i].id = g_menuBarCmds[menuIdx][i].id;
        g_popItems[i].separator = g_menuBarCmds[menuIdx][i].separator;
    }
    int menuH = POP_ITEM_H * g_popCount + 6;
    g_hPopMenu = CreateWindowExW(WS_EX_TOPMOST, POP_CLASS, L"",
        WS_POPUP | WS_BORDER, x, y, POP_MENU_W, menuH, hOwner, nullptr, hInst, nullptr);
    ShowWindow(g_hPopMenu, SW_SHOWNOACTIVATE);
    SetForegroundWindow(g_hPopMenu);
    SetCapture(g_hPopMenu);
}

void ShowPopupMenu(HWND hOwner, int x, int y, const std::wstring& path)
{
    g_popTarget = path;
    g_popHover = -1;
    g_isMenuBarPop = FALSE;

    {
        g_popItems[0] = { L"打开",              POP_OPEN,         FALSE };
        g_popItems[1] = { L"复制",              POP_COPY_PROG,    FALSE };
        g_popItems[2] = { L"以管理员身份运行",  POP_ADMIN,        FALSE };
        g_popItems[3] = { L"复制路径",          POP_COPY,         FALSE };
        g_popItems[4] = { L"打开所在文件夹",    POP_FOLDER,       FALSE };
        if (IsFavorite(path))
            g_popItems[5] = { L"取消个人常用", POP_REMOVE_FAVOR, FALSE };
        else
            g_popItems[5] = { L"添加到个人常用", POP_ADD_FAVOR, FALSE };
        g_popItems[6] = { L"删除",              POP_DELETE,       FALSE };
        g_popItems[7] = { L"属性",              POP_PROPERTIES,   FALSE };
        g_popCount = ARRAYSIZE(g_popItems);
    }
    int menuH = POP_ITEM_H * g_popCount + 6;
    g_hPopMenu = CreateWindowExW(WS_EX_TOPMOST, POP_CLASS, L"",
        WS_POPUP | WS_BORDER, x, y, POP_MENU_W, menuH, hOwner, nullptr, hInst, nullptr);
    ShowWindow(g_hPopMenu, SW_SHOWNOACTIVATE);
    SetForegroundWindow(g_hPopMenu);
    SetCapture(g_hPopMenu);
}

LRESULT CALLBACK PopMenuProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc; GetClientRect(hWnd, &rc);
            FillRect(hdc, &rc, g_brBg);
            SetBkMode(hdc, TRANSPARENT);
            HFONT old = (HFONT)SelectObject(hdc, g_hFontUI);
            for (int i = 0; i < g_popCount; i++)
            {
                RECT rcItem = { 0, 3 + i * POP_ITEM_H, rc.right, 3 + (i + 1) * POP_ITEM_H };
                if (g_popItems[i].separator)
                {
                    HPEN op = (HPEN)SelectObject(hdc, g_penLine);
                    int sy = rcItem.top + POP_ITEM_H / 2;
                    MoveToEx(hdc, 8, sy, nullptr);
                    LineTo(hdc, rc.right - 8, sy);
                    SelectObject(hdc, op);
                    continue;
                }
                if (i == g_popHover)
                    FillRect(hdc, &rcItem, g_brMenuHover);
                SetTextColor(hdc, CLR_TEXT);
                RECT rcText = rcItem; rcText.left += 14;
                DrawTextW(hdc, g_popItems[i].text, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }
            SelectObject(hdc, old);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_MOUSEMOVE:
        {
            int y = GET_Y_LPARAM(lParam);
            int newHover = -1;
            if (y >= 3) { int idx = (y - 3) / POP_ITEM_H; if (idx >= 0 && idx < g_popCount && !g_popItems[idx].separator) newHover = idx; }
            if (newHover != g_popHover) { g_popHover = newHover; InvalidateRect(hWnd, nullptr, FALSE); }
        }
        break;

    case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            ReleaseCapture();
            int idx = -1;
            if (y >= 3 && x >= 0 && x < POP_MENU_W) { idx = (y - 3) / POP_ITEM_H; if (idx < 0 || idx >= g_popCount || g_popItems[idx].separator) idx = -1; }
            HWND hOwner = GetParent(hWnd);
            int cmdId = (idx >= 0) ? g_popItems[idx].id : 0;
            BOOL wasMenuBar = g_isMenuBarPop;
            DestroyWindow(hWnd);
            g_hPopMenu = nullptr;
            g_openMenuBar = -1;
            g_isMenuBarPop = FALSE;
            if (hOwner && wasMenuBar)
            {
                RECT rcMB = { 0, 0, g_menuBarCount * MENUBAR_ITEM_W, MENUBAR_H };
                InvalidateRect(hOwner, &rcMB, FALSE);
            }
            if (idx >= 0)
            {
                if (wasMenuBar)
                    SendMessageW(hOwner, WM_COMMAND, cmdId, 0);
                else
                    ExecutePopCommand(hOwner, cmdId, g_popTarget);
            }
        }
        break;

    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        {
            BOOL wasMenuBar = g_isMenuBarPop;
            HWND hOwner = GetParent(hWnd);
            ReleaseCapture();
            DestroyWindow(hWnd);
            g_hPopMenu = nullptr;
            g_openMenuBar = -1;
            g_isMenuBarPop = FALSE;
            if (hOwner && wasMenuBar)
            {
                RECT rcMB = { 0, 0, g_menuBarCount * MENUBAR_ITEM_W, MENUBAR_H };
                InvalidateRect(hOwner, &rcMB, FALSE);
            }
        }
        break;

    case WM_KILLFOCUS:
        {
            BOOL wasMenuBar = g_isMenuBarPop;
            HWND hOwner = GetParent(hWnd);
            ReleaseCapture();
            DestroyWindow(hWnd);
            g_hPopMenu = nullptr;
            g_openMenuBar = -1;
            g_isMenuBarPop = FALSE;
            if (hOwner && wasMenuBar)
            {
                RECT rcMB = { 0, 0, g_menuBarCount * MENUBAR_ITEM_W, MENUBAR_H };
                InvalidateRect(hOwner, &rcMB, FALSE);
            }
        }
        break;

    case WM_DESTROY:
        g_hPopMenu = nullptr;
        break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
