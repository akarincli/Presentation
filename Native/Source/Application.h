#pragma once

#include "Config.h"
#include <cstdint>
#include <cstddef>

class Application {
public:
    void Init() noexcept;
    void ControlHandOver() noexcept;
    void Stop() noexcept;
    void Finalize() noexcept;
};
