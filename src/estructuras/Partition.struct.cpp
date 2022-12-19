#ifndef _PARTITION_
#define _PARTITION_
struct Partition
{
    char part_status;//indica si la particion esta activa o no [0] inactivo [1] activo
    char part_type;//Indica el tipo de partición, primaria o extendida. Tendrá los valores P o E
    char part_fit;//Tipo de ajuste de la partición. Tendrá los valores B (Best), F (First) o W (worst)
    int part_start;//indica el byte del disco donde inicia la particion
    int part_size;//Tamaño total de la particion en bytes [Posicion del EBR inicial]
    char part_name[16];//Nombre de la particion
};

#endif