#include <windows.h>
#include <string>
#include <sstream>
#include <cstdarg>

using namespace std;

// =====================================================
// 1. Перевантаження функцій.
// Результат записується у перший аргумент і повертається
// через посилання int& або через вказівник int*.
// =====================================================
int& sumOverload(int& result, int a, int b) {
    result = a + b;
    return result;
}

int& sumOverload(int& result, int a, int b, int c) {
    result = a + b + c;
    return result;
}

int& sumOverload(int& result, int a, int b, int c, int d) {
    result = a + b + c + d;
    return result;
}

int* sumOverload(int* result, int a, int b) {
    if (result != nullptr) {
        *result = a + b;
    }

    return result;
}

int* sumOverload(int* result, int a, int b, int c) {
    if (result != nullptr) {
        *result = a + b + c;
    }

    return result;
}

int* sumOverload(int* result, int a, int b, int c, int d) {
    if (result != nullptr) {
        *result = a + b + c + d;
    }

    return result;
}

// =====================================================
// 2. Функції з параметрами за замовчуванням.
// =====================================================
int& sumDefault(int& result, int a, int b = 0, int c = 0, int d = 0) {
    result = a + b + c + d;
    return result;
}

int* sumDefault(int* result, int a, int b = 0, int c = 0, int d = 0) {
    if (result != nullptr) {
        *result = a + b + c + d;
    }

    return result;
}

// =====================================================
// 3. Функції зі змінною кількістю параметрів.
// f(int&, ...) та f(int*, ...)
// =====================================================
int& f(int& result, int count, ...) {
    result = 0;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        result += va_arg(args, int);
    }

    va_end(args);

    return result;
}

int* f(int* result, int count, ...) {
    if (result == nullptr) {
        return result;
    }

    *result = 0;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        *result += va_arg(args, int);
    }

    va_end(args);

    return result;
}

// =====================================================
// GUI
// =====================================================
HWND inputA;
HWND inputB;
HWND inputC;
HWND inputD;

HWND resultReferenceLabel;
HWND resultPointerLabel;
HWND outputBox;
HWND statusLabel;

HFONT titleFont;
HFONT subtitleFont;
HFONT normalFont;
HFONT smallFont;
HFONT monoFont;
HFONT resultFont;
HFONT buttonFont;

HBRUSH editBrush;
HBRUSH outputBrush;
HBRUSH staticBrush;

wstring currentReportText = L"";

const int ID_DEMO_VALUES = 101;
const int ID_OVERLOAD = 102;
const int ID_DEFAULT_PARAMS = 103;
const int ID_VARIADIC = 104;
const int ID_AUTO_DEMO = 105;
const int ID_CLEAR = 106;
const int ID_EXIT = 107;
const int ID_EXPAND_REPORT = 108;

// =====================================================
// Helpers
// =====================================================
wstring intToWString(int value) {
    wstringstream stream;
    stream << value;
    return stream.str();
}

bool readIntFromEdit(HWND edit, int& value) {
    wchar_t buffer[64];
    GetWindowTextW(edit, buffer, 64);

    try {
        value = stoi(buffer);
        return true;
    }
    catch (...) {
        return false;
    }
}

bool readAllValues(int& a, int& b, int& c, int& d) {
    if (!readIntFromEdit(inputA, a) ||
        !readIntFromEdit(inputB, b) ||
        !readIntFromEdit(inputC, c) ||
        !readIntFromEdit(inputD, d)) {
        SetWindowTextW(statusLabel, L"Помилка: усі аргументи мають бути цілими числами.");
        return false;
    }

    return true;
}

void setStatus(const wstring& text) {
    SetWindowTextW(statusLabel, text.c_str());
}

void setOutput(const wstring& text) {
    currentReportText = text;
    SetWindowTextW(outputBox, text.c_str());
}

void setResults(int referenceResult, int pointerResult) {
    wstring referenceText = L"Через посилання int&: " + intToWString(referenceResult);
    wstring pointerText = L"Через вказівник int*: " + intToWString(pointerResult);

    SetWindowTextW(resultReferenceLabel, referenceText.c_str());
    SetWindowTextW(resultPointerLabel, pointerText.c_str());
}

