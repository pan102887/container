#include "map_test_data_struct.h"

const char* random_str(int length) {
    char* buffer = (char*)malloc(sizeof(char) * length + 1);

    std::random_device rd;
    std::default_random_engine random(rd());

    char tmp;
    for (int i = 0; i < length; i++) {
        auto value = random() % 36;
        tmp = value;
        if (tmp < 10) {
            tmp += '0';
        }
        else {
            tmp -= 10;
            tmp += 'A';
        }
        buffer[i] = tmp;
    }
    buffer[length] = '\0';
    return buffer;
}
