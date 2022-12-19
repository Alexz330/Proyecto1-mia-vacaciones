#ifndef _RECUPERARDISCO_
#define _RECUPERARDISCO_

#include <vector>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <fstream>

#include "../estructuras/Mbr.struct.cpp"


using namespace std;


MBR RecuperarDisco(string ruta)
{
    char sc[ruta.size()];
    strcpy(sc, ruta.c_str());

    //creo un objeto Disco a recuperar
    MBR mbr;

    // Verificamos si la ruta existe utilizando la función stat
    struct stat sb;
    if (stat(sc, &sb) != 0) {
        // La ruta no existe, retornamos un valor predeterminado
        return mbr;
    }

    // Si la ruta existe, continuamos abriendo el archivo
    FILE *file = fopen(sc, "rb+");


    //escribo en el objeto
    fread(&mbr, sizeof(mbr), 1, file);
    fclose(file);
    return mbr;
}

#endif