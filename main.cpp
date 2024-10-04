#include "Lexer.h"

#include <iostream>
#include <fstream>
#include <format>

namespace {
    /**
     * Devuelve el valor del contenido de un token como texto.
     * @param val La variante con el contenido del token.
     * @return El valor del contenido en forma de texto.
     */
    constexpr std::string VariantToString(const auto& val) {
        return std::visit(
            []<typename U>(U&& arg) -> std::string {
                if constexpr (std::is_same_v<std::decay_t<U>, std::string>)
                    return std::format("{:?} ({})", arg, CountCodepointsUtf8(arg));
                else if constexpr (std::is_same_v<std::decay_t<U>, std::monostate>)
                    return "";
                else
                    return std::to_string(arg);
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

    int status = 0;

    bool isRunning = true;
    while (isRunning) {
        try {
            const auto [type, val] = lexer.GetToken();
            std::cout << ToString(type) << ' ' << VariantToString(val) << std::endl;
            if (type == TokenType::END) isRunning = false;
        } catch (const LexicalException& e) {
            std::cout << "(" << e.GetLine() << ":" << e.GetColumn() << ") ERROR: " << e.what() << std::endl;
            lexer.SkipChar();
            status = 1;
        }
    }

    return status;
}
