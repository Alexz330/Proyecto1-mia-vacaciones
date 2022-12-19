#ifndef _RUTAACTUAL_
#define _RUTAACTUAL_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <linux/limits.h>


using namespace std;
string rutaActual()
{
    char buff[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
    string wet;
    if (len != -1)
    {
        wet = std::string(buff, static_cast<size_t>(len));
    }
    /* handle error condition */
    //retirando el nombre del archivo
    string space_delimiter = "/";
    vector<string> words{};
    size_t pos = 3;
    while ((pos = wet.find(space_delimiter)) != string::npos)
    {
        words.push_back(wet.substr(0, pos) + "/");
        wet.erase(0, pos + space_delimiter.length());
    }
    string ruta = "";
    for (const auto &str : words)
    {
        ruta += str;
    }
    string ruta2 ="";
    for (size_t i = 0; i < ruta.size()-1; i++)
    {
        ruta2+=ruta[i];
    }
    
    return ruta2;
}
#endif