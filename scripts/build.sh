g++ -std=c++17 -O2 -g \
    -Wall -Wextra -Werror -pedantic \
    -Wshadow -Wconversion -Wsign-conversion \
    -fsanitize=undefined,address \
    -fno-omit-frame-pointer \
    -o ./bin/Neptune src/*.cpp
