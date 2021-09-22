#include <string>
#include <iostream>

int to_int(char c) {
    return '0' <= c && c <= '9' ? c - '0' :
            'a' <= c && c <= 'z' ? c - 'a' + 10:
            'A' <= c && c <= 'Z' ? c - 'Z' + 10: -1;
}

char to_char(int x) {
    return x <= 9 ? '0' + x : 'a' + (x - 10);
}

std::string convert(std::string input, int input_radix, int output_radix) {
    std::string output;
    for (int i = 0, n = input.size(), v = 0; i < n;) {
        if (input[i] == '0') {
            ++i;
            continue;
        }
        for (int j = i; j < n; ++j) {
            v *= input_radix;
            v += to_int(input[j]);
            input[j] = to_char(v / output_radix);
            v %= output_radix;
        }
        output.push_back(to_char(v));
        v = 0;
    }
    if (output.empty()) {
        return "0";
    }
    else {
        return std::string(output.rbegin(), output.rend());
    }
}


int main() {
    std::string hex, dec;
    std::cout << "> ";
    while (std::cin >> hex) {
        dec = convert(hex, 10, 16);
        std::cout << dec << std::endl;
        std::cout << "> ";
    }
}