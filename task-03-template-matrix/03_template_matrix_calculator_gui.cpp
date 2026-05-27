#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;

const int MATRIX_SIZE = 4;

// =====================================================
// Шаблонний клас двовимірного динамічного масиву
// =====================================================
template <typename T>
class DynamicMatrix {
private:
    int rows;
    int cols;
    T** data;

    void allocateMemory() {
        data = new T*[rows];

        for (int i = 0; i < rows; i++) {
            data[i] = new T[cols];
        }
    }

    void freeMemory() {
        if (data != nullptr) {
            for (int i = 0; i < rows; i++) {
                delete[] data[i];
            }

            delete[] data;
            data = nullptr;
        }
    }

public:
    DynamicMatrix() {
        rows = 0;
        cols = 0;
        data = nullptr;
    }

    DynamicMatrix(int rowCount, int colCount) {
        rows = rowCount;
        cols = colCount;
        data = nullptr;

        if (rows > 0 && cols > 0) {
            allocateMemory();

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    data[i][j] = T();
                }
            }
        }
    }

    DynamicMatrix(const DynamicMatrix<T>& other) {
        rows = other.rows;
        cols = other.cols;
        data = nullptr;

        if (rows > 0 && cols > 0) {
            allocateMemory();

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    data[i][j] = other.data[i][j];
                }
            }
        }
    }

    DynamicMatrix<T>& operator=(const DynamicMatrix<T>& other) {
        if (this == &other) {
            return *this;
        }

        freeMemory();

        rows = other.rows;
        cols = other.cols;
        data = nullptr;

        if (rows > 0 && cols > 0) {
            allocateMemory();

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    data[i][j] = other.data[i][j];
                }
            }
        }

        return *this;
    }

    ~DynamicMatrix() {
        freeMemory();
    }

    int getRows() const {
        return rows;
    }

    int getCols() const {
        return cols;
    }

    T* operator[](int row) {
        return data[row];
    }

    const T* operator[](int row) const {
        return data[row];
    }

    template <typename U>
    friend istream& operator>>(istream& input, DynamicMatrix<U>& matrix);

    template <typename U>
    friend ostream& operator<<(ostream& output, const DynamicMatrix<U>& matrix);
};

// =====================================================
// Потокове введення масиву
// =====================================================
template <typename T>
istream& operator>>(istream& input, DynamicMatrix<T>& matrix) {
    for (int i = 0; i < matrix.rows; i++) {
        for (int j = 0; j < matrix.cols; j++) {
            input >> matrix.data[i][j];
        }
    }

    return input;
}

// =====================================================
// Потокове виведення масиву
// =====================================================
template <typename T>
ostream& operator<<(ostream& output, const DynamicMatrix<T>& matrix) {
    for (int i = 0; i < matrix.rows; i++) {
        output << "| ";

        for (int j = 0; j < matrix.cols; j++) {
            output << setw(8) << matrix.data[i][j] << " ";
        }

        output << "|" << endl;
    }

    return output;
}

// =====================================================
// Шаблонний клас копіювання 2D масиву
// =====================================================
template <typename T>
class MatrixCopier {
public:
    static DynamicMatrix<T> copyByRows(const DynamicMatrix<T>& source) {
        DynamicMatrix<T> result(source.getRows(), source.getCols());

        for (int i = 0; i < source.getRows(); i++) {
            for (int j = 0; j < source.getCols(); j++) {
                result[i][j] = source[i][j];
            }
        }

        return result;
    }

    static DynamicMatrix<T> copyByColumns(const DynamicMatrix<T>& source) {
        DynamicMatrix<T> result(source.getRows(), source.getCols());

        for (int j = 0; j < source.getCols(); j++) {
            for (int i = 0; i < source.getRows(); i++) {
                result[i][j] = source[i][j];
            }
        }

        return result;
    }

