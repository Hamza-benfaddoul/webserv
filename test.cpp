#include "includes/main.hpp"

int main()
{
    std::ofstream oo("c++.pdf", std::ios::binary);

    std::string w = "h�24P0P��4Q040S���w�I,.�M, �e����� V@bQj^IHQj*P�M�/���;�R�P?(?'b�1XMH";
    oo << "%PDF-1.6" << std::endl;
    oo << "%����" << std::endl;
    oo << "33099 0 obj" << std::endl;
    oo << "<</Filter/FlateDecode/First 13/Length 116/N 2/Type/ObjStm>>stream" << std::endl;
    oo << w << std::endl;
    
}
