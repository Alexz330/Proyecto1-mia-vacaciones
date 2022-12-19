#ifndef _PARTICIONESMONTADAS_
#define _PARTICIONESMONTADAS_

#include <vector>

using namespace std;

struct ParticionesMontadas
{
    char nombre[16]; //nombre del disco ESTA CREO QUE NO LA USO
    char path[100]; //RUTA DEL DISCO
    char letra[5]; //LETRA DEL DISCO
    int numero; //NUMERO DE PARTICION
    int carnet=59;
};

vector <ParticionesMontadas> particiones_montadas;

#endif