#include "stdafx.h"

void replace_all(std::string& str, const std::string& old, const std::string& repl) {
    size_t pos = 0;
    while ((pos = str.find(old, pos)) != std::string::npos) {
        str.replace(pos, old.length(), repl);
        pos += repl.length();
    }
}

std::string escape_xml(std::string str) {
    replace_all(str, std::string("&"), std::string("&amp;"));
    replace_all(str, std::string("'"), std::string("&apos;"));
    replace_all(str, std::string("\""), std::string("&quot;"));
    replace_all(str, std::string(">"), std::string("&gt;"));
    replace_all(str, std::string("<"), std::string("&lt;"));

    return str;
}