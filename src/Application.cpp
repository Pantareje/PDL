#include "Application.h"
#include "Parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

namespace {
    int GenerateTokensFile(std::istream& input, std::ostream& output) {
        Lexer lexer(input);
        SymbolTable symbolTable;

        int status = 0;

        bool isRunning = true;
        while (isRunning) {
            try {
                const auto token = lexer.GetToken(symbolTable);
                std::string content = TokenAttributeToString(token);
                output << "<" << ToString(token.type) << ", " << content << ">" << std::endl;
                if (token.type == TokenType::END) isRunning = false;
            } catch (const LexicalException& e) {
                std::cerr << "(" << e.GetLine() << ":" << e.GetColumn() << ") ERROR: " << e.what() << std::endl;
                lexer.SkipChar();
                status = 1;
            }
        }

        return status;
    }

    int GenerateTokensFile(std::istream& input, const std::string& outputFileName) {
        std::ofstream tokenFile(outputFileName.empty() ? "tokens.txt" : outputFileName, std::ios::binary);
        return GenerateTokensFile(input, tokenFile);
    }

    int GenerateTokensFile(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream fileStream(inputFileName, std::ios::binary);
        if (!fileStream) {
            std::cerr << "No existe el archivo \"" << inputFileName << "\"." << std::endl;
            return 1;
        }

        return GenerateTokensFile(fileStream, outputFileName);
    }


    int GenerateSymbolsFile(std::istream& input, std::ostream& output) {
        Lexer lexer(input);
        SymbolTable symbolTable;

        int status = 0;

        bool isRunning = true;
        while (isRunning) {
            try {
                const auto token = lexer.GetToken(symbolTable);
                if (token.type == TokenType::END) isRunning = false;
            } catch (const LexicalException& e) {
                std::cerr << "(" << e.GetLine() << ":" << e.GetColumn() << ") ERROR: " << e.what() << std::endl;
                lexer.SkipChar();
                status = 1;
            }
        }

        symbolTable.WriteTable(output);

        return status;
    }

    int GenerateSymbolsFile(std::istream& input, const std::string& outputFileName) {
        std::ofstream symbolsFile(outputFileName.empty() ? "symbols.txt" : outputFileName, std::ios::binary);
        return GenerateSymbolsFile(input, symbolsFile);
    }

    int GenerateSymbolsFile(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream fileStream(inputFileName, std::ios::binary);
        if (!fileStream) {
            std::cerr << "No existe el archivo \"" << inputFileName << "\"." << std::endl;
            return 1;
        }

        return GenerateSymbolsFile(fileStream, outputFileName);
    }


    int GenerateParse(std::istream& input, std::ostream& output) {
        Parser parser(input);
        SymbolTable symbolTable;

        int status = 0;

        std::ostringstream ss;

        try {
            parser.Parse(symbolTable, ss);
            output << "Des" << ss.str() << std::endl;
        } catch (const SyntaxException& e) {
            std::cerr << "(" << e.GetLine() << ":" << e.GetColumn() << ") ERROR: " << e.what() << std::endl;
            status = 1;
        }

        return status;
    }

    int GenerateParse(std::istream& input, const std::string& outputFileName) {
        int result = 0;

        if (outputFileName.empty()) {
            result = GenerateParse(input, std::cout);
        } else {
            std::ofstream symbolsFile(outputFileName, std::ios::binary);
            result = GenerateParse(input, symbolsFile);
        }

        return result;
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
            if (m_inputFileName.empty())
                result = GenerateTokensFile(std::cin, m_outputFileName);
            else
                result = GenerateTokensFile(m_inputFileName, m_outputFileName);
            break;
        }

        case TaskType::Symbols: {
            if (m_inputFileName.empty())
                result = GenerateSymbolsFile(std::cin, m_outputFileName);
            else
                result = GenerateSymbolsFile(m_inputFileName, m_outputFileName);
            break;
        }

        case TaskType::Parse: {
            if (m_inputFileName.empty())
                result = GenerateParse(std::cin, m_outputFileName);
            else
                result = GenerateParse(m_inputFileName, m_outputFileName);
            break;
        }

        case TaskType::None:
            assert(false);
    }

    return result;
}
