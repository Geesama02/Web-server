#include <iostream>
#include <fstream>

using namespace std;

int main(void) 
{
    ofstream file;

    file.open("test.conf");
    std::string tmp;
        
    file.write(tmp.data(), tmp.length());
    for (int i = 1; i < 1000; i++) {

        tmp = "server {\n\
    listen " + std::to_string(i + 1024) +  ";\n\
    server_name walid;\n\
    autoindex on;\n" + "}\n";

        file.write(tmp.data(), tmp.length());
    }
    // tmp += "}\n";
    file.close();
}