wstring makeHeader(const wstring& title, int a, int b, int c, int d) {
    wstringstream report;

    report << title << L"\r\n";
    report << L"============================================================\r\n";
    report << L"Вхідні аргументи:\r\n";
    report << L"a = " << a << L", b = " << b << L", c = " << c << L", d = " << d << L"\r\n";
    report << L"============================================================\r\n\r\n";

    return report.str();
}

void fillDemoValues() {
    SetWindowTextW(inputA, L"10");
    SetWindowTextW(inputB, L"20");
    SetWindowTextW(inputC, L"30");
    SetWindowTextW(inputD, L"40");

    setResults(0, 0);

    setOutput(
        L"Демо-значення встановлено:\r\n"
        L"a = 10, b = 20, c = 30, d = 40\r\n\r\n"
        L"Оберіть режим обчислення:\r\n"
        L"1. Перевантаження функцій.\r\n"
        L"2. Параметри за замовчуванням.\r\n"
        L"3. Змінна кількість параметрів f(int&, ...).\r\n"
        L"4. Автодемо всіх способів.\r\n\r\n"
        L"Умова: функція знаходить суму аргументів і повертає результат\r\n"
        L"через перший аргумент — через посилання або через вказівник.\r\n\r\n"
        L"У цьому варіанті функції мають тип повернення int& або int*,\r\n"
        L"тому їх можна використовувати у виразах."
    );

    setStatus(L"Демо-значення встановлено. Оберіть режим.");
}

