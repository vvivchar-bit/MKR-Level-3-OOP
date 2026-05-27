#include <windows.h>
#include <string>
#include <sstream>

using namespace std;

// =====================================================
// Клас цифрового лічильника
// =====================================================
class DigitalCounter {
private:
    int minValue;
    int maxValue;
    int currentValue;

public:
    DigitalCounter(int minVal = 0, int maxVal = 9) {
        if (minVal > maxVal) {
            minValue = 0;
            maxValue = 9;
        } else {
            minValue = minVal;
            maxValue = maxVal;
        }

        currentValue = minValue;
    }

    bool setRange(int minVal, int maxVal) {
        if (minVal > maxVal) {
            return false;
        }

        minValue = minVal;
        maxValue = maxVal;
        currentValue = minValue;
        return true;
    }

    bool setMinValue(int minVal) {
        if (minVal > maxValue) {
            return false;
        }

        minValue = minVal;

        if (currentValue < minValue) {
            currentValue = minValue;
        }

        return true;
    }

    bool setMaxValue(int maxVal) {
        if (maxVal < minValue) {
            return false;
        }

        maxValue = maxVal;

        if (currentValue > maxValue) {
            currentValue = minValue;
        }

        return true;
    }

    bool increment() {
        if (currentValue >= maxValue) {
            currentValue = minValue;
            return true;
        }

        currentValue++;
        return false;
    }

    int getCurrentValue() const {
        return currentValue;
    }

    int getMinValue() const {
        return minValue;
    }

    int getMaxValue() const {
        return maxValue;
    }

    void reset() {
        currentValue = minValue;
    }
};

// =====================================================
// Глобальні змінні
// =====================================================
DigitalCounter counter(0, 9);

HWND minEdit;
HWND maxEdit;
HWND autoButton;

HFONT titleFont;
HFONT normalFont;
HFONT displayFont;
HFONT smallFont;

wstring statusMessage = L"Лічильник створено. Натисніть +1 для збільшення.";
bool autoRunning = false;

const int ID_INCREMENT = 101;
const int ID_RESET = 102;
const int ID_SET_RANGE = 103;
const int ID_GET_VALUE = 104;
const int ID_AUTO_RUN = 105;
const int ID_EXIT = 106;

const int ID_MIN_EDIT = 201;
const int ID_MAX_EDIT = 202;

const int ID_TIMER = 301;

// =====================================================
// Допоміжні функції
// =====================================================
wstring intToWString(int value) {
    wstringstream stream;
    stream << value;
    return stream.str();
}

wstring formatDisplayValue(int value) {
    if (value >= 0 && value < 10) {
        return L"00" + intToWString(value);
    }

    if (value >= 10 && value < 100) {
        return L"0" + intToWString(value);
    }

    return intToWString(value);
}

bool readIntFromEdit(HWND edit, int& result) {
    wchar_t buffer[64];
    GetWindowTextW(edit, buffer, 64);

    try {
        result = stoi(buffer);
        return true;
    }
    catch (...) {
        return false;
    }
}

void stopAutoRun(HWND hwnd) {
    KillTimer(hwnd, ID_TIMER);
    autoRunning = false;
    SetWindowTextW(autoButton, L"Автозапуск");
}

void updateWindow(HWND hwnd, const wstring& message) {
    statusMessage = message;
    InvalidateRect(hwnd, NULL, TRUE);
}

void drawTextLine(HDC hdc, const wstring& text, int x, int y, HFONT font, COLORREF color) {
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);

    TextOutW(hdc, x, y, text.c_str(), static_cast<int>(text.length()));

    SelectObject(hdc, oldFont);
}

void drawRoundedPanel(HDC hdc, RECT rect, COLORREF fillColor, COLORREF borderColor) {
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN pen = CreatePen(PS_SOLID, 2, borderColor);

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 24, 24);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(brush);
    DeleteObject(pen);
}

