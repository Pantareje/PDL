#pragma once

#include "ApplicationAttributes.h"

class Application {
    TaskType m_taskType;
    bool m_useSemantic;
    std::string m_inputFileName;
    std::string m_outputFileName;

public:
    explicit Application(const ApplicationAttributes& attributes);

    [[nodiscard]] int Run() const;
};
