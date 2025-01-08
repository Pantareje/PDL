#include "application/Application.h"

#include <stdexcept>
#include <iostream>
#include <format>

namespace {
    class ArgumentException final : public std::runtime_error {
    public:
        explicit ArgumentException(const char* message) : std::runtime_error(message) {}
        explicit ArgumentException(const std::string& message) : std::runtime_error(message) {}
    };
    
    class ArgumentsReader {
        int m_argc;
        const char** m_argv;

        mutable int m_current;
        mutable std::string_view m_arg;

        bool GetNextArgument() const {
            if (m_current < m_argc) {
                m_arg = { m_argv[m_current] };
                m_current += 1;
                return true;
            }

            return false;
        }

    public:
        ArgumentsReader(const int argc, const char* argv[]) : m_argc(argc), m_argv(argv), m_current() {}

        ApplicationAttributes GetAttributes() const {
            ApplicationAttributes attributes = {
                .taskType = TaskType::None,
                .useSemantic = false
            };

            m_current = 1;

            while (GetNextArgument()) {
                if (m_arg == "-t") {
                    if (attributes.taskType != TaskType::None)
                        throw ArgumentException("Ya se ha indicado un tipo de tarea a realizar.");
                    attributes.taskType = TaskType::Tokens;
                } else if (m_arg == "-s") {
                    if (attributes.taskType != TaskType::None)
                        throw ArgumentException("Ya se ha indicado un tipo de tarea a realizar.");
                    attributes.taskType = TaskType::Symbols;
                } else if (m_arg == "-p") {
                    if (attributes.taskType != TaskType::None)
                        throw ArgumentException("Ya se ha indicado un tipo de tarea a realizar.");
                    attributes.taskType = TaskType::Parse;
                } else if (m_arg == "-u") {
                    attributes.useSemantic = true;
                } else if (m_arg == "-i") {
                    if (!attributes.inputFileName.empty())
                        throw ArgumentException("Ya se ha definido un fichero de entrada.");
                    if (!GetNextArgument())
                        throw ArgumentException("Después de «-i» se debe especificar un fichero de entrada.");
                    if (m_arg.empty())
                        throw ArgumentException("El nombre del fichero de entrada no puede estar vacío.");
                    attributes.inputFileName = m_arg;
                } else if (m_arg == "-o") {
                    if (!attributes.outputFileName.empty())
                        throw ArgumentException("Ya se ha definido un fichero de salida.");
                    if (!GetNextArgument())
                        throw ArgumentException("Después de «-o» se debe especificar un fichero de salida.");
                    if (m_arg.empty())
                        throw ArgumentException("El nombre del fichero de salida no puede estar vacío.");
                    attributes.outputFileName = m_arg;
                } else {
                    throw ArgumentException(std::format("El argumento «{}» es desconocido.", m_arg));
                }
            }

            if (attributes.taskType == TaskType::None) {
                throw ArgumentException("Es necesario indicar una tarea a relizar.");
            }

            return attributes;
        }
    };
}

int main(const int argc, const char* argv[]) {
    try {
        const ApplicationAttributes attributes = ArgumentsReader(argc, argv).GetAttributes();
        const Application application(attributes);
        return application.Run();
    } catch (const ArgumentException& e) {
        std::cerr << e.what() << std::endl;
        return 4;
    }
}