void drawInterface(HWND hwnd, HDC hdc) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    HBRUSH backgroundBrush = CreateSolidBrush(RGB(18, 24, 38));
    FillRect(hdc, &clientRect, backgroundBrush);
    DeleteObject(backgroundBrush);

    drawTextLine(
        hdc,
        L"МКР Рівень 3 — Завдання 2",
        30,
        20,
        titleFont,
        RGB(125, 210, 255)
    );

    drawTextLine(
        hdc,
        L"Цифровий лічильник з обмеженим діапазоном",
        30,
        58,
        normalFont,
        RGB(230, 235, 245)
    );

    drawTextLine(
        hdc,
        L"Клас підтримує мінімум, максимум, збільшення на 1, скидання після максимуму та повернення поточного значення.",
        30,
        90,
        smallFont,
        RGB(190, 200, 215)
    );

    RECT displayPanel = { 45, 135, 520, 390 };
    drawRoundedPanel(hdc, displayPanel, RGB(5, 10, 20), RGB(0, 180, 255));

    drawTextLine(
        hdc,
        L"COUNTER / TIMER DISPLAY",
        120,
        155,
        normalFont,
        RGB(0, 210, 255)
    );

    RECT valueRect = { 80, 190, 485, 310 };
    HFONT oldFont = (HFONT)SelectObject(hdc, displayFont);
    SetTextColor(hdc, RGB(255, 220, 70));
    SetBkMode(hdc, TRANSPARENT);

    wstring valueText = formatDisplayValue(counter.getCurrentValue());
    DrawTextW(
        hdc,
        valueText.c_str(),
        static_cast<int>(valueText.length()),
        &valueRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SelectObject(hdc, oldFont);

    wstring rangeText =
        L"MIN: " + intToWString(counter.getMinValue()) +
        L"        MAX: " + intToWString(counter.getMaxValue());

    drawTextLine(
        hdc,
        rangeText,
        130,
        320,
        normalFont,
        RGB(120, 255, 160)
    );

    wstring currentText =
        L"getCurrentValue(): " + intToWString(counter.getCurrentValue());

    drawTextLine(
        hdc,
        currentText,
        130,
        350,
        smallFont,
        RGB(210, 220, 235)
    );

    RECT settingsPanel = { 560, 135, 895, 390 };
    drawRoundedPanel(hdc, settingsPanel, RGB(28, 36, 55), RGB(90, 120, 170));

    drawTextLine(
        hdc,
        L"Налаштування діапазону",
        600,
        160,
        normalFont,
        RGB(230, 235, 245)
    );

    drawTextLine(
        hdc,
        L"Мінімальне значення:",
        590,
        215,
        smallFont,
        RGB(210, 220, 235)
    );

    drawTextLine(
        hdc,
        L"Максимальне значення:",
        590,
        270,
        smallFont,
        RGB(210, 220, 235)
    );

    drawTextLine(
        hdc,
        L"Кнопка Автозапуск автоматично збільшує значення",
        590,
        330,
        smallFont,
        RGB(175, 185, 200)
    );

    drawTextLine(
        hdc,
        L"у межах встановленого діапазону.",
        590,
        352,
        smallFont,
        RGB(175, 185, 200)
    );

    RECT statusPanel = { 45, 470, 895, 535 };
    drawRoundedPanel(hdc, statusPanel, RGB(32, 42, 60), RGB(255, 190, 70));

    drawTextLine(
        hdc,
        L"СТАН:",
        70,
        490,
        normalFont,
        RGB(255, 210, 90)
    );

    drawTextLine(
        hdc,
        statusMessage,
        160,
        492,
        smallFont,
        RGB(235, 240, 250)
    );
}

