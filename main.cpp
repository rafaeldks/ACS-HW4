#include <iostream>
#include <pthread.h>
#include <fstream>
#include <thread>

using namespace std;

// Вспомогательный класс для вычисления значений функции
class func {
    double a, b, c, d;
public:
    func(double a, double b, double c, double d) : a(a), b(b), c(c), d(d) {}

    [[nodiscard]] double value(double x) const {
        return a * x * x * x + b * x * x + c * x + d;
    }
};

// Глобальные переменные, к которым обращаются потоки
// значения "левого" и "правого" определённого интеграла
double x = 0, y = 0;
// Границы интегрирования
double A, B;
// Число интервалов разбиения
int K = 2;
// Функция f(x)
func f = func(1, 1, 1, 1);

// Потоки
pthread_t first_adder, second_adder;

// Функция приближения определённого интеграла по формуле Симпсона
double simpson(double a, double b, int n) {
    double h = (b - a) / n;
    double first_sum = 0;
    for (int s = 0; s < n - 1; ++s) {
        first_sum += (f.value(a + s * h + h / 2));
    }
    double second_sum = 0;
    for (int s = 1; s < n - 1; ++s) {
        second_sum += (f.value(a + s * h));
    }
    return (h / 6) * (4 * first_sum + 2 * second_sum + f.value(a) + f.value(b));
}

// Процедура для первого потока
void *calculateX(void *param) {
    K *= 2;
    x = simpson(A, B, K / 2);
}

// Процедура для второго потока
void *calculateY(void *param) {
    pthread_join(first_adder, nullptr);
    y = simpson(A, B, K);
}

// Рекурсивная функция адаптивной квадратуры, в которой потоки вместе вычисляют необходимые значения функции Симпсона
double adapt(double a, double b, double eps) {
    A = a;
    B = b;
    pthread_create(&first_adder, nullptr, calculateX, nullptr); // Запуск первого потока
    pthread_create(&second_adder, nullptr, calculateY, nullptr); // Запуск второго потока
    pthread_join(second_adder, nullptr); // К следующей строке кода необходимо, чтобы значения x и y были вычислены
    if (abs(x - y) > eps) return adapt(a, (a + b) / 2, eps) + adapt((a + b) / 2, b, eps);
    return y;
}

// Точка входа
int main(int argc, char *argv[]) {
    double a, b, c, d;
    if (argc >= 2) {
        a = stoi(argv[1]);
        b = stoi(argv[2]);
        c = stoi(argv[3]);
        d = stoi(argv[4]);
    } else {
        cout << "Выберите способ ввода данных:" << endl;
        cout << "1 - файл" << endl;
        cout << "2 - консоль" << endl;
        int action;
        cin >> action;
        if (action == 1) {
            ifstream file;
            file.open("test5.in");
            file >> a >> b >> c >> d;
            file.close();
        }
        if (action == 2) {
            cout << "Введите коэффициенты функции a,b,c,d: ";
            cin >> a >> b >> c >> d;
        }
    }
    f = func(a, b, c, d);
    ofstream out("test5.out");
    double result = 1 - adapt(0, 1, 0.000001);
    out << result;
    cout << endl << "Площадь занимаемых земель: " << result;
    return 0;
}
