#include <iostream>
#include <vector>
#include "token_stream.h"
std::string src = R"a(
    package main
    func test(){
        var a = 11
        var b = 12
        fmt.Println(a+b)
    }
)a";

int main() {
    std::vector<uint8_t> src_data(src.begin(), src.end());

    auto stream = mygo::TokenStream(std::move(src_data));

    bool is_test = true;
    if (is_test) {
    }
    std::cout << "mygo v0.1, author: lzq, all right reserved" << std::endl;
}
