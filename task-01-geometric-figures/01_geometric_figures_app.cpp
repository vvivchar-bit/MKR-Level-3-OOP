#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <limits>

using namespace std;

const int KEY_ENTER = 13;
const int KEY_ESC = 27;
const int KEY_UP = 72;
const int KEY_DOWN = 80;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void clearScreen() {
    system("cls");
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
}

void waitForKey() {
    setColor(8);
    cout << endl << "Натисніть будь-яку клавішу для повернення в меню...";
    setColor(7);
    _getch();
}

bool readDoubleValue(const string& message, double& value) {
    cout << message;
    cin >> value;

    if (cin.fail()) {
        clearInputBuffer();
        setColor(12);
        cout << "Помилка: введено некоректне число." << endl;
        setColor(7);
        return false;
    }

    clearInputBuffer();
    return true;
}

int clampInt(int value, int minValue, int maxValue) {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

// =====================================================
// Абстрактний базовий клас Figure
// =====================================================
class Figure {
protected:
    double centerX;
    double centerY;
    double angle;
    double scale;
    bool visible;

    double initialX;
    double initialY;
    double initialAngle;
    double initialScale;

    void printCommonData() const {
        cout << "Центр фігури: (" << centerX << ", " << centerY << ")" << endl;
        cout << "Кут повороту: " << angle << " градусів" << endl;
        cout << "Масштабний фактор: " << scale << endl;
        cout << "Видимість: " << (visible ? "видима" : "невидима") << endl;
    }

    int getRotationStage() const {
        int normalized = static_cast<int>(angle) % 360;

        if (normalized < 0) {
            normalized += 360;
        }

        if (normalized >= 45 && normalized < 135) {
            return 1; // умовний поворот вправо
        }

        if (normalized >= 135 && normalized < 225) {
            return 2; // умовний поворот на 180 градусів
        }

        if (normalized >= 225 && normalized < 315) {
            return 3; // умовний поворот вліво
        }

        return 0; // початкове положення
    }

    int getScaleLevel() const {
        if (scale < 0.8) {
            return 0; // маленьке зображення
        }

        if (scale < 1.5) {
            return 1; // стандартне зображення
        }

        return 2; // збільшене зображення
    }

    int getVisualIndent() const {
        int baseIndent = 4;
        int shift = static_cast<int>((centerX - initialX) / 2.0);
        return clampInt(baseIndent + shift, 0, 45);
    }

    int getVisualTopOffset() const {
        int shift = static_cast<int>((centerY - initialY) / 3.0);
        return clampInt(shift, 0, 8);
    }

    void drawAsciiArt(const vector<string>& art, int color) const {
        for (int i = 0; i < getVisualTopOffset(); i++) {
            cout << endl;
        }

        int indent = getVisualIndent();

        setColor(color);

        for (const string& line : art) {
            cout << string(indent, ' ') << line << endl;
        }

        setColor(7);
    }

public:
    Figure(double x, double y, double startAngle, double scaleFactor)
        : centerX(x),
          centerY(y),
          angle(startAngle),
          scale(scaleFactor),
          visible(true),
          initialX(x),
          initialY(y),
          initialAngle(startAngle),
          initialScale(scaleFactor) {}

    virtual void draw() const = 0;

    virtual void hide() {
        visible = false;
    }

    virtual void show() {
        visible = true;
    }

    virtual void rotate(double degrees) {
        angle += degrees;
    }

    virtual void move(double dx, double dy) {
        centerX += dx;
        centerY += dy;
    }

    virtual void setScale(double newScale) {
        if (newScale > 0) {
            scale = newScale;
        }
    }

    virtual void reset() {
        centerX = initialX;
        centerY = initialY;
        angle = initialAngle;
        scale = initialScale;
        visible = true;
    }

    virtual string getName() const = 0;

    virtual ~Figure() {}
};

// =====================================================
// Похідний клас TriangleFigure
// =====================================================
class TriangleFigure : public Figure {
private:
    double sideA;
    double sideB;
    double sideC;

    vector<string> createArt() const {
        int rotation = getRotationStage();
        int size = getScaleLevel();

        if (size == 0) {
            if (rotation == 0) return { " /\\ ", "/__\\" };
            if (rotation == 1) return { " /|", "< |", " \\|" };
            if (rotation == 2) return { "\\__/", " \\/ " };
            return { "|\\ ", "| >", "|/ " };
        }

        if (size == 1) {
            if (rotation == 0) return {
                "     /\\     ",
                "    /  \\    ",
                "   /____\\   "
            };

            if (rotation == 1) return {
                "      /|    ",
                "    /  |    ",
                "    \\  |    ",
                "      \\|    "
            };

            if (rotation == 2) return {
                "   \\____/   ",
                "    \\  /    ",
                "     \\/     "
            };

            return {
                "    |\\      ",
                "    |  \\    ",
                "    |  /    ",
                "    |/      "
            };
        }

        if (rotation == 0) return {
            "       /\\       ",
            "      /  \\      ",
            "     /    \\     ",
            "    /      \\    ",
            "   /________\\   "
        };

        if (rotation == 1) return {
            "        /|      ",
            "      /  |      ",
            "    /    |      ",
            "    \\    |      ",
            "      \\  |      ",
            "        \\|      "
        };

        if (rotation == 2) return {
            "   \\________/   ",
            "    \\      /    ",
            "     \\    /     ",
            "      \\  /      ",
            "       \\/       "
        };

        return {
            "      |\\        ",
            "      |  \\      ",
            "      |    \\    ",
            "      |    /    ",
            "      |  /      ",
            "      |/        "
        };
    }

public:
    TriangleFigure(double x, double y, double startAngle, double scaleFactor,
                   double a, double b, double c)
        : Figure(x, y, startAngle, scaleFactor),
          sideA(a),
          sideB(b),
          sideC(c) {}

    string getName() const override {
        return "Трикутник";
    }

    void draw() const override {
        setColor(11);
        cout << "Фігура: " << getName() << endl;
        setColor(7);

        if (!visible) {
            setColor(8);
            cout << "Стан: невидима, тому графічне зображення не виводиться." << endl;
            setColor(7);
            return;
        }

        printCommonData();
        cout << "Сторони: " << sideA << ", " << sideB << ", " << sideC << endl;

        drawAsciiArt(createArt(), 11);
    }
};

// =====================================================
// Похідний клас QuadrilateralFigure
// =====================================================
class QuadrilateralFigure : public Figure {
private:
    double width;
    double height;

    vector<string> createArt() const {
        int rotation = getRotationStage();
        int size = getScaleLevel();

        if (size == 0) {
            if (rotation == 0 || rotation == 2) return {
                "+--+",
                "|  |",
                "+--+"
            };

            if (rotation == 1) return {
                " /-/",
                "/-/ "
            };

            return {
                "\\-\\ ",
                " \\-\\"
            };
        }

        if (size == 1) {
            if (rotation == 0 || rotation == 2) return {
                "   +------+ ",
                "   |      | ",
                "   +------+ "
            };

            if (rotation == 1) return {
                "      /---/ ",
                "     /   /  ",
                "    /---/   "
            };

            return {
                "   \\---\\    ",
                "    \\   \\   ",
                "     \\---\\  "
            };
        }

        if (rotation == 0 || rotation == 2) return {
            "   +------------+ ",
            "   |            | ",
            "   |            | ",
            "   +------------+ "
        };

        if (rotation == 1) return {
            "        /------/ ",
            "       /      /  ",
            "      /      /   ",
            "     /------/    "
        };

        return {
            "   \\------\\       ",
            "    \\      \\      ",
            "     \\      \\     ",
            "      \\------\\    "
        };
    }

public:
    QuadrilateralFigure(double x, double y, double startAngle, double scaleFactor,
                        double w, double h)
        : Figure(x, y, startAngle, scaleFactor),
          width(w),
          height(h) {}

    string getName() const override {
        return "Чотирикутник";
    }

    void draw() const override {
        setColor(10);
        cout << "Фігура: " << getName() << endl;
        setColor(7);

        if (!visible) {
            setColor(8);
            cout << "Стан: невидима, тому графічне зображення не виводиться." << endl;
            setColor(7);
            return;
        }

        printCommonData();
        cout << "Ширина: " << width << ", висота: " << height << endl;

        drawAsciiArt(createArt(), 10);
    }
};

// =====================================================
// Похідний клас PolygonFigure
// =====================================================
class PolygonFigure : public Figure {
private:
    int verticesCount;

    vector<string> createArt() const {
        int rotation = getRotationStage();
        int size = getScaleLevel();

        if (size == 0) {
            if (rotation == 0) return {
                "/--\\",
                "\\__/"
            };

            if (rotation == 1 || rotation == 3) return {
                " /\\ ",
                "<  >",
                " \\/ "
            };

            return {
                "/__\\",
                "\\--/"
            };
        }

        if (size == 1) {
            if (rotation == 0) return {
                "    /----\\   ",
                "   /      \\  ",
                "   \\______/  "
            };

            if (rotation == 1) return {
                "      /\\     ",
                "    /   \\    ",
                "    \\   /    ",
                "      \\/     "
            };

            if (rotation == 2) return {
                "   /------\\  ",
                "   \\      /  ",
                "    \\____/   "
            };

            return {
                "     /\\      ",
                "    /  \\     ",
                "    \\  /     ",
                "     \\/      "
            };
        }

        if (rotation == 0) return {
            "      /--------\\    ",
            "     /          \\   ",
            "    /            \\  ",
            "    \\            /  ",
            "     \\__________/   "
        };

        if (rotation == 1) return {
            "         /\\         ",
            "       /    \\       ",
            "     /        \\     ",
            "     \\        /     ",
            "       \\    /       ",
            "         \\/         "
        };

        if (rotation == 2) return {
            "    /____________\\  ",
            "    \\            /  ",
            "     \\          /   ",
            "      \\________/    "
        };

        return {
            "       /\\           ",
            "      /  \\          ",
            "     /    \\         ",
            "     \\    /         ",
            "      \\  /          ",
            "       \\/           "
        };
    }

public:
    PolygonFigure(double x, double y, double startAngle, double scaleFactor,
                  int vertices)
        : Figure(x, y, startAngle, scaleFactor),
          verticesCount(vertices) {}

    string getName() const override {
        return "Многокутник";
    }

    void draw() const override {
        setColor(14);
        cout << "Фігура: " << getName() << endl;
        setColor(7);

        if (!visible) {
            setColor(8);
            cout << "Стан: невидима, тому графічне зображення не виводиться." << endl;
            setColor(7);
            return;
        }

        printCommonData();
        cout << "Кількість вершин: " << verticesCount << endl;

        drawAsciiArt(createArt(), 14);
    }
};

// =====================================================
// Інтерфейс програми
// =====================================================
void printHeader(const string& selectedFigureName) {
    setColor(11);
    cout << "============================================================" << endl;
    cout << "              МКР Рівень 3 — Завдання 1" << endl;
    cout << "        Абстрактний клас геометричних фігур" << endl;
    cout << "============================================================" << endl;
    setColor(7);

    cout << "Керування: ";
    setColor(14);
    cout << "стрілки ↑ / ↓";
    setColor(7);
    cout << " — вибір пункту, ";
    setColor(14);
    cout << "Enter";
    setColor(7);
    cout << " — виконати, ";
    setColor(14);
    cout << "Esc";
    setColor(7);
    cout << " — вихід" << endl;

    cout << "Обрана фігура: ";
    setColor(10);
    cout << selectedFigureName << endl;
    setColor(7);

    cout << "------------------------------------------------------------" << endl;
}

void printMenu(const vector<string>& menu, int selectedItem) {
    for (int i = 0; i < static_cast<int>(menu.size()); i++) {
        if (i == selectedItem) {
            setColor(240);
            cout << " > " << menu[i] << " " << endl;
            setColor(7);
        } else {
            cout << "   " << menu[i] << endl;
        }
    }

    cout << "------------------------------------------------------------" << endl;
}

void printAllFigures(const vector<Figure*>& figures) {
    setColor(14);
    cout << "Показ усіх фігур. Кількість об'єктів у масиві Figure*: "
         << figures.size() << endl;
    setColor(7);

    cout << "------------------------------------------------------------" << endl;

    for (int i = 0; i < static_cast<int>(figures.size()); i++) {
        setColor(11);
        cout << "Елемент масиву Figure* [" << i << "]" << endl;
        setColor(7);

        figures[i]->draw();

        cout << "------------------------------------------------------------" << endl;
    }
}

void printLateBindingCheck(const vector<Figure*>& figures) {
    setColor(14);
    cout << "Перевірка пізнього зв'язування" << endl;
    setColor(7);

    cout << "У програмі створено масив вказівників на абстрактний клас:" << endl;
    setColor(10);
    cout << "vector<Figure*> figures;" << endl;
    setColor(7);

    cout << "У цей масив записано адреси об'єктів похідних класів:" << endl;
    cout << "TriangleFigure, QuadrilateralFigure, PolygonFigure." << endl;

    cout << endl;
    cout << "Далі викликається однакова команда:" << endl;
    setColor(10);
    cout << "figure->draw();" << endl;
    setColor(7);

    cout << "Але фактично виконується draw() конкретного похідного класу." << endl;
    cout << "------------------------------------------------------------" << endl;

    for (Figure* figure : figures) {
        cout << "Вказівник Figure* посилається на об'єкт класу: ";
        setColor(10);
        cout << figure->getName() << endl;
        setColor(7);

        figure->draw();
        cout << "------------------------------------------------------------" << endl;
    }
}

void printAutoDemo(vector<Figure*>& figures) {
    setColor(14);
    cout << "Автоматична демонстрація всіх вимог завдання" << endl;
    setColor(7);
    cout << "------------------------------------------------------------" << endl;

    cout << "1. Абстрактний клас Figure містить protected-дані:" << endl;
    cout << "   centerX, centerY — координати центра фігури;" << endl;
    cout << "   angle — кут повороту у градусах;" << endl;
    cout << "   scale — масштабний фактор;" << endl;
    cout << "   visible — стан видимості." << endl;

    cout << endl;
    cout << "2. Похідні класи:" << endl;
    cout << "   TriangleFigure — трикутник;" << endl;
    cout << "   QuadrilateralFigure — чотирикутник;" << endl;
    cout << "   PolygonFigure — многокутник." << endl;

    cout << endl;
    cout << "3. Початкове зображення всіх фігур:" << endl;
    cout << "------------------------------------------------------------" << endl;
    printAllFigures(figures);

    cout << endl;
    cout << "4. Викликаємо move(5, 3) для трикутника." << endl;
    figures[0]->move(5, 3);
    figures[0]->draw();

    cout << "------------------------------------------------------------" << endl;
    cout << "5. Викликаємо rotate(80) для чотирикутника." << endl;
    figures[1]->rotate(80);
    figures[1]->draw();

    cout << "------------------------------------------------------------" << endl;
    cout << "6. Викликаємо setScale(2.0) для трикутника." << endl;
    figures[0]->setScale(2.0);
    figures[0]->draw();

    cout << "------------------------------------------------------------" << endl;
    cout << "7. Викликаємо hide() для многокутника." << endl;
    figures[2]->hide();
    figures[2]->draw();

    cout << "------------------------------------------------------------" << endl;
    cout << "8. Перевірка пізнього зв'язування через Figure*:" << endl;
    printLateBindingCheck(figures);
}

int readArrowKey() {
    int key = _getch();

    if (key == 224 || key == 0) {
        key = _getch();
    }

    return key;
}

void resetAllFigures(vector<Figure*>& figures) {
    for (Figure* figure : figures) {
        figure->reset();
    }
}

void showAllFigures(vector<Figure*>& figures) {
    for (Figure* figure : figures) {
        figure->show();
    }
}

// =====================================================
// main()
// =====================================================
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    TriangleFigure triangle(10, 15, 0, 1.0, 3, 4, 5);
    QuadrilateralFigure quadrilateral(20, 10, 15, 1.2, 6, 4);
    PolygonFigure polygonFigure(5, 8, 30, 0.8, 6);

    vector<Figure*> figures;
    figures.push_back(&triangle);
    figures.push_back(&quadrilateral);
    figures.push_back(&polygonFigure);

    int selectedMenuItem = 0;
    int selectedFigureIndex = 0;

    vector<string> menu = {
        "Показати всі фігури",
        "Обрати наступну фігуру",
        "Перемістити обрану фігуру на заданий вектор",
        "Повернути обрану фігуру на заданий кут",
        "Змінити масштаб обраної фігури",
        "Зробити обрану фігуру невидимою",
        "Зробити обрану фігуру видимою",
        "Зробити всі фігури видимими",
        "Перевірити пізнє зв'язування",
        "Автоматична демонстрація всіх вимог",
        "Скинути всі фігури",
        "Вийти"
    };

    string resultMessage = "Програма готова до роботи.";

    bool running = true;

    while (running) {
        clearScreen();

        printHeader(figures[selectedFigureIndex]->getName());
        printMenu(menu, selectedMenuItem);

        setColor(14);
        cout << "Результат останньої дії:" << endl;
        setColor(7);
        cout << resultMessage << endl;
        cout << "------------------------------------------------------------" << endl;

        int key = readArrowKey();

        if (key == KEY_UP) {
            selectedMenuItem--;
            if (selectedMenuItem < 0) {
                selectedMenuItem = static_cast<int>(menu.size()) - 1;
            }
        }
        else if (key == KEY_DOWN) {
            selectedMenuItem++;
            if (selectedMenuItem >= static_cast<int>(menu.size())) {
                selectedMenuItem = 0;
            }
        }
        else if (key == KEY_ESC) {
            running = false;
        }
        else if (key == KEY_ENTER) {
            clearScreen();
            printHeader(figures[selectedFigureIndex]->getName());

            switch (selectedMenuItem) {
            case 0:
                resultMessage = "Виведено всі фігури.";
                printAllFigures(figures);
                waitForKey();
                break;

            case 1:
                selectedFigureIndex++;
                if (selectedFigureIndex >= static_cast<int>(figures.size())) {
                    selectedFigureIndex = 0;
                }
                resultMessage = "Обрано фігуру: " + figures[selectedFigureIndex]->getName();
                break;

            case 2: {
                double dx, dy;

                if (!readDoubleValue("Введіть dx: ", dx)) {
                    waitForKey();
                    break;
                }

                if (!readDoubleValue("Введіть dy: ", dy)) {
                    waitForKey();
                    break;
                }

                figures[selectedFigureIndex]->move(dx, dy);
                resultMessage = "Фігуру \"" + figures[selectedFigureIndex]->getName()
                    + "\" переміщено на заданий вектор.";

                figures[selectedFigureIndex]->draw();
                waitForKey();
                break;
            }

            case 3: {
                double degrees;

                if (!readDoubleValue("Введіть кут повороту в градусах: ", degrees)) {
                    waitForKey();
                    break;
                }

                figures[selectedFigureIndex]->rotate(degrees);
                resultMessage = "Фігуру \"" + figures[selectedFigureIndex]->getName()
                    + "\" повернуто на заданий кут.";

                figures[selectedFigureIndex]->draw();
                waitForKey();
                break;
            }

            case 4: {
                double newScale;

                if (!readDoubleValue("Введіть новий масштабний фактор: ", newScale)) {
                    waitForKey();
                    break;
                }

                if (newScale <= 0) {
                    setColor(12);
                    cout << "Масштаб має бути більшим за 0." << endl;
                    setColor(7);
                    waitForKey();
                    break;
                }

                figures[selectedFigureIndex]->setScale(newScale);
                resultMessage = "Для фігури \"" + figures[selectedFigureIndex]->getName()
                    + "\" змінено масштабний фактор.";

                figures[selectedFigureIndex]->draw();
                waitForKey();
                break;
            }

            case 5:
                figures[selectedFigureIndex]->hide();
                resultMessage = "Фігуру \"" + figures[selectedFigureIndex]->getName()
                    + "\" зроблено невидимою.";
                figures[selectedFigureIndex]->draw();
                waitForKey();
                break;

            case 6:
                figures[selectedFigureIndex]->show();
                resultMessage = "Фігуру \"" + figures[selectedFigureIndex]->getName()
                    + "\" знову зроблено видимою.";
                figures[selectedFigureIndex]->draw();
                waitForKey();
                break;

            case 7:
                showAllFigures(figures);
                resultMessage = "Усі фігури знову зроблено видимими.";
                printAllFigures(figures);
                waitForKey();
                break;

            case 8:
                resultMessage = "Виконано перевірку пізнього зв'язування.";
                printLateBindingCheck(figures);
                waitForKey();
                break;

            case 9:
                resultMessage = "Виконано автоматичну демонстрацію всіх вимог завдання.";
                printAutoDemo(figures);
                waitForKey();
                break;

            case 10:
                resetAllFigures(figures);
                selectedFigureIndex = 0;
                resultMessage = "Стан усіх фігур скинуто до початкового.";
                printAllFigures(figures);
                waitForKey();
                break;

            case 11:
                running = false;
                break;
            }
        }
    }

    clearScreen();
    setColor(10);
    cout << "Програму завершено." << endl;
    setColor(7);

    return 0;
}