    static DynamicMatrix<T> copySelectedRow(const DynamicMatrix<T>& source, int rowIndex) {
        DynamicMatrix<T> result(1, source.getCols());

        if (rowIndex < 0 || rowIndex >= source.getRows()) {
            return result;
        }

        for (int j = 0; j < source.getCols(); j++) {
            result[0][j] = source[rowIndex][j];
        }

        return result;
    }

    static DynamicMatrix<T> copySelectedColumn(const DynamicMatrix<T>& source, int columnIndex) {
        DynamicMatrix<T> result(source.getRows(), 1);

        if (columnIndex < 0 || columnIndex >= source.getCols()) {
            return result;
        }

        for (int i = 0; i < source.getRows(); i++) {
            result[i][0] = source[i][columnIndex];
        }

        return result;
    }

    static DynamicMatrix<T> copyMainDiagonalAsColumn(const DynamicMatrix<T>& source) {
        int size = source.getRows();

        if (source.getCols() < size) {
            size = source.getCols();
        }

        DynamicMatrix<T> result(size, 1);

        for (int i = 0; i < size; i++) {
            result[i][0] = source[i][i];
        }

        return result;
    }

    static DynamicMatrix<T> copyMainDiagonalAsMatrix(const DynamicMatrix<T>& source) {
        DynamicMatrix<T> result(source.getRows(), source.getCols());

        int size = source.getRows();

        if (source.getCols() < size) {
            size = source.getCols();
        }

        for (int i = 0; i < size; i++) {
            result[i][i] = source[i][i];
        }

        return result;
    }
};

// =====================================================
// Глобальні змінні інтерфейсу
// =====================================================
HWND sourceEdits[MATRIX_SIZE][MATRIX_SIZE];
HWND resultEdits[MATRIX_SIZE][MATRIX_SIZE];

HWND rowEdit;
HWND columnEdit;
HWND statusLabel;

HFONT titleFont;
HFONT normalFont;
HFONT cellFont;
HFONT smallFont;
HFONT tinyFont;

HBRUSH darkEditBrush;
HBRUSH darkStaticBrush;
HBRUSH resultEditBrush;

const int ID_FILL_DEMO = 101;
const int ID_COPY_ROWS = 102;
const int ID_COPY_COLUMNS = 103;
const int ID_COPY_ROW = 104;
const int ID_COPY_COLUMN = 105;
const int ID_COPY_DIAGONAL = 106;
const int ID_STREAM_IO = 107;
const int ID_CLEAR_RESULT = 108;
const int ID_EXIT = 109;

const int ID_ROW_EDIT = 201;
const int ID_COLUMN_EDIT = 202;

// =====================================================
// Допоміжні функції
// =====================================================
wstring stringToWString(const string& text) {
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
    wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &result[0], sizeNeeded);

    if (!result.empty() && result.back() == L'\0') {
        result.pop_back();
    }

    return result;
}

wstring doubleToWString(double value) {
    wstringstream stream;
    stream << fixed << setprecision(1) << value;
    return stream.str();
}

wstring intToWString(int value) {
    wstringstream stream;
    stream << value;
    return stream.str();
}

bool readDoubleFromEdit(HWND edit, double& result) {
    wchar_t buffer[64];
    GetWindowTextW(edit, buffer, 64);

    try {
        result = stod(buffer);
        return true;
    }
    catch (...) {
        return false;
    }
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

void updateStatus(const wstring& text) {
    SetWindowTextW(statusLabel, text.c_str());
}

void clearResultGrid() {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            SetWindowTextW(resultEdits[i][j], L"");
        }
    }
}

void fillDemoMatrix() {
    int value = 1;

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            SetWindowTextW(sourceEdits[i][j], intToWString(value).c_str());
            value++;
        }
    }

    clearResultGrid();
    updateStatus(L"Демо-матрицю 4x4 заповнено значеннями від 1 до 16.");
}

bool readSourceMatrix(DynamicMatrix<double>& matrix) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            double value;

            if (!readDoubleFromEdit(sourceEdits[i][j], value)) {
                updateStatus(L"Помилка: усі комірки початкової матриці мають містити числа.");
                return false;
            }

            matrix[i][j] = value;
        }
    }

    return true;
}