// =====================================================
// Expanded report window
// =====================================================
LRESULT CALLBACK ExpandedReportProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND reportEdit = NULL;

    switch (message) {
    case WM_CREATE: {
        reportEdit = CreateWindowW(
            L"EDIT",
            currentReportText.c_str(),
            WS_VISIBLE | WS_CHILD | WS_BORDER |
            ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY |
            WS_VSCROLL,
            10,
            10,
            960,
            600,
            hwnd,
            NULL,
            GetModuleHandleW(NULL),
            NULL
        );

        SendMessageW(reportEdit, WM_SETFONT, (WPARAM)monoFont, TRUE);
        return 0;
    }

    case WM_SIZE: {
        if (reportEdit != NULL) {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            MoveWindow(reportEdit, 10, 10, width - 20, height - 20, TRUE);
        }
        return 0;
    }

    case WM_DESTROY:
        reportEdit = NULL;
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void showExpandedReport(HWND parent) {
    if (currentReportText.empty()) {
        MessageBoxW(
            parent,
            L"Звіт поки порожній. Спочатку виконайте один із режимів обчислення.",
            L"Розгорнутий звіт",
            MB_OK | MB_ICONINFORMATION
        );
        return;
    }

    const wchar_t REPORT_CLASS_NAME[] = L"ExpandedFunctionSumReportWindow";

    static bool classRegistered = false;

    if (!classRegistered) {
        WNDCLASSW reportClass = {};
        reportClass.lpfnWndProc = ExpandedReportProcedure;
        reportClass.hInstance = GetModuleHandleW(NULL);
        reportClass.lpszClassName = REPORT_CLASS_NAME;
        reportClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        reportClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClassW(&reportClass);
        classRegistered = true;
    }

    HWND reportWindow = CreateWindowExW(
        0,
        REPORT_CLASS_NAME,
        L"Розгорнутий звіт виконання — Function Sum Lab",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        120,
        70,
        1050,
        700,
        parent,
        NULL,
        GetModuleHandleW(NULL),
        NULL
    );

    ShowWindow(reportWindow, SW_SHOW);
    UpdateWindow(reportWindow);
}

// =====================================================
// Demo functions
// =====================================================
void runOverloadDemo() {
    int a, b, c, d;

    if (!readAllValues(a, b, c, d)) {
        return;
    }

    int ref2 = 0;
    int ref3 = 0;
    int ref4 = 0;

    int ptr2 = 0;
    int ptr3 = 0;
    int ptr4 = 0;

    int returnedRef2 = sumOverload(ref2, a, b);
    int returnedRef3 = sumOverload(ref3, a, b, c);
    int returnedRef4 = sumOverload(ref4, a, b, c, d);

    int returnedPtr2 = *sumOverload(&ptr2, a, b);
    int returnedPtr3 = *sumOverload(&ptr3, a, b, c);
    int returnedPtr4 = *sumOverload(&ptr4, a, b, c, d);

    wstringstream report;
    report << makeHeader(L"РЕЖИМ 1. ПЕРЕВАНТАЖЕННЯ ФУНКЦІЙ", a, b, c, d);

    report << L"Варіант повернення через посилання int&:\r\n";
    report << L"int returnedRef2 = sumOverload(result, a, b);       = " << returnedRef2 << L"\r\n";
    report << L"int returnedRef3 = sumOverload(result, a, b, c);    = " << returnedRef3 << L"\r\n";
    report << L"int returnedRef4 = sumOverload(result, a, b, c, d); = " << returnedRef4 << L"\r\n\r\n";

    report << L"Варіант повернення через вказівник int*:\r\n";
    report << L"int returnedPtr2 = *sumOverload(&result, a, b);       = " << returnedPtr2 << L"\r\n";
    report << L"int returnedPtr3 = *sumOverload(&result, a, b, c);    = " << returnedPtr3 << L"\r\n";
    report << L"int returnedPtr4 = *sumOverload(&result, a, b, c, d); = " << returnedPtr4 << L"\r\n\r\n";

    report << L"Пояснення:\r\n";
    report << L"Створено кілька функцій з однаковою назвою sumOverload,\r\n";
    report << L"але з різною кількістю параметрів. Компілятор сам обирає\r\n";
    report << L"потрібну функцію залежно від кількості аргументів.\r\n\r\n";

    report << L"Висновок:\r\n";
    report << L"Функції мають тип повернення int& або int*, тому результат\r\n";
    report << L"можна отримати не тільки через змінений перший аргумент,\r\n";
    report << L"а й через значення виразу виклику функції.\r\n";

    setResults(returnedRef4, returnedPtr4);
    setOutput(report.str());
    setStatus(L"Виконано режим: перевантаження функцій.");
}

void runDefaultParamsDemo() {
    int a, b, c, d;

    if (!readAllValues(a, b, c, d)) {
        return;
    }

    int ref1 = 0;
    int ref2 = 0;
    int ref3 = 0;
    int ref4 = 0;

    int ptr1 = 0;
    int ptr2 = 0;
    int ptr3 = 0;
    int ptr4 = 0;

    int returnedRef1 = sumDefault(ref1, a);
    int returnedRef2 = sumDefault(ref2, a, b);
    int returnedRef3 = sumDefault(ref3, a, b, c);
    int returnedRef4 = sumDefault(ref4, a, b, c, d);

    int returnedPtr1 = *sumDefault(&ptr1, a);
    int returnedPtr2 = *sumDefault(&ptr2, a, b);
    int returnedPtr3 = *sumDefault(&ptr3, a, b, c);
    int returnedPtr4 = *sumDefault(&ptr4, a, b, c, d);

    wstringstream report;
    report << makeHeader(L"РЕЖИМ 2. ПАРАМЕТРИ ЗА ЗАМОВЧУВАННЯМ", a, b, c, d);

    report << L"Через посилання int&:\r\n";
    report << L"int returnedRef1 = sumDefault(result, a);          = " << returnedRef1 << L"\r\n";
    report << L"int returnedRef2 = sumDefault(result, a, b);       = " << returnedRef2 << L"\r\n";
    report << L"int returnedRef3 = sumDefault(result, a, b, c);    = " << returnedRef3 << L"\r\n";
    report << L"int returnedRef4 = sumDefault(result, a, b, c, d); = " << returnedRef4 << L"\r\n\r\n";

    report << L"Через вказівник int*:\r\n";
    report << L"int returnedPtr1 = *sumDefault(&result, a);          = " << returnedPtr1 << L"\r\n";
    report << L"int returnedPtr2 = *sumDefault(&result, a, b);       = " << returnedPtr2 << L"\r\n";
    report << L"int returnedPtr3 = *sumDefault(&result, a, b, c);    = " << returnedPtr3 << L"\r\n";
    report << L"int returnedPtr4 = *sumDefault(&result, a, b, c, d); = " << returnedPtr4 << L"\r\n\r\n";

    report << L"Пояснення:\r\n";
    report << L"Одна функція може викликатися з різною кількістю аргументів,\r\n";
    report << L"тому що b, c, d мають значення за замовчуванням 0.\r\n\r\n";

    report << L"Висновок:\r\n";
    report << L"Функції sumDefault повертають int& або int*, тому виклик\r\n";
    report << L"можна використовувати у виразах.\r\n";

    setResults(returnedRef4, returnedPtr4);
    setOutput(report.str());
    setStatus(L"Виконано режим: параметри за замовчуванням.");
}

void runVariadicDemo() {
    int a, b, c, d;

    if (!readAllValues(a, b, c, d)) {
        return;
    }

    int ref2 = 0;
    int ref3 = 0;
    int ref4 = 0;

    int ptr2 = 0;
    int ptr3 = 0;
    int ptr4 = 0;

    int returnedRef2 = f(ref2, 2, a, b);
    int returnedRef3 = f(ref3, 3, a, b, c);
    int returnedRef4 = f(ref4, 4, a, b, c, d);

    int returnedPtr2 = *f(&ptr2, 2, a, b);
    int returnedPtr3 = *f(&ptr3, 3, a, b, c);
    int returnedPtr4 = *f(&ptr4, 4, a, b, c, d);

    wstringstream report;
    report << makeHeader(L"РЕЖИМ 3. ЗМІННА КІЛЬКІСТЬ ПАРАМЕТРІВ", a, b, c, d);

    report << L"Через посилання int&:\r\n";
    report << L"int returnedRef2 = f(result, 2, a, b);       = " << returnedRef2 << L"\r\n";
    report << L"int returnedRef3 = f(result, 3, a, b, c);    = " << returnedRef3 << L"\r\n";
    report << L"int returnedRef4 = f(result, 4, a, b, c, d); = " << returnedRef4 << L"\r\n\r\n";

    report << L"Через вказівник int*:\r\n";
    report << L"int returnedPtr2 = *f(&result, 2, a, b);       = " << returnedPtr2 << L"\r\n";
    report << L"int returnedPtr3 = *f(&result, 3, a, b, c);    = " << returnedPtr3 << L"\r\n";
    report << L"int returnedPtr4 = *f(&result, 4, a, b, c, d); = " << returnedPtr4 << L"\r\n\r\n";

    report << L"Пояснення:\r\n";
    report << L"Функція f використовує va_list, va_start, va_arg, va_end.\r\n";
    report << L"Параметр count показує, скільки наступних аргументів\r\n";
    report << L"потрібно просумувати.\r\n\r\n";

    report << L"Висновок:\r\n";
    report << L"Реалізовано f(int&, ...) та f(int*, ...). Обидві функції\r\n";
    report << L"повертають результат через тип int& або int*.\r\n";

    setResults(returnedRef4, returnedPtr4);
    setOutput(report.str());
    setStatus(L"Виконано режим: змінна кількість параметрів.");
}

void runAutoDemo() {
    int a, b, c, d;

    if (!readAllValues(a, b, c, d)) {
        return;
    }

    int overloadRef = 0;
    int overloadPtr = 0;
    int defaultRef = 0;
    int defaultPtr = 0;
    int variadicRef = 0;
    int variadicPtr = 0;

    int returnedOverloadRef = sumOverload(overloadRef, a, b, c, d);
    int returnedOverloadPtr = *sumOverload(&overloadPtr, a, b, c, d);

    int returnedDefaultRef = sumDefault(defaultRef, a, b, c, d);
    int returnedDefaultPtr = *sumDefault(&defaultPtr, a, b, c, d);

    int returnedVariadicRef = f(variadicRef, 4, a, b, c, d);
    int returnedVariadicPtr = *f(&variadicPtr, 4, a, b, c, d);

    wstringstream report;
    report << makeHeader(L"АВТОДЕМО. ПОРІВНЯННЯ ВСІХ СПОСОБІВ", a, b, c, d);

    report << L"1. Перевантаження функцій:\r\n";
    report << L"   int& повернув: " << returnedOverloadRef << L"\r\n";
    report << L"   int* повернув: " << returnedOverloadPtr << L"\r\n\r\n";

    report << L"2. Параметри за замовчуванням:\r\n";
    report << L"   int& повернув: " << returnedDefaultRef << L"\r\n";
    report << L"   int* повернув: " << returnedDefaultPtr << L"\r\n\r\n";

    report << L"3. Змінна кількість параметрів:\r\n";
    report << L"   f(int&, ...) повернула: " << returnedVariadicRef << L"\r\n";
    report << L"   f(int*, ...) повернула: " << returnedVariadicPtr << L"\r\n\r\n";

    report << L"Контрольний висновок:\r\n";
    report << L"Усі три способи дали однакову суму.\r\n";
    report << L"У кожному випадку функція має тип повернення int& або int*,\r\n";
    report << L"тому результат можна отримувати через посилання або через вказівник.\r\n";

    setResults(returnedVariadicRef, returnedVariadicPtr);
    setOutput(report.str());
    setStatus(L"Автодемо виконано: перевірено всі способи.");
}

void clearOutput() {
    setResults(0, 0);
    SetWindowTextW(outputBox, L"");
    currentReportText = L"";
    setStatus(L"Результати очищено.");
}

// =====================================================
// Drawing
// =====================================================
void drawTextLine(HDC hdc, const wstring& text, int x, int y, HFONT font, COLORREF color) {
    HFONT oldFont = (HFONT)SelectObject(hdc, font);

    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);

    TextOutW(hdc, x, y, text.c_str(), static_cast<int>(text.length()));

    SelectObject(hdc, oldFont);
}