void createControls(HWND hwnd) {
    titleFont = CreateFontW(
        28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    normalFont = CreateFontW(
        20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    smallFont = CreateFontW(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    displayFont = CreateFontW(
        88, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas"
    );

    minEdit = CreateWindowW(
        L"EDIT",
        L"0",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
        760, 210, 90, 32,
        hwnd,
        (HMENU)ID_MIN_EDIT,
        GetModuleHandleW(NULL),
        NULL
    );

    maxEdit = CreateWindowW(
        L"EDIT",
        L"9",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
        760, 265, 90, 32,
        hwnd,
        (HMENU)ID_MAX_EDIT,
        GetModuleHandleW(NULL),
        NULL
    );

    SendMessageW(minEdit, WM_SETFONT, (WPARAM)normalFont, TRUE);
    SendMessageW(maxEdit, WM_SETFONT, (WPARAM)normalFont, TRUE);

    CreateWindowW(
        L"BUTTON",
        L"+1",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        45, 560, 120, 42,
        hwnd,
        (HMENU)ID_INCREMENT,
        GetModuleHandleW(NULL),
        NULL
    );

    CreateWindowW(
        L"BUTTON",
        L"Скинути",
        WS_VISIBLE | WS_CHILD,
        175, 560, 110, 42,
        hwnd,
        (HMENU)ID_RESET,
        GetModuleHandleW(NULL),
        NULL
    );

    CreateWindowW(
        L"BUTTON",
        L"Встановити діапазон",
        WS_VISIBLE | WS_CHILD,
        295, 560, 180, 42,
        hwnd,
        (HMENU)ID_SET_RANGE,
        GetModuleHandleW(NULL),
        NULL
    );

    CreateWindowW(
        L"BUTTON",
        L"Поточне значення",
        WS_VISIBLE | WS_CHILD,
        485, 560, 160, 42,
        hwnd,
        (HMENU)ID_GET_VALUE,
        GetModuleHandleW(NULL),
        NULL
    );

    autoButton = CreateWindowW(
        L"BUTTON",
        L"Автозапуск",
        WS_VISIBLE | WS_CHILD,
        655, 560, 135, 42,
        hwnd,
        (HMENU)ID_AUTO_RUN,
        GetModuleHandleW(NULL),
        NULL
    );

    CreateWindowW(
        L"BUTTON",
        L"Вийти",
        WS_VISIBLE | WS_CHILD,
        800, 560, 90, 42,
        hwnd,
        (HMENU)ID_EXIT,
        GetModuleHandleW(NULL),
        NULL
    );
}

// =====================================================
// Обробник повідомлень вікна
// =====================================================
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        createControls(hwnd);
        updateWindow(hwnd, L"Лічильник створено з діапазоном [0; 9].");
        return 0;

    case WM_COMMAND: {
        int commandId = LOWORD(wParam);

        if (commandId == ID_INCREMENT) {
            stopAutoRun(hwnd);

            bool wasReset = counter.increment();

            if (wasReset) {
                updateWindow(hwnd, L"Досягнуто максимуму. Наступний крок скинув лічильник до мінімуму.");
            } else {
                updateWindow(hwnd, L"Значення лічильника збільшено на 1.");
            }
        }

        if (commandId == ID_RESET) {
            stopAutoRun(hwnd);
            counter.reset();
            updateWindow(hwnd, L"Лічильник скинуто до мінімального значення.");
        }

        if (commandId == ID_SET_RANGE) {
            stopAutoRun(hwnd);

            int minVal;
            int maxVal;

            if (!readIntFromEdit(minEdit, minVal) || !readIntFromEdit(maxEdit, maxVal)) {
                updateWindow(hwnd, L"Помилка: потрібно ввести коректні цілі числа.");
                return 0;
            }

            if (counter.setRange(minVal, maxVal)) {
                wstring text =
                    L"Новий діапазон встановлено: [" +
                    intToWString(counter.getMinValue()) +
                    L"; " +
                    intToWString(counter.getMaxValue()) +
                    L"]. Поточне значення встановлено на мінімум.";

                updateWindow(hwnd, text);
            } else {
                updateWindow(hwnd, L"Помилка: мінімальне значення не може бути більшим за максимальне.");
            }
        }

        if (commandId == ID_GET_VALUE) {
            wstring text =
                L"Метод getCurrentValue() повернув значення: " +
                intToWString(counter.getCurrentValue());

            updateWindow(hwnd, text);
        }

        if (commandId == ID_AUTO_RUN) {
            if (!autoRunning) {
                autoRunning = true;
                SetWindowTextW(autoButton, L"Зупинити");
                updateWindow(hwnd, L"Автозапуск запущено. Лічильник автоматично збільшується в межах поточного діапазону.");
                SetTimer(hwnd, ID_TIMER, 600, NULL);
            } else {
                stopAutoRun(hwnd);
                updateWindow(hwnd, L"Автозапуск зупинено користувачем.");
            }
        }

        if (commandId == ID_EXIT) {
            DestroyWindow(hwnd);
        }

        return 0;
    }

    case WM_TIMER:
        if (wParam == ID_TIMER) {
            bool wasReset = counter.increment();

            if (wasReset) {
                updateWindow(hwnd, L"Автозапуск: досягнуто максимуму, виконано скидання до мінімуму.");
            } else {
                updateWindow(hwnd, L"Автозапуск: значення збільшено на 1.");
            }
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT paintStruct;
        HDC hdc = BeginPaint(hwnd, &paintStruct);

        drawInterface(hwnd, hdc);

        EndPaint(hwnd, &paintStruct);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER);

        DeleteObject(titleFont);
        DeleteObject(normalFont);
        DeleteObject(displayFont);
        DeleteObject(smallFont);

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// =====================================================
// Точка входу Windows-програми
// =====================================================
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"DigitalCounterTimerWindow";

    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = CLASS_NAME;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&windowClass);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"МКР Рівень 3 — Цифровий лічильник",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        960,
        670,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};

    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}