void showMatrixInResult(const DynamicMatrix<double>& matrix) {
    clearResultGrid();

    for (int i = 0; i < matrix.getRows() && i < MATRIX_SIZE; i++) {
        for (int j = 0; j < matrix.getCols() && j < MATRIX_SIZE; j++) {
            SetWindowTextW(resultEdits[i][j], doubleToWString(matrix[i][j]).c_str());
        }
    }
}

void showDiagonalInResult(const DynamicMatrix<double>& diagonalColumn) {
    clearResultGrid();

    for (int i = 0; i < diagonalColumn.getRows() && i < MATRIX_SIZE; i++) {
        SetWindowTextW(resultEdits[i][i], doubleToWString(diagonalColumn[i][0]).c_str());
    }
}

void showStreamIODemo(HWND hwnd) {
    wstringstream sourceStream;

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            wchar_t buffer[64];
            GetWindowTextW(sourceEdits[i][j], buffer, 64);
            sourceStream << buffer << L" ";
        }
    }

    string inputText;
    wstring wideInput = sourceStream.str();

    int inputSize = WideCharToMultiByte(CP_UTF8, 0, wideInput.c_str(), -1, NULL, 0, NULL, NULL);
    inputText.resize(inputSize);
    WideCharToMultiByte(CP_UTF8, 0, wideInput.c_str(), -1, &inputText[0], inputSize, NULL, NULL);

    stringstream inputStream(inputText);
    DynamicMatrix<double> matrix(MATRIX_SIZE, MATRIX_SIZE);

    inputStream >> matrix;

    stringstream outputStream;
    outputStream << matrix;

    string report =
        "Демонстрація потокового введення/виведення\n\n"
        "1. Дані з GUI-комірок сформовано в потік stringstream.\n"
        "2. Матрицю прочитано через operator>>.\n"
        "3. Матрицю виведено через operator<<.\n\n"
        "Результат operator<<:\n\n" +
        outputStream.str();

    MessageBoxW(
        hwnd,
        stringToWString(report).c_str(),
        L"Потокове введення-виведення operator>> / operator<<",
        MB_OK | MB_ICONINFORMATION
    );

    updateStatus(L"Показано демонстрацію потокового введення та виведення operator>> / operator<<.");
}

void drawTextLine(HDC hdc, const wstring& text, int x, int y, HFONT font, COLORREF color) {
    HFONT oldFont = (HFONT)SelectObject(hdc, font);

    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);

    TextOutW(hdc, x, y, text.c_str(), static_cast<int>(text.length()));

    SelectObject(hdc, oldFont);
}

void drawPanel(HDC hdc, RECT rect, COLORREF fillColor, COLORREF borderColor) {
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

void drawNeonLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void drawCyberBackground(HDC hdc, RECT clientRect) {
    HBRUSH background = CreateSolidBrush(RGB(7, 10, 25));
    FillRect(hdc, &clientRect, background);
    DeleteObject(background);

    for (int y = 20; y < clientRect.bottom; y += 28) {
        for (int x = 18; x < 260; x += 24) {
            COLORREF color = ((x + y) % 3 == 0) ? RGB(0, 180, 255) : RGB(0, 80, 140);
            SetPixel(hdc, x, y, color);
            SetPixel(hdc, x + 1, y, color);
        }
    }

    for (int i = 0; i < 18; i++) {
        int x = 20 + i * 23;
        drawNeonLine(hdc, x, 120 + (i % 4) * 12, x + 80, 120 + (i % 4) * 12, RGB(0, 95, 160));
    }

    drawNeonLine(hdc, 20, 670, 280, 670, RGB(0, 180, 255));
    drawNeonLine(hdc, 280, 670, 330, 620, RGB(0, 180, 255));
    drawNeonLine(hdc, 330, 620, 470, 620, RGB(0, 180, 255));

    drawNeonLine(hdc, 720, 45, 940, 45, RGB(255, 80, 120));
    drawNeonLine(hdc, 790, 70, 920, 70, RGB(0, 200, 255));

    HPEN cyanPen = CreatePen(PS_SOLID, 2, RGB(0, 180, 255));
    HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 70, 120));
    HBRUSH emptyBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

    HPEN oldPen = (HPEN)SelectObject(hdc, cyanPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, emptyBrush);

    Ellipse(hdc, 75, 105, 145, 175);
    Ellipse(hdc, 95, 125, 125, 155);

    SelectObject(hdc, redPen);
    Ellipse(hdc, 245, 150, 295, 200);
    Ellipse(hdc, 870, 585, 930, 645);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(cyanPen);
    DeleteObject(redPen);
}

