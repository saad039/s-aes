#if !defined(__SAES__)
#define __SAES__

#if defined __has_include
#if __has_include(<wmmintrin.h>)
#include <wmmintrin.h>
#pragma message "using aes-ni instructions"
#endif
#else
#error Compiler Not Supported.
#endif

#include <cstdint>
#include <cstring>
template <typename Str, typename... Args>
void log(const Str& str, Args&&... args) {
    fmt::print(str, std::forward<Args>(args)...);
}

// void printKey(__m128i reg)
// {
//     uint8_t vals[16];
//     memcpy(vals, &reg, sizeof(vals));
//     for (int i = 0; i < 16; i++)
//         log("{:#04x},", vals[i]);

//     log("\n");
// }

#define AES_128_key_exp(k, rcon) aes_128_key_expansion(k, _mm_aeskeygenassist_si128(k, rcon))

__m128i aes_128_key_expansion(__m128i key, __m128i keygened) {
    keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3, 3, 3, 3));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    return _mm_xor_si128(key, keygened);
}

template <std::size_t rounds>
void aes128_load_key(const uint8_t* enc_key, __m128i* key_schedule) {
    key_schedule[0] = _mm_loadu_si128((const __m128i*)enc_key);

    if constexpr (rounds == 4) {
        // key schedule for encryption
        key_schedule[1] = AES_128_key_exp(key_schedule[0], 0x01);
        key_schedule[2] = AES_128_key_exp(key_schedule[1], 0x02);
        key_schedule[3] = AES_128_key_exp(key_schedule[2], 0x04);
        key_schedule[4] = AES_128_key_exp(key_schedule[3], 0x08);

        // key schedule for decryption
        key_schedule[5] = _mm_aesimc_si128(key_schedule[3]);
        key_schedule[6] = _mm_aesimc_si128(key_schedule[2]);
        key_schedule[7] = _mm_aesimc_si128(key_schedule[1]);
    } else // incase of 10 rounds
    {
        // key schedule for encryption
        key_schedule[1] = AES_128_key_exp(key_schedule[0], 0x01);
        key_schedule[2] = AES_128_key_exp(key_schedule[1], 0x02);
        key_schedule[3] = AES_128_key_exp(key_schedule[2], 0x04);
        key_schedule[4] = AES_128_key_exp(key_schedule[3], 0x08);
        key_schedule[5] = AES_128_key_exp(key_schedule[4], 0x10);
        key_schedule[6] = AES_128_key_exp(key_schedule[5], 0x20);
        key_schedule[7] = AES_128_key_exp(key_schedule[6], 0x40);
        key_schedule[8] = AES_128_key_exp(key_schedule[7], 0x80);
        key_schedule[9] = AES_128_key_exp(key_schedule[8], 0x1B);
        key_schedule[10] = AES_128_key_exp(key_schedule[9], 0x36);

        // key schedule for decryption
        key_schedule[11] = _mm_aesimc_si128(key_schedule[9]);
        key_schedule[12] = _mm_aesimc_si128(key_schedule[8]);
        key_schedule[13] = _mm_aesimc_si128(key_schedule[7]);
        key_schedule[14] = _mm_aesimc_si128(key_schedule[6]);
        key_schedule[15] = _mm_aesimc_si128(key_schedule[5]);
        key_schedule[16] = _mm_aesimc_si128(key_schedule[4]);
        key_schedule[17] = _mm_aesimc_si128(key_schedule[3]);
        key_schedule[18] = _mm_aesimc_si128(key_schedule[2]);
        key_schedule[19] = _mm_aesimc_si128(key_schedule[1]);
    }
}

template <std::size_t rounds>
void aes128_encrypt(const uint8_t* plainText, uint8_t* out, size_t length, __m128i* key_schedule) {
    __m128i temp;
    if (length % 16)
        length = length / 16 + 1;
    else
        length = length / 16;

    for (int i = 0; i < length; i++) {

        temp = _mm_loadu_si128(&((__m128i*)plainText)[i]);
        temp = _mm_xor_si128(temp, key_schedule[0]);
        if constexpr (rounds == 4) {
            temp = _mm_aesenc_si128(temp, key_schedule[1]);
            temp = _mm_aesenc_si128(temp, key_schedule[2]);
            temp = _mm_aesenc_si128(temp, key_schedule[3]);
            temp = _mm_aesenclast_si128(temp, key_schedule[4]);
        } else {
            temp = _mm_aesenc_si128(temp, key_schedule[1]);
            temp = _mm_aesenc_si128(temp, key_schedule[2]);
            temp = _mm_aesenc_si128(temp, key_schedule[3]);
            temp = _mm_aesenc_si128(temp, key_schedule[4]);
            temp = _mm_aesenc_si128(temp, key_schedule[5]);
            temp = _mm_aesenc_si128(temp, key_schedule[6]);
            temp = _mm_aesenc_si128(temp, key_schedule[7]);
            temp = _mm_aesenc_si128(temp, key_schedule[8]);
            temp = _mm_aesenc_si128(temp, key_schedule[9]);
            temp = _mm_aesenclast_si128(temp, key_schedule[10]);
        }

        _mm_storeu_si128(&((__m128i*)out)[i], temp);
    }
}
template <std::size_t rounds>
void aes128_decrypt(uint8_t* cipherText, uint8_t* out, size_t length, __m128i* key_schedule) {
    __m128i temp;
    if (length % 16)
        length = length / 16 + 1;
    else
        length = length / 16;

    for (int i = 0; i < length; i++) {
        temp = _mm_loadu_si128(&((__m128i*)cipherText)[i]);
        if constexpr (rounds == 4) {

            temp = _mm_xor_si128(temp, key_schedule[4]);
            temp = _mm_aesdec_si128(temp, key_schedule[5]);
            temp = _mm_aesdec_si128(temp, key_schedule[6]);
            temp = _mm_aesdec_si128(temp, key_schedule[7]);
        } else {

            temp = _mm_xor_si128(temp, key_schedule[10 + 0]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 1]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 2]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 3]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 4]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 5]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 6]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 7]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 8]);
            temp = _mm_aesdec_si128(temp, key_schedule[10 + 9]);
        }
        temp = _mm_aesdeclast_si128(temp, key_schedule[0]);
        _mm_storeu_si128(&((__m128i*)out)[i], temp);
    }
}

#endif // __SAES__
