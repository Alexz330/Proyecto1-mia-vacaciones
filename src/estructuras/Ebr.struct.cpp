#ifndef _EBR_
#define _EBR_
#include <cstring>

using namespace std;

struct EBR
{
    char part_status; //indica si la particion esta activa o no
    char part_fit;//Tipo de ajuste de la partición. Tendrá los valores B (Best), F (First) o W (worst)
    int part_start;//indica el byte del disco donde inicia la particion
    int part_size;//Tamaño total de la particion en bytes
    int part_next;//byte en que se encuentra el proximo EBR. -1 si no hay siguiente
    char part_name[16];//Nombre de la particion
};

EBR InicializarEBR(EBR ebr){
    ebr.part_status='0'; //indica si la particion esta activa o no
    ebr.part_fit='B';//Tipo de ajuste de la partición. Tendrá los valores B (Best), F (First) o W (worst)
    ebr.part_start=-1;//indica el byte del disco donde inicia la particion
    ebr.part_size=-1;//Tamaño total de la particion en bytes
    ebr.part_next=-1;//byte en que se encuentra el proximo EBR. -1 si no hay siguiente
    strcpy(ebr.part_name,"");
    return ebr;
}

#endif 
