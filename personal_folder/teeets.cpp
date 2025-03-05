#include <iostream>
#include <variant>
#include <memory>
#include <string>

using String = std::string;

class Type1;
class Type2;

using MyVariant = std::variant<std::shared_ptr<Type1>, std::shared_ptr<Type2>>;

class CommonInterface {
    public:
        virtual String _commonFunction() const = 0;  // Pure virtual function
        virtual ~CommonInterface() = default;
};

class Type1 : CommonInterface {
    public:
        MyVariant elem;

        Type1() : elem() {}

        String _commonFunction() const override {
            return "1";
        }
};

class Type2 : CommonInterface {
    public:
        MyVariant elem;

        Type2() : elem() {}

        String _commonFunction() const override {
            return "2";
        }
};

class Type3 : CommonInterface {
    public:
        MyVariant el;
        int butt;

        Type3() : el(), butt() {}

        Type3(MyVariant& e, int b) : el(e), butt(b) {}

        String _commonFunction() const override {
            return "2";
        }
};

int main() {
    return 0;
}