void drawPanel(HDC hdc, RECT rect, COLORREF fillColor, COLORREF borderColor, int radius = 22) {
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN pen = CreatePen(PS_SOLID, 2, borderColor);

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(brush);
    DeleteObject(pen);
}

void drawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color, int width = 1) {
    HPEN pen = CreatePen(PS_SOLID, width, color);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void drawMethodIcon(HDC hdc, int x, int y, int type) {
    RECT box = { x, y, x + 82, y + 72 };
    drawPanel(hdc, box, RGB(9, 86, 150), RGB(230, 250, 255), 16);

    HPEN whitePen = CreatePen(PS_SOLID, 3, RGB(235, 250, 255));
    HPEN cyanPen = CreatePen(PS_SOLID, 3, RGB(80, 230, 255));
    HBRUSH emptyBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

    HPEN oldPen = (HPEN)SelectObject(hdc, whitePen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, emptyBrush);

    if (type == 1) {
        Rectangle(hdc, x + 18, y + 14, x + 64, y + 26);
        Rectangle(hdc, x + 18, y + 31, x + 64, y + 43);
        Rectangle(hdc, x + 18, y + 48, x + 64, y + 60);
        SelectObject(hdc, cyanPen);
        drawLine(hdc, x + 12, y + 20, x + 18, y + 20, RGB(80, 230, 255), 3);
        drawLine(hdc, x + 64, y + 20, x + 70, y + 20, RGB(80, 230, 255), 3);
    }
    else if (type == 2) {
        Ellipse(hdc, x + 20, y + 15, x + 62, y + 57);
        Ellipse(hdc, x + 31, y + 26, x + 51, y + 46);
        SelectObject(hdc, cyanPen);
        drawLine(hdc, x + 41, y + 8, x + 41, y + 18, RGB(80, 230, 255), 3);
        drawLine(hdc, x + 41, y + 54, x + 41, y + 64, RGB(80, 230, 255), 3);
        drawLine(hdc, x + 13, y + 36, x + 23, y + 36, RGB(80, 230, 255), 3);
        drawLine(hdc, x + 59, y + 36, x + 69, y + 36, RGB(80, 230, 255), 3);
    }
    else if (type == 3) {
        SelectObject(hdc, cyanPen);
        Arc(hdc, x + 10, y + 15, x + 32, y + 58, x + 28, y + 15, x + 28, y + 58);
        Arc(hdc, x + 50, y + 15, x + 72, y + 58, x + 54, y + 58, x + 54, y + 15);
        SelectObject(hdc, whitePen);
        Ellipse(hdc, x + 27, y + 32, x + 33, y + 38);
        Ellipse(hdc, x + 38, y + 32, x + 44, y + 38);
        Ellipse(hdc, x + 49, y + 32, x + 55, y + 38);
    }
    else {
        SelectObject(hdc, cyanPen);
        Ellipse(hdc, x + 18, y + 15, x + 64, y + 61);
        SelectObject(hdc, whitePen);
        drawLine(hdc, x + 26, y + 38, x + 38, y + 50, RGB(235, 250, 255), 4);
        drawLine(hdc, x + 38, y + 50, x + 58, y + 25, RGB(235, 250, 255), 4);
    }

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(whitePen);
    DeleteObject(cyanPen);
}

void drawBlueprintBackground(HDC hdc, RECT clientRect) {
    for (int y = 0; y < clientRect.bottom; y++) {
        int green = 145 + y / 18;
        int blue = 205 + y / 24;

        if (green > 205) {
            green = 205;
        }

        if (blue > 245) {
            blue = 245;
        }

        HBRUSH rowBrush = CreateSolidBrush(RGB(14, green, blue));
        RECT row = { 0, y, clientRect.right, y + 1 };
        FillRect(hdc, &row, rowBrush);
        DeleteObject(rowBrush);
    }

    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(120, 220, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

    for (int x = 30; x < clientRect.right; x += 38) {
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, clientRect.bottom);
    }

    for (int y = 30; y < clientRect.bottom; y += 38) {
        MoveToEx(hdc, 0, y, NULL);
        LineTo(hdc, clientRect.right, y);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
}

void drawInterface(HWND hwnd, HDC hdc) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    drawBlueprintBackground(hdc, clientRect);

    RECT appFrame = { 35, 35, 1245, 765 };
    drawPanel(hdc, appFrame, RGB(13, 105, 178), RGB(235, 250, 255), 28);

    RECT header = { 60, 60, 1220, 145 };
    RECT inputPanel = { 60, 165, 380, 405 };
    RECT methodPanel = { 405, 165, 705, 405 };
    RECT resultPanel = { 730, 165, 1220, 405 };
    RECT reportPanel = { 60, 430, 1220, 700 };

    drawPanel(hdc, header, RGB(8, 86, 155), RGB(190, 240, 255), 20);
    drawPanel(hdc, inputPanel, RGB(7, 75, 140), RGB(235, 250, 255), 18);
    drawPanel(hdc, methodPanel, RGB(7, 75, 140), RGB(235, 250, 255), 18);
    drawPanel(hdc, resultPanel, RGB(7, 75, 140), RGB(235, 250, 255), 18);
    drawPanel(hdc, reportPanel, RGB(6, 60, 118), RGB(235, 250, 255), 18);

    drawTextLine(hdc, L"МКР Рівень 3 — Завдання 4", 85, 78, titleFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"Function Sum Lab", 85, 112, subtitleFont, RGB(210, 250, 255));
    drawTextLine(hdc, L"Функції повертають результат через тип int& або int*, тому виклик можна використовувати у виразах.", 470, 98, smallFont, RGB(235, 250, 255));

    drawTextLine(hdc, L"АРГУМЕНТИ", 165, 185, normalFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"РЕЖИМИ", 510, 185, normalFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"РОЗГОРНУТИЙ РЕЗУЛЬТАТ", 865, 185, normalFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"ДЕТАЛЬНИЙ ЗВІТ ВИКОНАННЯ", 485, 450, normalFont, RGB(235, 250, 255));

    drawTextLine(hdc, L"a", 95, 245, normalFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"b", 175, 245, normalFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"c", 255, 245, normalFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"d", 95, 320, normalFont, RGB(235, 250, 255));

    drawMethodIcon(hdc, 430, 225, 1);
    drawTextLine(hdc, L"Overload", 525, 242, smallFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"int& / int*", 525, 265, smallFont, RGB(235, 250, 255));

    drawMethodIcon(hdc, 430, 315, 2);
    drawTextLine(hdc, L"Default", 525, 332, smallFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"b,c,d = 0", 525, 355, smallFont, RGB(235, 250, 255));

    drawMethodIcon(hdc, 600, 225, 3);
    drawTextLine(hdc, L"Variadic", 605, 315, smallFont, RGB(235, 250, 255));
    drawTextLine(hdc, L"f(int&, ...)", 605, 338, smallFont, RGB(235, 250, 255));

    drawMethodIcon(hdc, 600, 315, 4);
}

// =====================================================
// Controls
// =====================================================
HWND createButton(HWND hwnd, const wchar_t* text, int x, int y, int w, int h, int id) {
    HWND button = CreateWindowW(
        L"BUTTON",
        text,
        WS_VISIBLE | WS_CHILD,
        x,
        y,
        w,
        h,
        hwnd,
        (HMENU)id,
        GetModuleHandleW(NULL),
        NULL
    );

    SendMessageW(button, WM_SETFONT, (WPARAM)buttonFont, TRUE);
    return button;
}

void createControls(HWND hwnd) {
    titleFont = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    subtitleFont = CreateFontW(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    normalFont = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    smallFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    buttonFont = CreateFontW(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    // Ось тут шрифт звіту зроблено 17
    monoFont = CreateFontW(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas");

    resultFont = CreateFontW(19, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    editBrush = CreateSolidBrush(RGB(225, 248, 255));
    outputBrush = CreateSolidBrush(RGB(225, 248, 255));
    staticBrush = CreateSolidBrush(RGB(7, 75, 140));

    inputA = CreateWindowW(L"EDIT", L"10", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER, 120, 275, 60, 34, hwnd, NULL, GetModuleHandleW(NULL), NULL);
    inputB = CreateWindowW(L"EDIT", L"20", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER, 200, 275, 60, 34, hwnd, NULL, GetModuleHandleW(NULL), NULL);
    inputC = CreateWindowW(L"EDIT", L"30", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER, 280, 275, 60, 34, hwnd, NULL, GetModuleHandleW(NULL), NULL);
    inputD = CreateWindowW(L"EDIT", L"40", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER, 120, 350, 60, 34, hwnd, NULL, GetModuleHandleW(NULL), NULL);

    SendMessageW(inputA, WM_SETFONT, (WPARAM)resultFont, TRUE);
    SendMessageW(inputB, WM_SETFONT, (WPARAM)resultFont, TRUE);
    SendMessageW(inputC, WM_SETFONT, (WPARAM)resultFont, TRUE);
    SendMessageW(inputD, WM_SETFONT, (WPARAM)resultFont, TRUE);

    resultReferenceLabel = CreateWindowW(
        L"STATIC",
        L"Через посилання int&: 0",
        WS_VISIBLE | WS_CHILD,
        770,
        245,
        390,
        38,
        hwnd,
        NULL,
        GetModuleHandleW(NULL),
        NULL
    );

    resultPointerLabel = CreateWindowW(
        L"STATIC",
        L"Через вказівник int*: 0",
        WS_VISIBLE | WS_CHILD,
        770,
        305,
        390,
        38,
        hwnd,
        NULL,
        GetModuleHandleW(NULL),
        NULL
    );

    SendMessageW(resultReferenceLabel, WM_SETFONT, (WPARAM)resultFont, TRUE);
    SendMessageW(resultPointerLabel, WM_SETFONT, (WPARAM)resultFont, TRUE);

    outputBox = CreateWindowW(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER |
        ES_MULTILINE | ES_AUTOVSCROLL |
        WS_VSCROLL | ES_READONLY,
        90,
        485,
        1100,
        185,
        hwnd,
        NULL,
        GetModuleHandleW(NULL),
        NULL
    );

    SendMessageW(outputBox, WM_SETFONT, (WPARAM)monoFont, TRUE);

    statusLabel = CreateWindowW(
        L"STATIC",
        L"Готово до роботи. Оберіть режим обчислення.",
        WS_VISIBLE | WS_CHILD,
        90,
        707,
        750,
        28,
        hwnd,
        NULL,
        GetModuleHandleW(NULL),
        NULL
    );

    SendMessageW(statusLabel, WM_SETFONT, (WPARAM)smallFont, TRUE);

    createButton(hwnd, L"Демо числа", 900, 700, 120, 34, ID_DEMO_VALUES);
    createButton(hwnd, L"Розгорнути звіт", 1030, 700, 160, 34, ID_EXPAND_REPORT);

    createButton(hwnd, L"Перевантаження", 70, 790, 155, 38, ID_OVERLOAD);
    createButton(hwnd, L"За замовчуванням", 235, 790, 170, 38, ID_DEFAULT_PARAMS);
    createButton(hwnd, L"Змінні параметри", 415, 790, 170, 38, ID_VARIADIC);
    createButton(hwnd, L"Автодемо", 595, 790, 120, 38, ID_AUTO_DEMO);
    createButton(hwnd, L"Очистити", 725, 790, 110, 38, ID_CLEAR);
    createButton(hwnd, L"Вийти", 845, 790, 90, 38, ID_EXIT);

    fillDemoValues();
}

// =====================================================
// Window procedure
// =====================================================
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        createControls(hwnd);
        return 0;

    case WM_COMMAND: {
        int commandId = LOWORD(wParam);

        if (commandId == ID_DEMO_VALUES) {
            fillDemoValues();
        }

        if (commandId == ID_OVERLOAD) {
            runOverloadDemo();
        }

        if (commandId == ID_DEFAULT_PARAMS) {
            runDefaultParamsDemo();
        }

        if (commandId == ID_VARIADIC) {
            runVariadicDemo();
        }

        if (commandId == ID_AUTO_DEMO) {
            runAutoDemo();
        }

        if (commandId == ID_EXPAND_REPORT) {
            showExpandedReport(hwnd);
        }

        if (commandId == ID_CLEAR) {
            clearOutput();
        }

        if (commandId == ID_EXIT) {
            DestroyWindow(hwnd);
        }

        return 0;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;

        SetTextColor(hdcEdit, RGB(0, 55, 95));
        SetBkColor(hdcEdit, RGB(225, 248, 255));

        return (INT_PTR)editBrush;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;

        SetTextColor(hdcStatic, RGB(235, 250, 255));
        SetBkMode(hdcStatic, TRANSPARENT);

        return (INT_PTR)staticBrush;
    }

    case WM_PAINT: {
        PAINTSTRUCT paintStruct;
        HDC hdc = BeginPaint(hwnd, &paintStruct);

        drawInterface(hwnd, hdc);

        EndPaint(hwnd, &paintStruct);
        return 0;
    }

    case WM_DESTROY:
        DeleteObject(titleFont);
        DeleteObject(subtitleFont);
        DeleteObject(normalFont);
        DeleteObject(smallFont);
        DeleteObject(monoFont);
        DeleteObject(resultFont);
        DeleteObject(buttonFont);
        DeleteObject(editBrush);
        DeleteObject(outputBrush);
        DeleteObject(staticBrush);

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// =====================================================
// Entry point
// =====================================================
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"FunctionSumLabWideDashboardWindow";

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
        L"МКР Рівень 3 — Function Sum Lab",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1300,
        900,
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