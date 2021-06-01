#include "saes.h"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <fmt/core.h>
#include <fstream>
#include <vector>
/*
There are different block cipher modes of operations each having their own set of pros & cons.
In this project, I'm using Electronic Code Block (ECB).
Using AES-NI eliminates the need for AES lookup tables, which have been a source of cache-related
and timing side channel vulnerabilities.
*/

constexpr std::size_t ROUNDS = 4; // works for only 4 and 10 rounds

void printBuffer(const std::string& str, const uint8_t* buffer, std::size_t size);

std::vector<uint8_t> readFile();

int main(int argc, char const* argv[]) {

    const uint8_t plainText[] = {0x54, 0x77, 0x6F, 0x20, 0x4F, 0x6E, 0x65, 0x20,
                                 0x4E, 0x69, 0x6E, 0x65, 0x20, 0x54, 0x77, 0x6F};

    const uint8_t key[] = {0x54, 0x68, 0x61, 0x74, 0x73, 0x20, 0x6D, 0x79,
                           0x20, 0x4B, 0x75, 0x6E, 0x67, 0x20, 0x46, 0x75};

    __m128i key_schedule[ROUNDS * 2];

    aes128_load_key<ROUNDS>(key, key_schedule);

    uint8_t decryptedText[16] = {0};
    uint8_t cipherText[16] = {0};
    const auto len = sizeof(plainText);

    log("Encrypting...\n");
    aes128_encrypt<ROUNDS>(plainText, cipherText, len, key_schedule);
    log("Decrypting...\n");
    aes128_decrypt<ROUNDS>(cipherText, decryptedText, len, key_schedule);

    printBuffer("Plain text: \n", plainText, sizeof(plainText));
    log("\n");

    printBuffer("Encrypted text: \n", cipherText, sizeof(cipherText));
    log("\n");

    printBuffer("Decrypted text: \n", decryptedText, sizeof(decryptedText));
    log("\n");

    assert(memcmp(decryptedText, plainText, sizeof plainText) == 0);
    return 0;
}

void printBuffer(const std::string& str, const uint8_t* buffer, std::size_t size) {
    log(str);
    for (std::size_t i = 0; i < size; i++) log("({:#04x})", buffer[i]);
    log("\n");
}