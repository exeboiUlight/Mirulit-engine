/*
 * MIRULIT/UTILS.H
 * BY EXEBOI
 */

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* === Конфигурация === */
#ifdef MIRULIT_STATIC
#define MIRULIT_API static
#else
#define MIRULIT_API extern
#endif

/* === Типы данных === */
typedef void (*MirulitErrorFun)(int code, const char* desc);
typedef void (*MirulitKeyFun)(int key, int action);
typedef void (*MirulitMouseButtonFun)(int button, int action);
typedef void (*MirulitCursorPosFun)(double x, double y);
typedef void (*MirulitWindowSizeFun)(int width, int height);
typedef void (*MirulitScrollFun)(double xoffset, double yoffset);

/* Структура окна */
typedef struct MirulitWindow MirulitWindow;

/* Коды ошибок */
#define MIRULIT_NO_ERROR          0
#define MIRULIT_NOT_INITIALIZED   1
#define MIRULIT_NO_CURRENT_CONTEXT 2
#define MIRULIT_INVALID_ENUM      3
#define MIRULIT_INVALID_VALUE     4
#define MIRULIT_OUT_OF_MEMORY     5
#define MIRULIT_API_UNAVAILABLE   6
#define MIRULIT_VERSION_UNAVAILABLE 7
#define MIRULIT_PLATFORM_ERROR    8
#define MIRULIT_FORMAT_UNAVAILABLE 9
#define MIRULIT_NO_WINDOW_CONTEXT 10

/* Клавиши */
#define MIRULIT_KEY_UNKNOWN       -1
#define MIRULIT_KEY_SPACE         32
#define MIRULIT_KEY_APOSTROPHE    39
#define MIRULIT_KEY_COMMA         44
#define MIRULIT_KEY_MINUS         45
#define MIRULIT_KEY_PERIOD        46
#define MIRULIT_KEY_SLASH         47
#define MIRULIT_KEY_0             48
#define MIRULIT_KEY_1             49
#define MIRULIT_KEY_2             50
#define MIRULIT_KEY_3             51
#define MIRULIT_KEY_4             52
#define MIRULIT_KEY_5             53
#define MIRULIT_KEY_6             54
#define MIRULIT_KEY_7             55
#define MIRULIT_KEY_8             56
#define MIRULIT_KEY_9             57
#define MIRULIT_KEY_SEMICOLON     59
#define MIRULIT_KEY_EQUAL         61
#define MIRULIT_KEY_A             65
#define MIRULIT_KEY_B             66
#define MIRULIT_KEY_C             67
#define MIRULIT_KEY_D             68
#define MIRULIT_KEY_E             69
#define MIRULIT_KEY_F             70
#define MIRULIT_KEY_G             71
#define MIRULIT_KEY_H             72
#define MIRULIT_KEY_I             73
#define MIRULIT_KEY_J             74
#define MIRULIT_KEY_K             75
#define MIRULIT_KEY_L             76
#define MIRULIT_KEY_M             77
#define MIRULIT_KEY_N             78
#define MIRULIT_KEY_O             79
#define MIRULIT_KEY_P             80
#define MIRULIT_KEY_Q             81
#define MIRULIT_KEY_R             82
#define MIRULIT_KEY_S             83
#define MIRULIT_KEY_T             84
#define MIRULIT_KEY_U             85
#define MIRULIT_KEY_V             86
#define MIRULIT_KEY_W             87
#define MIRULIT_KEY_X             88
#define MIRULIT_KEY_Y             89
#define MIRULIT_KEY_Z             90
#define MIRULIT_KEY_LEFT_BRACKET  91
#define MIRULIT_KEY_BACKSLASH     92
#define MIRULIT_KEY_RIGHT_BRACKET 93
#define MIRULIT_KEY_GRAVE_ACCENT  96
#define MIRULIT_KEY_ESCAPE        256
#define MIRULIT_KEY_ENTER         257
#define MIRULIT_KEY_TAB           258
#define MIRULIT_KEY_BACKSPACE     259
#define MIRULIT_KEY_INSERT        260
#define MIRULIT_KEY_DELETE        261
#define MIRULIT_KEY_RIGHT         262
#define MIRULIT_KEY_LEFT          263
#define MIRULIT_KEY_DOWN          264
#define MIRULIT_KEY_UP            265
#define MIRULIT_KEY_PAGE_UP       266
#define MIRULIT_KEY_PAGE_DOWN     267
#define MIRULIT_KEY_HOME          268
#define MIRULIT_KEY_END           269
#define MIRULIT_KEY_CAPS_LOCK     280
#define MIRULIT_KEY_SCROLL_LOCK   281
#define MIRULIT_KEY_NUM_LOCK      282
#define MIRULIT_KEY_PRINT_SCREEN  283
#define MIRULIT_KEY_PAUSE         284
#define MIRULIT_KEY_F1            290
#define MIRULIT_KEY_F2            291
#define MIRULIT_KEY_F3            292
#define MIRULIT_KEY_F4            293
#define MIRULIT_KEY_F5            294
#define MIRULIT_KEY_F6            295
#define MIRULIT_KEY_F7            296
#define MIRULIT_KEY_F8            297
#define MIRULIT_KEY_F9            298
#define MIRULIT_KEY_F10           299
#define MIRULIT_KEY_F11           300
#define MIRULIT_KEY_F12           301
#define MIRULIT_KEY_LEFT_SHIFT    340
#define MIRULIT_KEY_LEFT_CONTROL  341
#define MIRULIT_KEY_LEFT_ALT      342
#define MIRULIT_KEY_LEFT_SUPER    343
#define MIRULIT_KEY_RIGHT_SHIFT   344
#define MIRULIT_KEY_RIGHT_CONTROL 345
#define MIRULIT_KEY_RIGHT_ALT     346
#define MIRULIT_KEY_RIGHT_SUPER   347

