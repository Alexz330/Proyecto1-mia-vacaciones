#ifndef _MBR_
#define _MBR_
#include <string>
#include "../estructuras/Partition.struct.cpp"

using namespace std;



struct MBR
{
    int mbr_tamano; //tamaño total del disco en bytes
    time_t mbr_fecha_creacion; //fecha y hora de creacion del disco
    int mbr_disk_signature; //numero random que identifica de forma unica al disco--> vale madres
    char disk_fit; //tipo de ajuste de particion (B Best, F First, W Worst)
    Partition mbr_partition[4]; //estructura con informacion de la particion
};

MBR InicialiarMBR_Particiones(MBR mbr){
    //valores por defecto MBR
    mbr.mbr_fecha_creacion=time(0);
    mbr.disk_fit='_';
    mbr.mbr_disk_signature=(rand() % 100);
    mbr.mbr_tamano=0;
    //solamente se inicializa el part status de las particiones 
    Partition vacia;
    vacia.part_status='0';//status inactivo
    vacia.part_type='-';
    vacia.part_fit='-';
    vacia.part_start=-1;
    vacia.part_size=-1;
    vacia.part_name[0] = '\0';
    for (size_t i = 0; i < 4; i++){   
        mbr.mbr_partition[i]=vacia;
    }
    return mbr;
    
}

#endif