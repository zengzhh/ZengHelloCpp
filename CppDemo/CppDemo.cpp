// CppDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <set>

std::set<std::wstring> WStringSplit(const std::wstring& str, wchar_t delim) {
    std::size_t previous = 0;
    std::size_t current = str.find(delim);
    std::set<std::wstring> elems;
    while (current != std::string::npos) {
        if (current > previous) {
            elems.insert(str.substr(previous, current - previous));
        }
        previous = current + 1;
        current = str.find(delim, previous);
    }
    if (previous != str.size()) {
        elems.insert(str.substr(previous));
    }
    return elems;
}

int main()
{
    std::cout << "Hello World!\n";
}