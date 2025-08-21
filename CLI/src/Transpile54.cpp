// Minimal CLI tool to transpile Luau source to Lua 5.4
#include "Luau/Transpiler.h"
#include "Luau/Parser.h"
#include <iostream>
#include <sstream>

int main()
{
    std::ostringstream buf;
    buf << std::cin.rdbuf();
    std::string src = buf.str();

    Luau::ParseOptions opts;
    auto res = Luau::transpileToLua54(src, opts);
    if (!res.parseError.empty())
    {
        std::cerr << res.parseError << "\n";
        return 1;
    }
    std::cout << res.code;
    return 0;
}
