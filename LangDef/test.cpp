#include <variant>
#include <string>
#include <memory>
#include <iostream>

using String = std::string;

struct Inty {
    int number;

    Inty() : number(0) {}

    Inty(int n) : number(n) {}

    Inty(const Inty& other) : number(other.number) {}

    void update(int n) {
        number = n;
    }

};

struct Stringy {
    String str;

    Stringy() : str("") {}

    Stringy(String s) : str(s) {}

    Stringy(const Stringy& other) : str(other.str) {}

    void update(String s) {
        str = s;
    }

};

using Both = std::variant<
    std::shared_ptr<Inty>, 
    std::shared_ptr<Stringy>
>;

Both& testa(Both& obj) {
    switch (obj.index()) {
        case 0:
            std::get<std::shared_ptr<Inty>>(obj)-> update(12);
            return obj;
        case 1:
            std::get<std::shared_ptr<Stringy>>(obj)-> update("updated");
            return obj;
        default:
            return obj;
    }
}

int main() {
    Both john = std::make_shared<Stringy>("goober");

    switch (1) {
        case 1:
            john = std::make_shared<Inty>(3);
        default:
            ;
    }

    return 0;
}