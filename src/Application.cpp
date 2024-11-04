#include "Application.h"
#include "Parser.h"

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
                        return std::format("\"{}\"", EscapeUtf8String(arg));
                    else if constexpr (std::is_same_v<std::decay_t<U>, std::monostate>)
                        return "";
                    else
                        return std::to_string(arg);
                },
                val
        );
    }

    int GenerateTokensFile(std::istream& input, const std::string& outputFileName) {
        Lexer lexer(input);
        SymbolTable symbolTable;

        std::ofstream tokenFile(outputFileName, std::ios::binary);

        int status = 0;

        bool isRunning = true;
        while (isRunning) {
            try {
                const auto [type, val] = lexer.GetToken(symbolTable);

                std::string content = VariantToString(val);

                tokenFile << "<" << ToString(type) << ", " << content << ">" << std::endl;

                if (type == TokenType::END) isRunning = false;
            } catch (const LexicalException& e) {
                std::cerr << "(" << e.GetLine() << ":" << e.GetColumn() << ") ERROR: " << e.what() << std::endl;
                lexer.SkipChar();
                status = 1;
            }
        }

        return status;
    }

    int GenerateTokensFile(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream fileStream(inputFileName, std::ios::binary);
        if (!fileStream) {
            std::cerr << "No existe el archivo \"" << inputFileName << "\"." << std::endl;
            return 1;
        }

        return GenerateTokensFile(fileStream, outputFileName);
    }

    int GenerateSymbolsFile(std::istream& input, const std::string& outputFileName) {
        Lexer lexer(input);
        SymbolTable symbolTable;

        std::ofstream symbolsFile(outputFileName, std::ios::binary);

        int status = 0;

        bool isRunning = true;
        while (isRunning) {
            try {
                const auto [type, val] = lexer.GetToken(symbolTable);
                if (type == TokenType::END) isRunning = false;
            } catch (const LexicalException& e) {
                std::cerr << "(" << e.GetLine() << ":" << e.GetColumn() << ") ERROR: " << e.what() << std::endl;
                lexer.SkipChar();
                status = 1;
            }
        }

        symbolTable.WriteTable(symbolsFile);

        return status;
    }

    int GenerateSymbolsFile(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream fileStream(inputFileName, std::ios::binary);
        if (!fileStream) {
            std::cerr << "No existe el archivo \"" << inputFileName << "\"." << std::endl;
            return 1;
        }

        return GenerateSymbolsFile(fileStream, outputFileName);
    }

    int GenerateParse(std::istream& input, const std::string& outputFileName) {
        Parser parser(input);
        SymbolTable symbolTable;

        std::ofstream symbolsFile(outputFileName, std::ios::binary);

        int status = 0;

        std::cout << parser.Parse(symbolTable) << std::endl;

        return status;
    }

    int GenerateParse(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream fileStream(inputFileName, std::ios::binary);
        if (!fileStream) {
            std::cerr << "No existe el archivo \"" << inputFileName << "\"." << std::endl;
            return 1;
        }

        return GenerateParse(fileStream, outputFileName);
    }
}

Application::Application(const ApplicationAttributes& attributes) :
        m_taskType(attributes.taskType),
        m_inputFileName(attributes.inputFileName),
        m_outputFileName(attributes.outputFileName) {
    if (m_taskType == TaskType::None)
        throw std::runtime_error("Se debe especificar una tarea a ejecutar.");
}

int Application::Run() {
    int result = 0;

    switch (m_taskType) {
        case TaskType::Tokens: {
            const std::string& outputFileName = m_outputFileName.empty() ? "tokens.txt" : m_outputFileName;
            if (m_inputFileName.empty())
                result = GenerateTokensFile(std::cin, outputFileName);
            else
                result = GenerateTokensFile(m_inputFileName, outputFileName);
            break;
        }

        case TaskType::Symbols: {
            const std::string& outputFileName = m_outputFileName.empty() ? "symbols.txt" : m_outputFileName;
            if (m_inputFileName.empty())
                result = GenerateSymbolsFile(std::cin, outputFileName);
            else
                result = GenerateSymbolsFile(m_inputFileName, outputFileName);
            break;
        }

        case TaskType::Parse: {
            const std::string& outputFileName = m_outputFileName.empty() ? "parse.txt" : m_outputFileName;
            if (m_inputFileName.empty())
                result = GenerateParse(std::cin, outputFileName);
            else
                result = GenerateParse(m_inputFileName, outputFileName);
            break;
        }

        case TaskType::None:
            assert(false);
    }

    return result;
}
