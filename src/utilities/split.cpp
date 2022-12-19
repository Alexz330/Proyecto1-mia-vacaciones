#ifndef _SPLIT_
#define _SPLIT_
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "../utilities/validadorCadena.cpp"

using namespace std;

vector<string> Split(string cadena, string separador)
{
    size_t pos = 0;
    cadena += separador;
    string token;
    vector<string> CadenaDividida;
    bool indicador = false;
    string auxiliar = "";
    while ((pos = cadena.find(separador)) != std::string::npos)
    {
        token = cadena.substr(0, pos);
        if (CadenaContiene(token, "\"") == 1)
        {
            indicador = !indicador;
            if (indicador == false)
            {
                auxiliar += " " + token;
                CadenaDividida.push_back(auxiliar);
                auxiliar = "";
            }
            else
            {
                auxiliar += token;
            }
        }
        else if (indicador)
        {
            auxiliar += " " + token;
        }
        else
        {
            CadenaDividida.push_back(token);
        }
        cadena.erase(0, pos + separador.length());
    }
    return CadenaDividida;
}
#endif