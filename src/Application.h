#pragma once

#include "ApplicationAttributes.h"

class Application {
    TaskType m_taskType;
    std::string m_inputFileName;
    std::string m_outputFileName;

public:
    explicit Application(const ApplicationAttributes& attributes);

    int Run();
};