/* Состояния клавиш/кнопок */
#define MIRULIT_RELEASE 0
#define MIRULIT_PRESS   1
#define MIRULIT_REPEAT  2

/* Кнопки мыши */
#define MIRULIT_MOUSE_BUTTON_1     0
#define MIRULIT_MOUSE_BUTTON_2     1
#define MIRULIT_MOUSE_BUTTON_3     2
#define MIRULIT_MOUSE_BUTTON_4     3
#define MIRULIT_MOUSE_BUTTON_5     4
#define MIRULIT_MOUSE_BUTTON_6     5
#define MIRULIT_MOUSE_BUTTON_7     6
#define MIRULIT_MOUSE_BUTTON_8     7
#define MIRULIT_MOUSE_BUTTON_LAST  MIRULIT_MOUSE_BUTTON_8
#define MIRULIT_MOUSE_BUTTON_LEFT  MIRULIT_MOUSE_BUTTON_1
#define MIRULIT_MOUSE_BUTTON_RIGHT MIRULIT_MOUSE_BUTTON_2
#define MIRULIT_MOUSE_BUTTON_MIDDLE MIRULIT_MOUSE_BUTTON_3

/* Флаги окна */
#define MIRULIT_VISIBLE        0x00000001
#define MIRULIT_RESIZABLE      0x00000002
#define MIRULIT_DECORATED      0x00000004
#define MIRULIT_FOCUSED        0x00000008
#define MIRULIT_AUTO_ICONIFY   0x00000010
#define MIRULIT_FLOATING       0x00000020
#define MIRULIT_MAXIMIZED      0x00000040

/* === Прототипы функций === */

/* Инициализация/завершение */
MIRULIT_API int mirulitInit(void);
MIRULIT_API void mirulitTerminate(void);

/* Создание/уничтожение окна */
MIRULIT_API MirulitWindow* mirulitCreateWindow(int width, int height, const char* title, void* monitor, void* share);
MIRULIT_API void mirulitDestroyWindow(MirulitWindow* window);

/* Работа с окном */
MIRULIT_API void mirulitMakeContextCurrent(MirulitWindow* window);
MIRULIT_API MirulitWindow* mirulitGetCurrentContext(void);
MIRULIT_API int mirulitWindowShouldClose(MirulitWindow* window);
MIRULIT_API void mirulitSetWindowShouldClose(MirulitWindow* window, int value);
MIRULIT_API void mirulitSetWindowTitle(MirulitWindow* window, const char* title);
MIRULIT_API void mirulitGetWindowSize(MirulitWindow* window, int* width, int* height);
MIRULIT_API void mirulitSetWindowSize(MirulitWindow* window, int width, int height);
MIRULIT_API void mirulitGetFramebufferSize(MirulitWindow* window, int* width, int* height);
MIRULIT_API void mirulitIconifyWindow(MirulitWindow* window);
MIRULIT_API void mirulitRestoreWindow(MirulitWindow* window);
MIRULIT_API void mirulitShowWindow(MirulitWindow* window);
MIRULIT_API void mirulitHideWindow(MirulitWindow* window);
MIRULIT_API void mirulitFocusWindow(MirulitWindow* window);
MIRULIT_API int mirulitGetWindowAttrib(MirulitWindow* window, int attrib);

/* Ввод */
MIRULIT_API int mirulitGetKey(MirulitWindow* window, int key);
MIRULIT_API int mirulitGetMouseButton(MirulitWindow* window, int button);
MIRULIT_API void mirulitGetCursorPos(MirulitWindow* window, double* xpos, double* ypos);
MIRULIT_API void mirulitSetCursorPos(MirulitWindow* window, double xpos, double ypos);
MIRULIT_API void mirulitSetInputMode(MirulitWindow* window, int mode, int value);
MIRULIT_API int mirulitGetInputMode(MirulitWindow* window, int mode);

/* Функции обратного вызова */
MIRULIT_API void mirulitSetErrorCallback(MirulitErrorFun callback);
MIRULIT_API void mirulitSetKeyCallback(MirulitWindow* window, MirulitKeyFun callback);
MIRULIT_API void mirulitSetMouseButtonCallback(MirulitWindow* window, MirulitMouseButtonFun callback);
MIRULIT_API void mirulitSetCursorPosCallback(MirulitWindow* window, MirulitCursorPosFun callback);
MIRULIT_API void mirulitSetWindowSizeCallback(MirulitWindow* window, MirulitWindowSizeFun callback);
MIRULIT_API void mirulitSetScrollCallback(MirulitWindow* window, MirulitScrollFun callback);

