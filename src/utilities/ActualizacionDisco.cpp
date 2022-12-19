#ifndef _ACTUALIZARDISCO_
#define _ACTUALIZARDISCO_

#include <stdio.h>
#include <string.h>

#include "../estructuras/Mbr.struct.cpp"
using namespace std;

void AcutalizarDisco(string ruta, MBR mbr){
    char sc[ruta.size()];
    strcpy(sc, ruta.c_str());
    //colocando fecha e identificador al disco
    mbr.mbr_fecha_creacion = time(0);
    //abriendo el archivo en formato escritura
    FILE *file = fopen(sc, "rb+");
    //coloco el punteron en el inicio
    fseek(file, 0, SEEK_SET);
    //escribiendo el objeto
    fwrite(&mbr, sizeof(mbr), 1, file);
    //Colocando el puntero en el tamaño deseado
    fseek(file, mbr.mbr_tamano - 1, SEEK_SET);
    //escribo algo en el puntero para mantener su tamaño
    fwrite("\0", 1, 1, file);
    //cierro el archivo
    fclose(file);
}

#endif