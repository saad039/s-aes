# S-AES

AES-128 bit implementation using Intel's AES-NI instruction set extensions. Use of AES-NI eliminates the need for AES lookup tables(S-box and Inverse S-box), which have been a source of cache-related timing side-channel vulnerabilities.

## Prerequisites
In order to build this on your machine, you must have

<ol>
<li>A C++17 compatible compiler</li>
<li>A modern x86 processor supporting AES-NI</li>
</ol>


## Build

Use [premake ](https://premake.github.io/) to build this project. To build, simply

```bash
premake5 <target>
```
where *target* is **gmake** for *unix* makefiles or **vs2019** for *visual studio 2019 solution files*. For more information about premake, checkout the official documentation.

## Usage

```cpp
constexpr std::size_t ROUNDS = 4; //only supports 4 and 10 rounds

aes128_encrypt<ROUNDS>(plainText, cipherText, sizeof plainText, key_schedule); //encrypting

aes128_decrypt<ROUNDS>(cipherText, decryptedText, sizeof plainText, key_schedule); //decrypting
```

## Preview

You can find the compiler explorer preview [here](https://godbolt.org/z/37ns5W91n)

## License
[GPL3](https://www.gnu.org/licenses/gpl-3.0.en.html)