void drawInterface(HWND hwnd, HDC hdc) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    drawCyberBackground(hdc, clientRect);

    drawTextLine(
        hdc,
        L"МКР Рівень 3 — Завдання 3",
        30,
        20,
        titleFont,
        RGB(125, 220, 255)
    );

    drawTextLine(
        hdc,
        L"Matrix Copy Calculator",
        30,
        58,
        normalFont,
        RGB(0, 230, 255)
    );

    drawTextLine(
        hdc,
        L"DynamicMatrix<T> + MatrixCopier<T>: рядки, стовпці, вибраний рядок, вибраний стовпець, головна діагональ, operator>> / operator<<.",
        30,
        90,
        smallFont,
        RGB(205, 215, 235)
    );

    RECT leftPanel = { 42, 130, 430, 445 };
    RECT rightPanel = { 535, 130, 923, 445 };
    RECT actionPanel = { 42, 470, 923, 645 };

    drawPanel(hdc, leftPanel, RGB(4, 9, 20), RGB(0, 190, 255));
    drawPanel(hdc, rightPanel, RGB(4, 9, 20), RGB(80, 255, 150));
    drawPanel(hdc, actionPanel, RGB(22, 30, 50), RGB(255, 185, 70));

    drawTextLine(hdc, L"SOURCE MATRIX", 155, 148, normalFont, RGB(0, 230, 255));
    drawTextLine(hdc, L"RESULT MATRIX", 650, 148, normalFont, RGB(120, 255, 165));

    drawTextLine(hdc, L"Індекс рядка:", 75, 490, smallFont, RGB(235, 240, 250));
    drawTextLine(hdc, L"Індекс стовпця:", 265, 490, smallFont, RGB(235, 240, 250));

    drawTextLine(
        hdc,
        L"Індекси: 0..3. Діагональ показується саме по діагоналі результату.",
        460,
        490,
        smallFont,
        RGB(190, 200, 215)
    );

    drawTextLine(
        hdc,
        L"STATUS:",
        75,
        605,
        smallFont,
        RGB(255, 220, 90)
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
        15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    tinyFont = CreateFontW(
        13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    cellFont = CreateFontW(
        18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas"
    );

    darkEditBrush = CreateSolidBrush(RGB(5, 12, 24));
    resultEditBrush = CreateSolidBrush(RGB(24, 32, 50));
    darkStaticBrush = CreateSolidBrush(RGB(22, 30, 50));

    int startX = 82;
    int startY = 200;
    int cellWidth = 70;
    int cellHeight = 42;
    int gap = 10;

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            sourceEdits[i][j] = CreateWindowW(
                L"EDIT",
                L"",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
                startX + j * (cellWidth + gap),
                startY + i * (cellHeight + gap),
                cellWidth,
                cellHeight,
                hwnd,
                NULL,
                GetModuleHandleW(NULL),
                NULL
            );

            SendMessageW(sourceEdits[i][j], WM_SETFONT, (WPARAM)cellFont, TRUE);
        }
    }

    int resultX = 575;
    int resultY = 200;

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            resultEdits[i][j] = CreateWindowW(
                L"EDIT",
                L"",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_READONLY,
                resultX + j * (cellWidth + gap),
                resultY + i * (cellHeight + gap),
                cellWidth,
                cellHeight,
                hwnd,
                NULL,
                GetModuleHandleW(NULL),
                NULL
            );

            SendMessageW(resultEdits[i][j], WM_SETFONT, (WPARAM)cellFont, TRUE);
        }
    }

    rowEdit = CreateWindowW(
        L"EDIT",
        L"1",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
        168,
        486,
        55,
        30,
        hwnd,
        (HMENU)ID_ROW_EDIT,
        GetModuleHandleW(NULL),
        NULL
    );

    columnEdit = CreateWindowW(
        L"EDIT",
        L"2",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
        382,
        486,
        55,
        30,
        hwnd,
        (HMENU)ID_COLUMN_EDIT,
        GetModuleHandleW(NULL),
        NULL
    );

    SendMessageW(rowEdit, WM_SETFONT, (WPARAM)cellFont, TRUE);
    SendMessageW(columnEdit, WM_SETFONT, (WPARAM)cellFont, TRUE);

    CreateWindowW(L"BUTTON", L"Демо 1..16", WS_VISIBLE | WS_CHILD, 62, 530, 120, 38, hwnd, (HMENU)ID_FILL_DEMO, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"BUTTON", L"За рядками", WS_VISIBLE | WS_CHILD, 192, 530, 125, 38, hwnd, (HMENU)ID_COPY_ROWS, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"BUTTON", L"За стовпцями", WS_VISIBLE | WS_CHILD, 327, 530, 135, 38, hwnd, (HMENU)ID_COPY_COLUMNS, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"BUTTON", L"Рядок", WS_VISIBLE | WS_CHILD, 472, 530, 95, 38, hwnd, (HMENU)ID_COPY_ROW, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"BUTTON", L"Стовпець", WS_VISIBLE | WS_CHILD, 577, 530, 105, 38, hwnd, (HMENU)ID_COPY_COLUMN, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"BUTTON", L"Діагональ", WS_VISIBLE | WS_CHILD, 692, 530, 110, 38, hwnd, (HMENU)ID_COPY_DIAGONAL, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"BUTTON", L"Потокове I/O", WS_VISIBLE | WS_CHILD, 812, 530, 112, 38, hwnd, (HMENU)ID_STREAM_IO, GetModuleHandleW(NULL), NULL);

    CreateWindowW(L"BUTTON", L"Очистити", WS_VISIBLE | WS_CHILD, 62, 570, 120, 34, hwnd, (HMENU)ID_CLEAR_RESULT, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"BUTTON", L"Вийти", WS_VISIBLE | WS_CHILD, 812, 590, 112, 34, hwnd, (HMENU)ID_EXIT, GetModuleHandleW(NULL), NULL);

    statusLabel = CreateWindowW(
        L"STATIC",
        L"Натисніть «Демо 1..16», або введіть власні числа у початкову матрицю.",
        WS_VISIBLE | WS_CHILD,
        145,
        607,
        640,
        28,
        hwnd,
        NULL,
        GetModuleHandleW(NULL),
        NULL
    );

    SendMessageW(statusLabel, WM_SETFONT, (WPARAM)smallFont, TRUE);
}