/* Управление событиями */
MIRULIT_API void mirulitPollEvents(void);
MIRULIT_API void mirulitWaitEvents(void);
MIRULIT_API void mirulitWaitEventsTimeout(double timeout);

/* Буфер обмена */
MIRULIT_API void mirulitSwapBuffers(MirulitWindow* window);
MIRULIT_API void mirulitSwapInterval(int interval);

/* Время */
MIRULIT_API double mirulitGetTime(void);
MIRULIT_API void mirulitSetTime(double time);

/* Утилиты */
MIRULIT_API const char* mirulitGetVersionString(void);
MIRULIT_API void* mirulitGetProcAddress(const char* procname);

#ifdef __cplusplus
}
#endif

/* === Реализация === */
#ifdef MIRULIT_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Платформозависимые заголовки */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#endif

/* Структура окна */
struct MirulitWindow {
    int width;
    int height;
    int shouldClose;
    char title[256];
    
    /* Callbacks */
    MirulitKeyFun keyCallback;
    MirulitMouseButtonFun mouseButtonCallback;
    MirulitCursorPosFun cursorPosCallback;
    MirulitWindowSizeFun windowSizeCallback;
    MirulitScrollFun scrollCallback;
    
    /* Состояние ввода */
    unsigned char keys[512];
    unsigned char mouseButtons[8];
    double cursorX, cursorY;
    
#ifdef _WIN32
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
#else
    Display* display;
    Window xwindow;
    GLXContext glxContext;
    Atom wmDeleteMessage;
#endif
};

/* Глобальное состояние */
static struct {
    int initialized;
    MirulitErrorFun errorCallback;
    MirulitWindow* currentWindow;
} mirulit = {0};

/* Вспомогательные функции */
static void mirulitReportError(int code, const char* desc) {
    if (mirulit.errorCallback) {
        mirulit.errorCallback(code, desc);
    } else {
        fprintf(stderr, "Mirulit Error %d: %s\n", code, desc);
    }
}

#ifdef _WIN32

/* Windows реализация */

