#include <iostream>
#include <memory>
#include <string>
#include <limits>

using namespace std;

/* ====== ДЕКОРАТОР: напитки ====== */
class IBeverage {
public:
    virtual string getDescription() const = 0;
    virtual int cost() const = 0; // цена в целых (например, тенге)
    virtual ~IBeverage() {}
};

// Базовые напитки
class Espresso : public IBeverage {
public:
    string getDescription() const override { return "Эспрессо"; }
    int cost() const override { return 500; }
};

class Tea : public IBeverage {
public:
    string getDescription() const override { return "Чай"; }
    int cost() const override { return 300; }
};

class Latte : public IBeverage {
public:
    string getDescription() const override { return "Латте"; }
    int cost() const override { return 700; }
};

class Mocha : public IBeverage {
public:
    string getDescription() const override { return "Мокка"; }
    int cost() const override { return 800; }
};

// Базовый декоратор
class BeverageDecorator : public IBeverage {
protected:
    shared_ptr<IBeverage> drink;
public:
    BeverageDecorator(shared_ptr<IBeverage> d) : drink(d) {}
    string getDescription() const override { return drink->getDescription(); }
    int cost() const override { return drink->cost(); }
};

// Добавки
class Milk : public BeverageDecorator {
public:
    Milk(shared_ptr<IBeverage> d) : BeverageDecorator(d) {}
    string getDescription() const override { return drink->getDescription() + ", молоко"; }
    int cost() const override { return drink->cost() + 100; }
};

class Sugar : public BeverageDecorator {
public:
    Sugar(shared_ptr<IBeverage> d) : BeverageDecorator(d) {}
    string getDescription() const override { return drink->getDescription() + ", сахар"; }
    int cost() const override { return drink->cost() + 20; }
};

class WhippedCream : public BeverageDecorator {
public:
    WhippedCream(shared_ptr<IBeverage> d) : BeverageDecorator(d) {}
    string getDescription() const override { return drink->getDescription() + ", взбитые сливки"; }
    int cost() const override { return drink->cost() + 150; }
};

class Caramel : public BeverageDecorator {
public:
    Caramel(shared_ptr<IBeverage> d) : BeverageDecorator(d) {}
    string getDescription() const override { return drink->getDescription() + ", карамель"; }
    int cost() const override { return drink->cost() + 130; }
};

class Soy : public BeverageDecorator {
public:
    Soy(shared_ptr<IBeverage> d) : BeverageDecorator(d) {}
    string getDescription() const override { return drink->getDescription() + ", соя"; }
    int cost() const override { return drink->cost() + 120; }
};

/* ====== АДАПТЕР: оплата ====== */
class IPaymentProcessor {
public:
    virtual bool processPayment(double amount) = 0;
    virtual ~IPaymentProcessor() {}
};

class PayPalPaymentProcessor : public IPaymentProcessor {
public:
    bool processPayment(double amount) override {
        cout << "[PayPal] Оплата " << amount << " прошла." << endl;
        return true;
    }
};

// Сторонние сервисы
class StripePaymentService {
public:
    bool MakeTransaction(double totalAmount) {
        cout << "[Stripe] Транзакция на " << totalAmount << " выполнена." << endl;
        return true;
    }
};

class BankTransferService {
public:
    string Send(double amount, const string& currency) {
        cout << "[Bank] Перевод " << amount << " " << currency << " отправлен." << endl;
        return "OK";
    }
};

// Адаптеры
class StripePaymentAdapter : public IPaymentProcessor {
    StripePaymentService stripe;
public:
    bool processPayment(double amount) override {
        return stripe.MakeTransaction(amount);
    }
};

class BankTransferAdapter : public IPaymentProcessor {
    BankTransferService bank;
    string currency;
public:
    BankTransferAdapter(const string& c = "KZT") : currency(c) {}
    bool processPayment(double amount) override {
        string status = bank.Send(amount, currency);
        return status == "OK";
    }
};

/* ====== Утилиты для меню ====== */
static void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static int askInt(const string& prompt, int minV, int maxV) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= minV && x <= maxV) {
            clearCin();
            return x;
        }
        cout << "Неверный ввод. Попробуй ещё раз.\n";
        clearCin();
    }
}

int main() {
    cout << "==== Кафе (Декоратор + Адаптер) ====\n";

    // 1) Выбор базового напитка
    cout << "\nВыбери напиток:\n";
    cout << "1) Эспрессо (500)\n";
    cout << "2) Чай (300)\n";
    cout << "3) Латте (700)\n";
    cout << "4) Мокка (800)\n";
    int drinkChoice = askInt("Твой выбор (1-4): ", 1, 4);

    shared_ptr<IBeverage> order;
    if (drinkChoice == 1) order = make_shared<Espresso>();
    else if (drinkChoice == 2) order = make_shared<Tea>();
    else if (drinkChoice == 3) order = make_shared<Latte>();
    else order = make_shared<Mocha>();

    // 2) Добавки — можно добавлять сколько хочешь, пока не выберешь 0
    while (true) {
        cout << "\nДобавки:\n";
        cout << "1) Молоко (+100)\n";
        cout << "2) Сахар (+20)\n";
        cout << "3) Взбитые сливки (+150)\n";
        cout << "4) Карамель (+130)\n";
        cout << "5) Соя (+120)\n";
        cout << "0) Хватит\n";
        int add = askInt("Выбери (0-5): ", 0, 5);
        if (add == 0) break;
        if (add == 1) order = make_shared<Milk>(order);
        else if (add == 2) order = make_shared<Sugar>(order);
        else if (add == 3) order = make_shared<WhippedCream>(order);
        else if (add == 4) order = make_shared<Caramel>(order);
        else if (add == 5) order = make_shared<Soy>(order);
    }

    // Печатаем итог заказа
    cout << "\nТвой заказ: " << order->getDescription() << "\n";
    cout << "Итого к оплате: " << order->cost() << "\n";

    // 3) Выбор метода оплаты
    cout << "\nОплата:\n";
    cout << "1) PayPal\n";
    cout << "2) Stripe (через адаптер)\n";
    cout << "3) Банковский перевод (через адаптер)\n";
    int payChoice = askInt("Твой выбор (1-3): ", 1, 3);

    unique_ptr<IPaymentProcessor> proc;
    if (payChoice == 1) proc = make_unique<PayPalPaymentProcessor>();
    else if (payChoice == 2) proc = make_unique<StripePaymentAdapter>();
    else proc = make_unique<BankTransferAdapter>("KZT");

    bool ok = proc->processPayment(order->cost());
    cout << (ok ? "Статус: Успешно!\n" : "Статус: Ошибка!\n");

    cout << "\nСпасибо! До встречи :)\n";
    return 0;
}