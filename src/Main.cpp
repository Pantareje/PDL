#include "Application.h"

#include <stdexcept>
#include <iostream>

namespace {
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
            ApplicationAttributes attributes = {};

            m_current = 1;

            while (GetNextArgument()) {
                if (m_arg == "-t") {
                    if (attributes.taskType != TaskType::None)
                        throw std::runtime_error("Ya se ha indicado un tipo de tarea a realizar.");

                    attributes.taskType = TaskType::Tokens;
                }

                else if (m_arg == "-s") {
                    if (attributes.taskType != TaskType::None)
                        throw std::runtime_error("Ya se ha indicado un tipo de tarea a realizar.");

                    attributes.taskType = TaskType::Symbols;
                }

                else if (m_arg == "-p") {
                    if (attributes.taskType != TaskType::None)
                        throw std::runtime_error("Ya se ha indicado un tipo de tarea a realizar.");

                    attributes.taskType = TaskType::Parse;
                }


                else if (m_arg == "-i") {
                    if (!attributes.inputFileName.empty())
                        throw std::runtime_error("Ya se ha definido un fichero de entrada.");

                    if (!GetNextArgument())
                        throw std::runtime_error("Después de «-i» se debe especificar un fichero de entrada.");

                    if (m_arg.empty())
                        throw std::runtime_error("El nombre del fichero de entrada no puede estar vacío.");

                    attributes.inputFileName = m_arg;
                }

                else if (m_arg == "-o") {
                    if (!attributes.outputFileName.empty())
                        throw std::runtime_error("Ya se ha definido un fichero de salida.");

                    if (!GetNextArgument())
                        throw std::runtime_error("Después de «-o» se debe especificar un fichero de salida.");

                    if (m_arg.empty())
                        throw std::runtime_error("El nombre del fichero de salida no puede estar vacío.");

                    attributes.outputFileName = m_arg;
                }

                else {
                    throw std::runtime_error(std::format("El argumento «{}» es desconocido.", m_arg));
                }
            }

            return attributes;
        }
    };
}

int main(const int argc, const char* argv[]) {
    try {
        ApplicationAttributes attributes = ArgumentsReader(argc, argv).GetAttributes();
        Application application(attributes);
        return application.Run();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}