static LRESULT CALLBACK mirulitWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MirulitWindow* window = (MirulitWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!window) return DefWindowProc(hwnd, msg, wParam, lParam);
    
    switch (msg) {
        case WM_CLOSE:
            window->shouldClose = 1;
            return 0;
            
        case WM_SIZE: {
            window->width = LOWORD(lParam);
            window->height = HIWORD(lParam);
            if (window->windowSizeCallback) {
                window->windowSizeCallback(window->width, window->height);
            }
            break;
        }
        
        case WM_KEYDOWN:
        case WM_KEYUP: {
            int action = (msg == WM_KEYDOWN) ? MIRULIT_PRESS : MIRULIT_RELEASE;
            int key = (int)wParam;
            
            /* Преобразование ключей Windows в Mirulit */
            if (key >= 'A' && key <= 'Z') key = key - 'A' + MIRULIT_KEY_A;
            else if (key >= '0' && key <= '9') key = key - '0' + MIRULIT_KEY_0;
            else {
                switch (key) {
                    case VK_ESCAPE: key = MIRULIT_KEY_ESCAPE; break;
                    case VK_RETURN: key = MIRULIT_KEY_ENTER; break;
                    case VK_TAB: key = MIRULIT_KEY_TAB; break;
                    case VK_BACK: key = MIRULIT_KEY_BACKSPACE; break;
                    case VK_LEFT: key = MIRULIT_KEY_LEFT; break;
                    case VK_RIGHT: key = MIRULIT_KEY_RIGHT; break;
                    case VK_UP: key = MIRULIT_KEY_UP; break;
                    case VK_DOWN: key = MIRULIT_KEY_DOWN; break;
                    case VK_F1: key = MIRULIT_KEY_F1; break;
                    case VK_F2: key = MIRULIT_KEY_F2; break;
                    case VK_F3: key = MIRULIT_KEY_F3; break;
                    case VK_F4: key = MIRULIT_KEY_F4; break;
                    case VK_F5: key = MIRULIT_KEY_F5; break;
                    case VK_F6: key = MIRULIT_KEY_F6; break;
                    case VK_F7: key = MIRULIT_KEY_F7; break;
                    case VK_F8: key = MIRULIT_KEY_F8; break;
                    case VK_F9: key = MIRULIT_KEY_F9; break;
                    case VK_F10: key = MIRULIT_KEY_F10; break;
                    case VK_F11: key = MIRULIT_KEY_F11; break;
                    case VK_F12: key = MIRULIT_KEY_F12; break;
                    case VK_SHIFT: key = MIRULIT_KEY_LEFT_SHIFT; break;
                    case VK_CONTROL: key = MIRULIT_KEY_LEFT_CONTROL; break;
                    case VK_MENU: key = MIRULIT_KEY_LEFT_ALT; break;
                    default: key = MIRULIT_KEY_UNKNOWN;
                }
            }
            
            window->keys[key] = (action == MIRULIT_PRESS);
            if (window->keyCallback) {
                window->keyCallback(key, action);
            }
            break;
        }
        
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
            int button;
            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) button = MIRULIT_MOUSE_BUTTON_LEFT;
            else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) button = MIRULIT_MOUSE_BUTTON_RIGHT;
            else button = MIRULIT_MOUSE_BUTTON_MIDDLE;
            
            int action = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN) 
                         ? MIRULIT_PRESS : MIRULIT_RELEASE;
            
            window->mouseButtons[button] = (action == MIRULIT_PRESS);
            if (window->mouseButtonCallback) {
                window->mouseButtonCallback(button, action);
            }
            break;
        }
        
        case WM_MOUSEMOVE: {
            window->cursorX = (double)((short)LOWORD(lParam));
            window->cursorY = (double)((short)HIWORD(lParam));
            if (window->cursorPosCallback) {
                window->cursorPosCallback(window->cursorX, window->cursorY);
            }
            break;
        }
        
        case WM_MOUSEWHEEL: {
            double yoffset = (double)((short)HIWORD(wParam)) / (double)WHEEL_DELTA;
            if (window->scrollCallback) {
                window->scrollCallback(0.0, yoffset);
            }
            break;
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#else

/* X11 реализация */

static KeySym mirulitKeyToX11(int key) {
    switch (key) {
        case MIRULIT_KEY_A: return XK_a;
        case MIRULIT_KEY_B: return XK_b;
        case MIRULIT_KEY_C: return XK_c;
        case MIRULIT_KEY_D: return XK_d;
        case MIRULIT_KEY_E: return XK_e;
        case MIRULIT_KEY_F: return XK_f;
        case MIRULIT_KEY_G: return XK_g;
        case MIRULIT_KEY_H: return XK_h;
        case MIRULIT_KEY_I: return XK_i;
        case MIRULIT_KEY_J: return XK_j;
        case MIRULIT_KEY_K: return XK_k;
        case MIRULIT_KEY_L: return XK_l;
        case MIRULIT_KEY_M: return XK_m;
        case MIRULIT_KEY_N: return XK_n;
        case MIRULIT_KEY_O: return XK_o;
        case MIRULIT_KEY_P: return XK_p;
        case MIRULIT_KEY_Q: return XK_q;
        case MIRULIT_KEY_R: return XK_r;
        case MIRULIT_KEY_S: return XK_s;
        case MIRULIT_KEY_T: return XK_t;
        case MIRULIT_KEY_U: return XK_u;
        case MIRULIT_KEY_V: return XK_v;
        case MIRULIT_KEY_W: return XK_w;
        case MIRULIT_KEY_X: return XK_x;
        case MIRULIT_KEY_Y: return XK_y;
        case MIRULIT_KEY_Z: return XK_z;
        case MIRULIT_KEY_0: return XK_0;
        case MIRULIT_KEY_1: return XK_1;
        case MIRULIT_KEY_2: return XK_2;
        case MIRULIT_KEY_3: return XK_3;
        case MIRULIT_KEY_4: return XK_4;
        case MIRULIT_KEY_5: return XK_5;
        case MIRULIT_KEY_6: return XK_6;
        case MIRULIT_KEY_7: return XK_7;
        case MIRULIT_KEY_8: return XK_8;
        case MIRULIT_KEY_9: return XK_9;
        case MIRULIT_KEY_ESCAPE: return XK_Escape;
        case MIRULIT_KEY_ENTER: return XK_Return;
        case MIRULIT_KEY_TAB: return XK_Tab;
        case MIRULIT_KEY_BACKSPACE: return XK_BackSpace;
        case MIRULIT_KEY_SPACE: return XK_space;
        case MIRULIT_KEY_LEFT: return XK_Left;
        case MIRULIT_KEY_RIGHT: return XK_Right;
        case MIRULIT_KEY_UP: return XK_Up;
        case MIRULIT_KEY_DOWN: return XK_Down;
        case MIRULIT_KEY_F1: return XK_F1;
        case MIRULIT_KEY_F2: return XK_F2;
        case MIRULIT_KEY_F3: return XK_F3;
        case MIRULIT_KEY_F4: return XK_F4;
        case MIRULIT_KEY_F5: return XK_F5;
        case MIRULIT_KEY_F6: return XK_F6;
        case MIRULIT_KEY_F7: return XK_F7;
        case MIRULIT_KEY_F8: return XK_F8;
        case MIRULIT_KEY_F9: return XK_F9;
        case MIRULIT_KEY_F10: return XK_F10;
        case MIRULIT_KEY_F11: return XK_F11;
        case MIRULIT_KEY_F12: return XK_F12;
        case MIRULIT_KEY_LEFT_SHIFT: return XK_Shift_L;
        case MIRULIT_KEY_RIGHT_SHIFT: return XK_Shift_R;
        case MIRULIT_KEY_LEFT_CONTROL: return XK_Control_L;
        case MIRULIT_KEY_RIGHT_CONTROL: return XK_Control_R;
        case MIRULIT_KEY_LEFT_ALT: return XK_Alt_L;
        case MIRULIT_KEY_RIGHT_ALT: return XK_Alt_R;
        default: return 0;
    }
}

static int mirulitX11ToKey(KeySym keysym) {
    if (keysym >= XK_a && keysym <= XK_z) return MIRULIT_KEY_A + (keysym - XK_a);
    if (keysym >= XK_A && keysym <= XK_Z) return MIRULIT_KEY_A + (keysym - XK_A);
    if (keysym >= XK_0 && keysym <= XK_9) return MIRULIT_KEY_0 + (keysym - XK_0);
    
    switch (keysym) {
        case XK_Escape: return MIRULIT_KEY_ESCAPE;
        case XK_Return: return MIRULIT_KEY_ENTER;
        case XK_Tab: return MIRULIT_KEY_TAB;
        case XK_BackSpace: return MIRULIT_KEY_BACKSPACE;
        case XK_space: return MIRULIT_KEY_SPACE;
        case XK_Left: return MIRULIT_KEY_LEFT;
        case XK_Right: return MIRULIT_KEY_RIGHT;
        case XK_Up: return MIRULIT_KEY_UP;
        case XK_Down: return MIRULIT_KEY_DOWN;
        case XK_F1: return MIRULIT_KEY_F1;
        case XK_F2: return MIRULIT_KEY_F2;
        case XK_F3: return MIRULIT_KEY_F3;
        case XK_F4: return MIRULIT_KEY_F4;
        case XK_F5: return MIRULIT_KEY_F5;
        case XK_F6: return MIRULIT_KEY_F6;
        case XK_F7: return MIRULIT_KEY_F7;
        case XK_F8: return MIRULIT_KEY_F8;
        case XK_F9: return MIRULIT_KEY_F9;
        case XK_F10: return MIRULIT_KEY_F10;
        case XK_F11: return MIRULIT_KEY_F11;
        case XK_F12: return MIRULIT_KEY_F12;
        case XK_Shift_L: return MIRULIT_KEY_LEFT_SHIFT;
        case XK_Shift_R: return MIRULIT_KEY_RIGHT_SHIFT;
        case XK_Control_L: return MIRULIT_KEY_LEFT_CONTROL;
        case XK_Control_R: return MIRULIT_KEY_RIGHT_CONTROL;
        case XK_Alt_L: return MIRULIT_KEY_LEFT_ALT;
        case XK_Alt_R: return MIRULIT_KEY_RIGHT_ALT;
        default: return MIRULIT_KEY_UNKNOWN;
    }
}

#endif

/* Основные функции */

MIRULIT_API int mirulitInit(void) {
    if (mirulit.initialized) return 1;
    
#ifdef _WIN32
    /* Регистрация класса окна Windows */
    WNDCLASS wc = {0};
    wc.lpfnWndProc = mirulitWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "MirulitWindowClass";
    wc.style = CS_OWNDC;
    
    if (!RegisterClass(&wc)) {
        mirulitReportError(MIRULIT_PLATFORM_ERROR, "Failed to register window class");
        return 0;
    }
#else
    /* X11 инициализируется при создании первого окна */
#endif
    
    mirulit.initialized = 1;
    return 1;
}

MIRULIT_API void mirulitTerminate(void) {
    if (!mirulit.initialized) return;
    
    mirulit.currentWindow = NULL;
    mirulit.initialized = 0;
    
#ifdef _WIN32
    /* Отмена регистрации класса окна */
    UnregisterClass("MirulitWindowClass", GetModuleHandle(NULL));
#endif
}

MIRULIT_API MirulitWindow* mirulitCreateWindow(int width, int height, const char* title, void* monitor, void* share) {
    if (!mirulit.initialized && !mirulitInit()) {
        return NULL;
    }
    
    MirulitWindow* window = (MirulitWindow*)calloc(1, sizeof(MirulitWindow));
    if (!window) {
        mirulitReportError(MIRULIT_OUT_OF_MEMORY, "Failed to allocate window");
        return NULL;
    }
    
    window->width = width;
    window->height = height;
    window->shouldClose = 0;
    strncpy(window->title, title ? title : "Mirulit Window", sizeof(window->title) - 1);
    
#ifdef _WIN32
    /* Создание окна Windows */
    window->hwnd = CreateWindowEx(
        0,
        "MirulitWindowClass",
        window->title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (!window->hwnd) {
        free(window);
        mirulitReportError(MIRULIT_PLATFORM_ERROR, "Failed to create window");
        return NULL;
    }
    
    /* Создание контекста OpenGL */
    window->hdc = GetDC(window->hwnd);
    
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0, 0,
        0, 0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    
    int pixelFormat = ChoosePixelFormat(window->hdc, &pfd);
    SetPixelFormat(window->hdc, pixelFormat, &pfd);
    
    window->hglrc = wglCreateContext(window->hdc);
    
    SetWindowLongPtr(window->hwnd, GWLP_USERDATA, (LONG_PTR)window);
    ShowWindow(window->hwnd, SW_SHOW);
    
#else
    /* Создание окна X11 */
    window->display = XOpenDisplay(NULL);
    if (!window->display) {
        free(window);
        mirulitReportError(MIRULIT_PLATFORM_ERROR, "Failed to open X display");
        return NULL;
    }
    
    int screen = DefaultScreen(window->display);
    Window root = RootWindow(window->display, screen);
    
    /* Выбор визуализации OpenGL */
    static int visualAttribs[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        None
    };
    
    XVisualInfo* visual = glXChooseVisual(window->display, screen, visualAttribs);
    if (!visual) {
        XCloseDisplay(window->display);
        free(window);
        mirulitReportError(MIRULIT_PLATFORM_ERROR, "Failed to choose visual");
        return NULL;
    }
    
    /* Создание окна */
    XSetWindowAttributes swa = {0};
    swa.colormap = XCreateColormap(window->display, root, visual->visual, AllocNone);
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     StructureNotifyMask;
    
    window->xwindow = XCreateWindow(
        window->display, root,
        0, 0, width, height, 0,
        visual->depth, InputOutput, visual->visual,
        CWColormap | CWEventMask, &swa
    );
    
    /* Создание контекста OpenGL */
    window->glxContext = glXCreateContext(window->display, visual, NULL, GL_TRUE);
    
    /* Установка заголовка окна */
    XStoreName(window->display, window->xwindow, window->title);
    
    /* WM_DELETE_WINDOW сообщение */
    window->wmDeleteMessage = XInternAtom(window->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(window->display, window->xwindow, &window->wmDeleteMessage, 1);
    
    XMapWindow(window->display, window->xwindow);
    XFree(visual);
#endif
    
    return window;
}

MIRULIT_API void mirulitDestroyWindow(MirulitWindow* window) {
    if (!window) return;
    
#ifdef _WIN32
    if (window->hglrc) {
        wglDeleteContext(window->hglrc);
    }
    if (window->hwnd) {
        DestroyWindow(window->hwnd);
    }
#else
    if (window->glxContext) {
        glXDestroyContext(window->display, window->glxContext);
    }
    if (window->xwindow) {
        XDestroyWindow(window->display, window->xwindow);
    }
    if (window->display) {
        XCloseDisplay(window->display);
    }
#endif
    
    free(window);
}

MIRULIT_API void mirulitMakeContextCurrent(MirulitWindow* window) {
#ifdef _WIN32
    if (window) {
        wglMakeCurrent(window->hdc, window->hglrc);
    } else {
        wglMakeCurrent(NULL, NULL);
    }
#else
    if (window) {
        glXMakeCurrent(window->display, window->xwindow, window->glxContext);
    } else {
        glXMakeCurrent(NULL, None, NULL);
    }
#endif
    mirulit.currentWindow = window;
}

MIRULIT_API MirulitWindow* mirulitGetCurrentContext(void) {
    return mirulit.currentWindow;
}

MIRULIT_API int mirulitWindowShouldClose(MirulitWindow* window) {
    return window ? window->shouldClose : 1;
}

MIRULIT_API void mirulitSetWindowShouldClose(MirulitWindow* window, int value) {
    if (window) window->shouldClose = value;
}

MIRULIT_API void mirulitSetWindowTitle(MirulitWindow* window, const char* title) {
    if (!window || !title) return;
    
    strncpy(window->title, title, sizeof(window->title) - 1);
    
#ifdef _WIN32
    SetWindowText(window->hwnd, title);
#else
    XStoreName(window->display, window->xwindow, title);
#endif
}

MIRULIT_API void mirulitGetWindowSize(MirulitWindow* window, int* width, int* height) {
    if (width) *width = window->width;
    if (height) *height = window->height;
}

MIRULIT_API void mirulitSetWindowSize(MirulitWindow* window, int width, int height) {
    if (!window) return;
    
    window->width = width;
    window->height = height;
    
#ifdef _WIN32
    SetWindowPos(window->hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
#else
    XResizeWindow(window->display, window->xwindow, width, height);
#endif
}

MIRULIT_API void mirulitGetFramebufferSize(MirulitWindow* window, int* width, int* height) {
    mirulitGetWindowSize(window, width, height);
}

MIRULIT_API void mirulitSwapBuffers(MirulitWindow* window) {
    if (!window) return;
    
#ifdef _WIN32
    SwapBuffers(window->hdc);
#else
    glXSwapBuffers(window->display, window->xwindow);
#endif
}

MIRULIT_API void mirulitPollEvents(void) {
#ifdef _WIN32
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
    /* Обработка событий X11 для всех окон */
    if (mirulit.currentWindow && mirulit.currentWindow->display) {
        Display* display = mirulit.currentWindow->display;
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);
            
            MirulitWindow* window = mirulit.currentWindow;
            if (window && window->xwindow == event.xany.window) {
                switch (event.type) {
                    case ClientMessage:
                        if ((Atom)event.xclient.data.l[0] == window->wmDeleteMessage) {
                            window->shouldClose = 1;
                        }
                        break;
                        
                    case ConfigureNotify:
                        if (window->width != event.xconfigure.width || 
                            window->height != event.xconfigure.height) {
                            window->width = event.xconfigure.width;
                            window->height = event.xconfigure.height;
                            if (window->windowSizeCallback) {
                                window->windowSizeCallback(window->width, window->height);
                            }
                        }
                        break;
                        
                    case KeyPress:
                    case KeyRelease: {
                        int action = (event.type == KeyPress) ? MIRULIT_PRESS : MIRULIT_RELEASE;
                        KeySym keysym = XLookupKeysym(&event.xkey, 0);
                        int key = mirulitX11ToKey(keysym);
                        
                        if (key >= 0 && key < 512) {
                            window->keys[key] = (action == MIRULIT_PRESS);
                        }
                        if (window->keyCallback) {
                            window->keyCallback(key, action);
                        }
                        break;
                    }
                        
                    case ButtonPress:
                    case ButtonRelease: {
                        int button;
                        switch (event.xbutton.button) {
                            case Button1: button = MIRULIT_MOUSE_BUTTON_LEFT; break;
                            case Button2: button = MIRULIT_MOUSE_BUTTON_MIDDLE; break;
                            case Button3: button = MIRULIT_MOUSE_BUTTON_RIGHT; break;
                            case Button4: /* Scroll up */
                                if (window->scrollCallback) window->scrollCallback(0.0, 1.0);
                                break;
                            case Button5: /* Scroll down */
                                if (window->scrollCallback) window->scrollCallback(0.0, -1.0);
                                break;
                            default: button = event.xbutton.button - 1; break;
                        }
                        
                        if (event.xbutton.button >= Button1 && event.xbutton.button <= Button3) {
                            int action = (event.type == ButtonPress) ? MIRULIT_PRESS : MIRULIT_RELEASE;
                            window->mouseButtons[button] = (action == MIRULIT_PRESS);
                            if (window->mouseButtonCallback) {
                                window->mouseButtonCallback(button, action);
                            }
                        }
                        break;
                    }
                        
                    case MotionNotify:
                        window->cursorX = event.xmotion.x;
                        window->cursorY = event.xmotion.y;
                        if (window->cursorPosCallback) {
                            window->cursorPosCallback(window->cursorX, window->cursorY);
                        }
                        break;
                }
            }
        }
    }
#endif
}

MIRULIT_API void mirulitWaitEvents(void) {
#ifdef _WIN32
    WaitMessage();
    mirulitPollEvents();
#else
    if (mirulit.currentWindow && mirulit.currentWindow->display) {
        XEvent event;
        XNextEvent(mirulit.currentWindow->display, &event);
        /* Повторно отправляем событие в очередь для обработки в pollEvents */
        XPutBackEvent(mirulit.currentWindow->display, &event);
        mirulitPollEvents();
    }
#endif
}

MIRULIT_API void mirulitWaitEventsTimeout(double timeout) {
#ifdef _WIN32
    MsgWaitForMultipleObjects(0, NULL, FALSE, (DWORD)(timeout * 1000), QS_ALLINPUT);
    mirulitPollEvents();
#else
    if (mirulit.currentWindow && mirulit.currentWindow->display) {
        fd_set fds;
        FD_ZERO(&fds);
        int x11_fd = ConnectionNumber(mirulit.currentWindow->display);
        FD_SET(x11_fd, &fds);
        
        struct timeval tv;
        tv.tv_sec = (time_t)timeout;
        tv.tv_usec = (suseconds_t)((timeout - tv.tv_sec) * 1000000);
        
        select(x11_fd + 1, &fds, NULL, NULL, &tv);
        mirulitPollEvents();
    }
#endif
}

MIRULIT_API int mirulitGetKey(MirulitWindow* window, int key) {
    if (!window || key < 0 || key >= 512) return MIRULIT_RELEASE;
    return window->keys[key] ? MIRULIT_PRESS : MIRULIT_RELEASE;
}

MIRULIT_API int mirulitGetMouseButton(MirulitWindow* window, int button) {
    if (!window || button < 0 || button >= 8) return MIRULIT_RELEASE;
    return window->mouseButtons[button] ? MIRULIT_PRESS : MIRULIT_RELEASE;
}

MIRULIT_API void mirulitGetCursorPos(MirulitWindow* window, double* xpos, double* ypos) {
    if (xpos) *xpos = window->cursorX;
    if (ypos) *ypos = window->cursorY;
}

MIRULIT_API void mirulitSetCursorPos(MirulitWindow* window, double xpos, double ypos) {
    if (!window) return;
    
    window->cursorX = xpos;
    window->cursorY = ypos;
    
#ifdef _WIN32
    POINT pt = {(int)xpos, (int)ypos};
    ClientToScreen(window->hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
#else
    XWarpPointer(window->display, None, window->xwindow, 0, 0, 0, 0, (int)xpos, (int)ypos);
    XFlush(window->display);
#endif
}

MIRULIT_API double mirulitGetTime(void) {
    static clock_t start = 0;
    if (start == 0) start = clock();
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

MIRULIT_API void mirulitSetTime(double time) {
    /* Реализация оставлена как упражнение */
}

MIRULIT_API const char* mirulitGetVersionString(void) {
    return "Mirulit 1.0";
}

MIRULIT_API void* mirulitGetProcAddress(const char* procname) {
#ifdef _WIN32
    return (void*)wglGetProcAddress(procname);
#else
    return (void*)glXGetProcAddress((const GLubyte*)procname);
#endif
}

/* Callback функции */
MIRULIT_API void mirulitSetErrorCallback(MirulitErrorFun callback) {
    mirulit.errorCallback = callback;
}

MIRULIT_API void mirulitSetKeyCallback(MirulitWindow* window, MirulitKeyFun callback) {
    if (window) window->keyCallback = callback;
}

MIRULIT_API void mirulitSetMouseButtonCallback(MirulitWindow* window, MirulitMouseButtonFun callback) {
    if (window) window->mouseButtonCallback = callback;
}

MIRULIT_API void mirulitSetCursorPosCallback(MirulitWindow* window, MirulitCursorPosFun callback) {
    if (window) window->cursorPosCallback = callback;
}

MIRULIT_API void mirulitSetWindowSizeCallback(MirulitWindow* window, MirulitWindowSizeFun callback) {
    if (window) window->windowSizeCallback = callback;
}

MIRULIT_API void mirulitSetScrollCallback(MirulitWindow* window, MirulitScrollFun callback) {
    if (window) window->scrollCallback = callback;
}

/* Простые реализации для функций которые не всегда нужны */
MIRULIT_API void mirulitIconifyWindow(MirulitWindow* window) {
#ifdef _WIN32
    if (window) ShowWindow(window->hwnd, SW_MINIMIZE);
#else
    if (window) XIconifyWindow(window->display, window->xwindow, DefaultScreen(window->display));
#endif
}

MIRULIT_API void mirulitRestoreWindow(MirulitWindow* window) {
#ifdef _WIN32
    if (window) ShowWindow(window->hwnd, SW_RESTORE);
#else
    /* Упрощенная реализация для X11 */
#endif
}

MIRULIT_API void mirulitShowWindow(MirulitWindow* window) {
#ifdef _WIN32
    if (window) ShowWindow(window->hwnd, SW_SHOW);
#else
    if (window) XMapWindow(window->display, window->xwindow);
#endif
}

MIRULIT_API void mirulitHideWindow(MirulitWindow* window) {
#ifdef _WIN32
    if (window) ShowWindow(window->hwnd, SW_HIDE);
#else
    if (window) XUnmapWindow(window->display, window->xwindow);
#endif
}

MIRULIT_API void mirulitFocusWindow(MirulitWindow* window) {
#ifdef _WIN32
    if (window) SetFocus(window->hwnd);
#else
    if (window) XSetInputFocus(window->display, window->xwindow, RevertToParent, CurrentTime);
#endif
}

MIRULIT_API int mirulitGetWindowAttrib(MirulitWindow* window, int attrib) {
    /* Упрощенная реализация */
    switch (attrib) {
        case MIRULIT_VISIBLE:
        case MIRULIT_FOCUSED:
            return 1;
        default:
            return 0;
    }
}

MIRULIT_API void mirulitSetInputMode(MirulitWindow* window, int mode, int value) {
    /* Базовая реализация */
}

MIRULIT_API int mirulitGetInputMode(MirulitWindow* window, int mode) {
    return 0;
}

MIRULIT_API void mirulitSwapInterval(int interval) {
#ifdef _WIN32
    /* Для Windows используем wglSwapIntervalEXT если доступно */
    typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int interval);
    static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    
    if (wglSwapIntervalEXT == NULL) {
        /* Получаем адрес функции из драйвера */
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        
        if (wglSwapIntervalEXT == NULL) {
            /* Пробуем получить из opengl32.dll как fallback */
            HMODULE opengl = GetModuleHandle("opengl32.dll");
            if (opengl) {
                wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)GetProcAddress(opengl, "wglSwapIntervalEXT");
            }
        }
    }
    
    if (wglSwapIntervalEXT) {
        wglSwapIntervalEXT(interval);
    } else {
        /* Расширение не поддерживается */
        mirulitReportError(MIRULIT_API_UNAVAILABLE, 
                          "wglSwapIntervalEXT not supported");
    }
    
#else
    /* Для Linux/X11 используем glXSwapIntervalEXT/SGI/MESA */
    if (mirulit.currentWindow && mirulit.currentWindow->display) {
        /* Пробуем разные варианты расширений */
        
        /* 1. GLX_EXT_swap_control */
        typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display* dpy, GLXDrawable drawable, int interval);
        static PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = NULL;
        
        /* 2. GLX_SGI_swap_control */
        typedef int (*PFNGLXSWAPINTERVALSGIPROC)(int interval);
        static PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = NULL;
        
        /* 3. GLX_MESA_swap_control */
        typedef int (*PFNGLXSWAPINTERVALMESAPROC)(unsigned int interval);
        static PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA = NULL;
        
        /* Инициализируем указатели на функции при первом вызове */
        static int initialized = 0;
        if (!initialized) {
            glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)
                glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
            glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)
                glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");
            glXSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC)
                glXGetProcAddress((const GLubyte*)"glXSwapIntervalMESA");
            initialized = 1;
        }
        
        /* Пробуем использовать доступное расширение */
        if (glXSwapIntervalEXT) {
            glXSwapIntervalEXT(mirulit.currentWindow->display, 
                              mirulit.currentWindow->xwindow, interval);
        } else if (glXSwapIntervalMESA) {
            glXSwapIntervalMESA(interval);
        } else if (glXSwapIntervalSGI) {
            glXSwapIntervalSGI(interval);
        } else {
            mirulitReportError(MIRULIT_API_UNAVAILABLE,
                              "No swap interval extension available");
        }
    }
#endif
}

#endif /* MIRULIT_IMPLEMENTATION */

#endif /* MIRULIT_H */