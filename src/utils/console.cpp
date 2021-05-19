#include "console.hpp"
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

std::vector<gd::console::console_func> g_inputs;

bool gd::console::load() {
    if (AllocConsole() == 0)
        return false;

    // redirect console output
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONERR$", "w", stderr);

    return true;
}

void gd::console::unload() {
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    FreeConsole();
}

void gd::console::addInputs(std::vector<console_func> const& _inps) {
    g_inputs = _inps;
}

void gd::console::awaitUnload() {
    std::string inp;
    getline(std::cin, inp);

    for (auto const& func : g_inputs)
        if (inp._Starts_with(func.first))
            if (inp.find(" ") != std::string::npos)
                func.second(inp.substr(func.first.size() + 1));
            else func.second("");

    if (inp != "e")
        awaitUnload();
}

