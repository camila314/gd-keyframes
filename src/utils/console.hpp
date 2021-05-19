#pragma once

#include <vector>
#include <functional>

namespace gd {
    namespace console {
        using console_func = std::pair<std::string, std::function<void(std::string)>>;

        bool load();
        void unload();
        void awaitUnload();

        void addInputs(std::vector<console_func> const&);
    }
}

