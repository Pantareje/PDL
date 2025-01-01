#include "Application.h"
#include "Parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

namespace {
    int GenerateTokens(std::istream& input, std::ostream& output) {
        Lexer lexer(input);
        GlobalState globals = {
            .useSemantic = false,
            .globalTable = { SymbolTable(0) }
        };

        int status = 0;

        bool isRunning = true;
        while (isRunning) {
            try {
                const auto token = lexer.GetToken(globals);
                std::string content = TokenAttributeToString(token);
                output << "<" << ToString(token.type) << ", " << content << ">" << std::endl;
                if (token.type == TokenType::END) isRunning = false;
            } catch (const LexicalException& e) {
                std::cerr << std::format(
                    "({}:{}) LE-{:04X}: ",
                    e.GetLine(), e.GetColumn(),
                    static_cast<uint32_t>(e.GetError())
                );
                std::cerr << e.what() << std::endl;
                lexer.SkipLine();
                status = 1;
            }
        }

        return status;
    }

    int GenerateTokens(std::istream& input, const std::string& outputFileName) {
        std::ofstream tokenFile(outputFileName.empty() ? "tokens.txt" : outputFileName, std::ios::binary);
        return GenerateTokens(input, tokenFile);
    }

    int GenerateTokens(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream fileStream(inputFileName, std::ios::binary);
        if (!fileStream) {
            std::cerr << "No existe el archivo \"" << inputFileName << "\"." << std::endl;
            return 1;
        }

        return GenerateTokens(fileStream, outputFileName);
    }


    int GenerateParse(std::istream& input, std::ostream& output, bool useSemantic) {
        Parser parser(input);
        GlobalState globals = {
            .syntaxPrint = SyntaxPrint::PARSE,
            .useSemantic = useSemantic
        };

        int status = 0;

        std::ostringstream ss;

        try {
            parser.Parse(ss, globals);
        } catch (const SyntaxException& e) {
            std::cerr << "(" << e.GetLine() << ":" << e.GetColumn() << ")";
            std::cerr << " ERROR: " << e.what() << std::endl;
            status = 1;
        }

        output << "Des" << ss.str() << std::endl;
        return status;
    }

    int GenerateParse(std::istream& input, const std::string& outputFileName, bool useSemantic) {
        int result = 0;

        if (outputFileName.empty()) {
            result = GenerateParse(input, std::cout, useSemantic);
        } else {
            std::ofstream symbolsFile(outputFileName, std::ios::binary);
            result = GenerateParse(input, symbolsFile, useSemantic);
        }

        return result;
    }

    int GenerateParse(const std::string& inputFileName, const std::string& outputFileName, bool useSemantic) {
        std::ifstream fileStream(inputFileName, std::ios::binary);
        if (!fileStream) {
            std::cerr << "No existe el archivo \"" << inputFileName << "\"." << std::endl;
            return 1;
        }

        return GenerateParse(fileStream, outputFileName, useSemantic);
    }


    int GenerateLexicalSymbols(std::istream& input, std::ostream& output) {
        Lexer lexer(input);
        GlobalState globals = {
            .syntaxPrint = SyntaxPrint::SYMBOLS,
            .useSemantic = false,
            .globalTable = { SymbolTable(0) }
        };

        int status = 0;

        bool isRunning = true;
        while (isRunning) {
            try {
                const auto token = lexer.GetToken(globals);
                if (token.type == TokenType::END) isRunning = false;
            } catch (const LexicalException& e) {
                std::cerr << "(" << e.GetLine() << ":" << e.GetColumn() << ")";
                std::cerr << " ERROR: " << e.what() << std::endl;
                lexer.SkipLine();
                status = 1;
            }
        }

        globals.globalTable->WriteTable(output);
        globals.globalTable = std::nullopt;

        return status;
    }

    int GenerateSemanticSymbols(std::istream& input, std::ostream& output) {
        int status = 0;

        Parser parser(input);
        GlobalState globals = {
            .syntaxPrint = SyntaxPrint::SYMBOLS,
            .useSemantic = true
        };

        std::ostringstream ss;

        try {
            parser.Parse(ss, globals);
        } catch (const SyntaxException& e) {
            std::cerr << "(" << e.GetLine() << ":" << e.GetColumn() << ")";
            std::cerr << " ERROR: " << e.what() << std::endl;
            status = 1;
        }

        assert(globals.globalTable.has_value());

        globals.globalTable->WriteTable(output);
        output << "\n";
        globals.globalTable = std::nullopt;

        output << ss.str();

        return status;
    }

    int GenerateSymbols(std::istream& input, std::ostream& output, bool useSemantic) {
        int status = 0;

        if (useSemantic)
            status = GenerateSemanticSymbols(input, output);
        else
            status = GenerateLexicalSymbols(input, output);

        return status;
    }

    int GenerateSymbols(std::istream& input, const std::string& outputFileName, bool useSemantic) {
        std::ofstream symbolsFile(outputFileName.empty() ? "symbols.txt" : outputFileName, std::ios::binary);
        return GenerateSymbols(input, symbolsFile, useSemantic);
    }

    int GenerateSymbols(const std::string& inputFileName, const std::string& outputFileName, bool useSemantic) {
        std::ifstream fileStream(inputFileName, std::ios::binary);
        if (!fileStream) {
            std::cerr << "No existe el archivo \"" << inputFileName << "\"." << std::endl;
            return 1;
        }

        return GenerateSymbols(fileStream, outputFileName, useSemantic);
    }
}


Application::Application(const ApplicationAttributes& attributes)
    : m_taskType(attributes.taskType),
      m_useSemantic(attributes.useSemantic),
      m_inputFileName(attributes.inputFileName),
      m_outputFileName(attributes.outputFileName) {
    if (m_taskType == TaskType::None)
        throw std::runtime_error("Se debe especificar una tarea a ejecutar.");
}

int Application::Run() const {
    int result = 0;

    try {
        switch (m_taskType) {
        case TaskType::Tokens: {
            if (m_useSemantic)
                std::cerr << "El análisis léxico no admite funcionalidad semántica." << std::endl;

            if (m_inputFileName.empty())
                result = GenerateTokens(std::cin, m_outputFileName);
            else
                result = GenerateTokens(m_inputFileName, m_outputFileName);
            break;
        }

        case TaskType::Symbols: {
            if (m_inputFileName.empty())
                result = GenerateSymbols(std::cin, m_outputFileName, m_useSemantic);
            else
                result = GenerateSymbols(m_inputFileName, m_outputFileName, m_useSemantic);
            break;
        }

        case TaskType::Parse: {
            if (m_inputFileName.empty())
                result = GenerateParse(std::cin, m_outputFileName, m_useSemantic);
            else
                result = GenerateParse(m_inputFileName, m_outputFileName, m_useSemantic);
            break;
        }

        case TaskType::None:
            std::unreachable();
        }
    } catch (int) {}
    /*const std::exception& e) {
        std::cerr << "ERROR CRÍTICO: " << e.what() << std::endl;
        result = 2;
    }*/

    return result;
}
