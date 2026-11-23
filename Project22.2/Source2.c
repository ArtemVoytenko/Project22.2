#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <locale.h>
#define MAX 100
#define W 60
#define H 20
typedef double (*TFunc)(double);
typedef struct { double x, y; } Point;
double Y(double x) { return x > 0 ? log(sqrt(x * x * x)) : NAN; }
double V(double x)
{
    double t;
    if (x < 0) return exp(cos(x));
    if (x <= 1) return -2 * x;
    t = tan(x * x - 1);
    return (isfinite(t) && t >= 0) ? sqrt(t) : NAN;
}
double S(double x)
{
    double t;
    if (x == -1) return NAN;
    t = (x - 1) / (x + 1);
    return t + pow(t, 3) / 3 + pow(t, 5) / 5 + pow(t, 7) / 7;
}
TFunc choose(int n)
{
    if (n == 1) return Y;
    if (n == 2) return V;
    if (n == 3) return S;
    return NULL;
}
TFunc select_func(void)
{
    int n; printf("1 - Y(x)  2 - V(x)  3 - S(x): ");
    if (scanf("%d", &n) != 1) return NULL;
    return choose(n);
}
void table(TFunc f, double a, double b, double h)
{
    double x, y; int count = 0; printf("\n       x             f(x)\n"); for (x = a; x <= b + h * 1e-9 && count < MAX; x += h, count++)
    {
        y = f(x);
        if (isfinite(y)) printf("%10.3lf %12.5lf\n", x, y);
        else printf("%10.3lf Не определено\n", x);
        if (x + h <= x) { puts("Шаг слишком мал."); break; }
    }
    if (count == MAX && x <= b + h * 1e-9) puts("Выведены первые 100 точек. Увеличьте шаг для полного интервала.");
}
void print_points(Point p[], int n)
{
    int i;
    for (i = 0; i < n; i++)
        printf("Point[%d]: x=%.3lf, y=%.5lf\n", i, p[i].x, p[i].y);
}
void graph(TFunc f, double a, double b)
{
    char g[H][W]; double y[W], mn = INFINITY, mx = -INFINITY; int i, j;
    for (i = 0; i < W; i++)
    {
        y[i] = f(a + (b - a) * i / (W - 1));
        if (isfinite(y[i]))
        {
            if (y[i] < mn) mn = y[i];
            if (y[i] > mx) mx = y[i];
        }
    }
    if (!isfinite(mn)) { puts("Нет значений для графика."); return; }
    if (mn == mx) { mn--; mx++; }
    for (i = 0; i < H; i++)
        for (j = 0; j < W; j++) g[i][j] = ' ';
    for (i = 0; i < W; i++)
        if (isfinite(y[i]))
        {
            j = (int)((mx - y[i]) * (H - 1) / (mx - mn) + 0.5);
            if (j >= 0 && j < H) g[j][i] = '*';
        }
    for (i = 0; i < H; i++)
    {
        for (j = 0; j < W; j++) putchar(g[i][j]);
        putchar('\n');
    }
}
void operations(TFunc f)
{
    int n, i, count; double a, b, h, x, y;
    do
    {
        printf("\n1-Файл  2-Чтение  3-График  4-Максимум\n"); printf("5-Свойства  6-Point  7-Двумерный массив  8-Размах  0-Назад\nВыбор: "); scanf("%d", &n);
        switch (n)
        {
        case 1:
        {
            FILE* fp = fopen("dat.txt", "w");
            if (!fp) { puts("Не удалось открыть файл."); break; }
            printf("Введите a, b, h: "); if (scanf("%lf%lf%lf", &a, &b, &h) != 3 || !isfinite(a) ||
                !isfinite(b) || !isfinite(h) || a > b || h <= 0)
            {
                fclose(fp); puts("Неверные параметры."); break;
            }
            count = 0;
            for (x = a; x <= b + h * 1e-9; x += h)
            {
                if (count == MAX) { puts("Слишком много точек. Увеличьте шаг."); break; }
                if (count) fprintf(fp, ",");
                fprintf(fp, "%.6lf", x);
                count++;
                if (x + h <= x) { puts("Шаг слишком мал для данного интервала."); break; }
            }
            fclose(fp); printf("В файл dat.txt записано значений: %d\n", count);
            break;
        }
        case 2:
        {
            FILE* fp = fopen("dat.txt", "r");
            if (!fp) { puts("Файл не найден. Сначала выполните операцию 1."); break; }
            count = 0;
            while (fscanf(fp, " %lf", &x) == 1)
            {
                y = f(x);
                if (isfinite(y)) printf("x=%.3lf  f(x)=%.5lf\n", x, y);
                else printf("x=%.3lf  функция не определена\n", x);
                count++;
                while ((i = fgetc(fp)) == ',' || i == ' ' || i == '\n' || i == '\r' || i == '\t') {}
                if (i != EOF) ungetc(i, fp);
            }
            if (!count) puts("В файле нет числовых значений.");
            else printf("Обработано значений: %d\n", count);
            fclose(fp);
            break;
        }
        case 3:
            printf("Введите a и b: "); if (scanf("%lf%lf", &a, &b) != 2 || !isfinite(a) || !isfinite(b) || a >= b)
                puts("Неверный интервал.");
            else graph(f, a, b);
            break;
        case 4:
        {
            double c, d, fc, fd, k = 0.6180339; int ok = 1;
            printf("Введите a и b: "); if (scanf("%lf%lf", &a, &b) != 2 || !isfinite(a) || !isfinite(b) || a >= b)
            {
                puts("Неверный интервал."); break;
            }
            for (i = 0; i < 80; i++)
            {
                c = b - k * (b - a); d = a + k * (b - a); fc = f(c); fd = f(d);
                if (!isfinite(fc) || !isfinite(fd)) { ok = 0; break; }
                if (fc < fd) a = c; else b = d;
            }
            if (ok) { x = (a + b) / 2; printf("Максимум: x=%.5lf, y=%.5lf\n", x, f(x)); }
            else puts("На интервале есть неопределенные значения.");
            break;
        }
        case 5:
        {
            int up = 1, down = 1, pos = 0, neg = 0, zero = 0; int even = 1, odd = 1, valid = 1; double old; printf("Введите a и b: "); scanf("%lf%lf", &a, &b);
            if (a >= b) { puts("Неверный интервал."); break; }
            old = f(a);
            for (i = 0; i <= 100; i++)
            {
                x = a + (b - a) * i / 100.0; y = f(x);
                if (!isfinite(y)) { valid = 0; up = down = 0; continue; }
                if (y > 0) pos = 1;
                if (y < 0) neg = 1;
                if (y == 0) zero = 1;
                if (i && isfinite(old))
                {
                    if (y < old - 1e-8) up = 0;
                    if (y > old + 1e-8) down = 0;
                }
                old = y;
            }
            if (up) puts("Функция возрастает на проверенных точках.");
            else if (down) puts("Функция убывает на проверенных точках."); else puts("Функция немонотонная на проверенных точках.");
            if (pos && neg) puts("Функция знакопеременная.");
            else if (pos) puts("Есть положительные значения."); else if (neg) puts("Есть отрицательные значения.");
            if (zero) puts("Есть нулевые значения.");
            if (fabs(a + b) < 1e-9 && valid)
            {
                for (i = 0; i <= 100; i++)
                {
                    x = a + (b - a) * i / 100.0;
                    if (!isfinite(f(x)) || !isfinite(f(-x))) { even = odd = 0; break; }
                    if (fabs(f(x) - f(-x)) > 1e-6) even = 0;
                    if (fabs(f(x) + f(-x)) > 1e-6) odd = 0;
                }
                if (even) puts("Функция четная.");
                else if (odd) puts("Функция нечетная."); else puts("Функция не четная и не нечетная.");
            }
            else puts("Для проверки четности нужен симметричный интервал без разрывов.");
            break;
        }
        case 6:
        {
            Point p[MAX]; count = 0; printf("Введите a, b, h: "); scanf("%lf%lf%lf", &a, &b, &h);
            if (a > b || h <= 0) { puts("Неверные параметры."); break; }
            for (x = a; x <= b + h * 1e-9 && count < MAX; x += h)
            {
                y = f(x);
                if (isfinite(y)) { p[count].x = x; p[count++].y = y; }
            }
            print_points(p, count);
            break;
        }
        case 7:
        {
            double m[MAX][2]; count = 0; printf("Введите a, b, h: "); scanf("%lf%lf%lf", &a, &b, &h);
            if (a > b || h <= 0) { puts("Неверные параметры."); break; }
            for (x = a; x <= b + h * 1e-9 && count < MAX; x += h)
            {
                m[count][0] = x; m[count++][1] = f(x);
            }
            for (i = 0; i < count; i++)
                printf("x=%.3lf  y=%.5lf\n", m[i][0], m[i][1]);
            break;
        }
        case 8:
        {
            double mn = INFINITY, mx = -INFINITY; printf("Введите a, b, h: "); scanf("%lf%lf%lf", &a, &b, &h);             if (!isfinite(a) || !isfinite(b) || !isfinite(h) || a > b || h <= 0) { puts("Неверные параметры."); break; }
            count = 0; for (x = a; x <= b + h * 1e-9 && count < MAX; x += h, count++)
            {
                y = f(x);
                if (isfinite(y)) { if (y < mn) mn = y; if (y > mx) mx = y; }
                if (x + h <= x) { puts("Шаг слишком мал."); break; }
            }
            if (count == MAX && x <= b + h * 1e-9) puts("Слишком много точек. Увеличьте шаг."); else if (isfinite(mn)) printf("Размах X=%.3lf, размах Y=%.5lf\n", b - a, mx - mn);
            else puts("Нет определенных значений.");
            break;
        }
        case 0: break;
        default: puts("Неверный пункт.");
        }
    } while (n != 0);
}
int Z1(void)
{
    TFunc f = select_func(); double a, b, h;
    if (!f) { puts("Неверная функция."); return 0; }
    printf("Введите a, b, h: "); scanf("%lf%lf%lf", &a, &b, &h);
    if (a <= b && h > 0) table(f, a, b, h);
    else puts("Неверные параметры.");
    return 0;
}
int Z2(void)
{
    int n; TFunc f = NULL; double x, y, a, b, h;
    do
    {
        printf("\n1-Вычислить  2-Табулировать  3-Операция  0-Выход\nВыбор: ");
        if (scanf("%d", &n) != 1) return 0;
        if (n >= 1 && n <= 3)
        {
            f = select_func();
            if (!f) { puts("Неверная функция."); continue; }
        }
        switch (n)
        {
        case 1:
            printf("Введите x: ");
            if (scanf("%lf", &x) != 1 || !isfinite(x)) { puts("Неверное значение x."); break; }
            y = f(x);
            if (isfinite(y)) printf("f(x)=%.6lf\n", y); else puts("Функция не определена.");
            break;
        case 2:
            printf("Введите a, b, h: ");
            if (scanf("%lf%lf%lf", &a, &b, &h) == 3 && isfinite(a) &&
                isfinite(b) && isfinite(h) && a <= b && h > 0) table(f, a, b, h);
            else puts("Неверные параметры.");
            break;
        case 3: operations(f); break;
        case 0: break;
        default: puts("Неверный пункт.");
        }
    } while (n != 0);
    return 0;
}
int calculate(TFunc prtf, double* x, int n)
{
    int i, count = 0;
    for (i = 0; i < n; i++)
    {
        double y = prtf(x[i]);
        if (isfinite(y) && y > 0) count++;
    }
    return count;
}
int Z3(void)
{
    int n = 0, i; double a, b, h, x[MAX]; TFunc f;
    printf("\nВариант 5. Количество положительных значений\n");
    f = select_func();
    if (!f) { puts("Неверная функция."); return 0; }
    printf("Введите a, b, h: ");
    if (scanf("%lf%lf%lf", &a, &b, &h) != 3 || !isfinite(a) ||
        !isfinite(b) || !isfinite(h) || a > b || h <= 0)
    {
        puts("Неверные параметры."); return 0;
    }
    for (i = 0; i < MAX; i++)
    {
        x[i] = a + i * h;
        if (!isfinite(x[i]) || (i > 0 && x[i] <= x[i - 1]))
        {
            puts("Шаг слишком мал или значения вышли за пределы."); return 0;
        }
        if (x[i] > b + h * 1e-9) break;
        n++;
    }
    if (i == MAX && a + MAX * h <= b + h * 1e-9)
    {
        puts("Слишком много точек. Увеличьте шаг."); return 0;
    }
    printf("Всего точек: %d\n", n);
    printf("Количество положительных значений: %d\n", calculate(f, x, n));
    return 0;
}
int main(void)
{
    int n; setlocale(LC_ALL, "ru_RU.UTF-8");
    do
    {
        printf("\n========== ЛАБОРАТОРНАЯ РАБОТА 22 ==========\n"); printf("1. Задание 1 - Табулирование функций\n");         printf("2. Задание 2 - Меню и операции с функциями\n"); printf("3. Задание 3 - Индивидуальное задание (вариант 5)\n");         printf("4. Выход\nВыберите пункт: ");
        scanf("%d", &n);
        switch (n)
        {
        case 1: Z1(); break; case 2: Z2(); break;
        case 3: Z3(); break; case 4: puts("Выход из программы."); break;
        default: puts("Неверный пункт меню.");
        }
    } while (n != 4);
    return 0;
}
