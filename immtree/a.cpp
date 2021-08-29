#include <stdio.h>
#include <utility>
#include <map>

struct A {
    A() = default;
    A(A &&) {
        printf("A moved\n");
    }
    A(const A &) {
        printf("A copyed\n");
    }
};

void f(A a) {
    A b = std::move(a);
}

int main() {
    printf("%d\n", sizeof(std::map<int, int>::iterator));
    A a;
    A &b = a;
    f(std::move(b));
}
