#ifndef _CADENACONTIENE_
#define _CADENACONTIENE_

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
int CadenaContiene(string cadena, string caracter)
{
    int retorno = 0;
    for (size_t i = 0; i < cadena.length(); i++)
    {
        if (cadena[i] == caracter[0])
        {
            retorno += 1;
        }
    }
    return retorno;
}
#endif