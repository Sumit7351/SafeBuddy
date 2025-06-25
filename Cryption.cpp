/*#include "Cryption.hpp"
#include "../processes/Task.hpp"
#include "../fileHandling/ReadEnv.cpp"

int executeCryption(const std::string& taskData) {
    Task task = Task::fromString(taskData);
    ReadEnv env;
    std::string envKey = env.getenv();
    int key = std::stoi(envKey);
    if (task.action == Action::ENCRYPT) {
        char ch;
        while (task.f_stream.get(ch)) {
            ch = (ch + key) % 256;
            task.f_stream.seekp(-1, std::ios::cur);
            task.f_stream.put(ch);
        }
        task.f_stream.close();
    } else {
        char ch;
        while (task.f_stream.get(ch)) {
            ch = (ch - key + 256) % 256;
            task.f_stream.seekp(-1, std::ios::cur);
            task.f_stream.put(ch);
        }
        task.f_stream.close();
    }
    return 0;
}*/

#include "Cryption.hpp"
#include "../processes/Task.hpp"
#include "../fileHandling/ReadEnv.cpp"
#include <cstring>  // For memcpy
#include <fstream>
#include <vector>
#include <iostream>
#include <sodium.h>

int executeCryption(const std::string& taskData) {
    if (sodium_init() < 0) {
        std::cerr << "Libsodium initialization failed" << std::endl;
        return 1;
    }

    Task task = Task::fromString(taskData);
    ReadEnv env;
    std::string envKey = env.getenv();

    if (envKey.length() < 32) {
        std::cerr << "Key must be at least 32 characters for ChaCha20" << std::endl;
        return 1;
    }

    unsigned char key[crypto_stream_chacha20_KEYBYTES];
    memcpy(key, envKey.data(), crypto_stream_chacha20_KEYBYTES);

    unsigned char nonce[crypto_stream_chacha20_NONCEBYTES] = {0};  // You can randomize this or keep it fixed for testing

    // Read file into memory
    std::ifstream inFile(task.filePath, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open file: " << task.filePath << std::endl;
        return 1;
    }

    std::vector<unsigned char> data((std::istreambuf_iterator<char>(inFile)),
                                     std::istreambuf_iterator<char>());
    inFile.close();

    std::vector<unsigned char> output(data.size());

    if (task.action == Action::ENCRYPT || task.action == Action::DECRYPT) {
        crypto_stream_chacha20_xor(output.data(), data.data(), data.size(), nonce, key);
    }

    // Write result back to the same file
    std::ofstream outFile(task.filePath, std::ios::binary | std::ios::trunc);
    outFile.write(reinterpret_cast<char*>(output.data()), output.size());
    outFile.close();

    return 0;
}
