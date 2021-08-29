#include <stdio.h>
#include <utility>

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
    A a;
    A &b = a;
    f(std::move(b));
}
