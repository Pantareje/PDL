#include "Lexer.h"

#include <iostream>
#include <fstream>

namespace {
    /**
     * Devuelve el valor del contenido de un token como texto.
     * @param val La variante con el contenido del token.
     * @return El valor del contenido en forma de texto.
     */
    constexpr std::string VariantToString(const auto& val) {
        return std::visit(
            []<typename U>(U&& arg) -> std::string {
                if constexpr (std::is_same_v<std::decay_t<U>, std::monostate>) return "";
                else if constexpr (std::is_same_v<std::decay_t<U>, std::string>) return arg;
                else return std::to_string(arg);
            },
            val
        );
    }
}

int main(const int argc, const char* argv[]) {
    std::ifstream fileStream;

    switch (argc) {
    case 1:
        break;
    case 2:
        fileStream = std::ifstream(argv[1], std::ios::binary);
        if (!fileStream) return 1;
        break;
    default:
        return 1;
    }

    Lexer lexer(argc == 1 ? std::cin : fileStream);

    while (true) {
        try {
            const auto [type, val] = lexer.GetToken();
            std::cout << ToString(type) << ' ' << VariantToString(val) << std::endl;
            if (type == TokenType::END) return 0;
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
}