// =====================================================
// Обробник вікна
// =====================================================
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        createControls(hwnd);
        fillDemoMatrix();
        return 0;

    case WM_COMMAND: {
        int commandId = LOWORD(wParam);

        if (commandId == ID_FILL_DEMO) {
            fillDemoMatrix();
        }

        if (commandId == ID_COPY_ROWS) {
            DynamicMatrix<double> source(MATRIX_SIZE, MATRIX_SIZE);

            if (readSourceMatrix(source)) {
                DynamicMatrix<double> result = MatrixCopier<double>::copyByRows(source);
                showMatrixInResult(result);
                updateStatus(L"Виконано копіювання всієї матриці за рядками.");
            }
        }

        if (commandId == ID_COPY_COLUMNS) {
            DynamicMatrix<double> source(MATRIX_SIZE, MATRIX_SIZE);

            if (readSourceMatrix(source)) {
                DynamicMatrix<double> result = MatrixCopier<double>::copyByColumns(source);
                showMatrixInResult(result);
                updateStatus(L"Виконано копіювання всієї матриці за стовпцями.");
            }
        }

        if (commandId == ID_COPY_ROW) {
            DynamicMatrix<double> source(MATRIX_SIZE, MATRIX_SIZE);
            int rowIndex;

            if (readSourceMatrix(source) && readIntFromEdit(rowEdit, rowIndex)) {
                if (rowIndex < 0 || rowIndex >= MATRIX_SIZE) {
                    updateStatus(L"Помилка: індекс рядка має бути від 0 до 3.");
                } else {
                    DynamicMatrix<double> result = MatrixCopier<double>::copySelectedRow(source, rowIndex);
                    showMatrixInResult(result);

                    wstring text = L"Скопійовано вибраний рядок з індексом " + intToWString(rowIndex) + L".";
                    updateStatus(text);
                }
            }
        }

        if (commandId == ID_COPY_COLUMN) {
            DynamicMatrix<double> source(MATRIX_SIZE, MATRIX_SIZE);
            int columnIndex;

            if (readSourceMatrix(source) && readIntFromEdit(columnEdit, columnIndex)) {
                if (columnIndex < 0 || columnIndex >= MATRIX_SIZE) {
                    updateStatus(L"Помилка: індекс стовпця має бути від 0 до 3.");
                } else {
                    DynamicMatrix<double> result = MatrixCopier<double>::copySelectedColumn(source, columnIndex);
                    showMatrixInResult(result);

                    wstring text = L"Скопійовано вибраний стовпець з індексом " + intToWString(columnIndex) + L".";
                    updateStatus(text);
                }
            }
        }

        if (commandId == ID_COPY_DIAGONAL) {
            DynamicMatrix<double> source(MATRIX_SIZE, MATRIX_SIZE);

            if (readSourceMatrix(source)) {
                DynamicMatrix<double> diagonalColumn = MatrixCopier<double>::copyMainDiagonalAsColumn(source);
                showDiagonalInResult(diagonalColumn);
                updateStatus(L"Скопійовано головну діагональ і показано її саме по діагоналі результату.");
            }
        }

        if (commandId == ID_STREAM_IO) {
            showStreamIODemo(hwnd);
        }

        if (commandId == ID_CLEAR_RESULT) {
            clearResultGrid();
            updateStatus(L"Поле результату очищено.");
        }

        if (commandId == ID_EXIT) {
            DestroyWindow(hwnd);
        }

        return 0;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;

        SetTextColor(hdcEdit, RGB(120, 245, 255));
        SetBkColor(hdcEdit, RGB(5, 12, 24));

        return (INT_PTR)darkEditBrush;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND control = (HWND)lParam;

        SetTextColor(hdcStatic, RGB(235, 240, 250));
        SetBkMode(hdcStatic, TRANSPARENT);

        for (int i = 0; i < MATRIX_SIZE; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                if (control == resultEdits[i][j]) {
                    SetTextColor(hdcStatic, RGB(235, 245, 255));
                    SetBkColor(hdcStatic, RGB(24, 32, 50));
                    return (INT_PTR)resultEditBrush;
                }
            }
        }

        return (INT_PTR)darkStaticBrush;
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
        DeleteObject(normalFont);
        DeleteObject(cellFont);
        DeleteObject(smallFont);
        DeleteObject(tinyFont);
        DeleteObject(darkEditBrush);
        DeleteObject(resultEditBrush);
        DeleteObject(darkStaticBrush);

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// =====================================================
// Точка входу Windows-програми
// =====================================================
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MatrixCopyCalculatorCyberpunkWindow";

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
        L"МКР Рівень 3 — Matrix Copy Calculator",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        995,
        735,
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