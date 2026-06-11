#pragma once
#include <utility>

constexpr char XOR_SECRET_KEY[] = "InputYourSecretKeyHere";
constexpr size_t KEY_LENGTH = sizeof(XOR_SECRET_KEY) - 1;

template<size_t Length>
class XorString {
private:
    char data[Length];

public:
    template<size_t... Indices>
    constexpr __forceinline XorString(const char(&str)[Length], std::index_sequence<Indices...>)
        : data{ static_cast<char>(str[Indices] ^ XOR_SECRET_KEY[Indices % KEY_LENGTH])... } {
    }

    __forceinline char* get() {
        for (size_t i = 0; i < Length - 1; ++i) {
            data[i] ^= XOR_SECRET_KEY[i % KEY_LENGTH];
        }
        data[Length - 1] = '\0';
        return data;
    }
};

#define XOR(str) (XorString<sizeof(str)>(str, std::make_index_sequence<sizeof(str)>()).get())