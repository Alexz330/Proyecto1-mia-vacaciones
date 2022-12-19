#ifndef _SUPERBLOQUE_
#define _SUPERBLOQUE_

#include <iostream>
#include <ctime>

struct superbloque
{
    int s_filesystem_type;//numero que identifica el sistema de archivos utilizado
    int s_inodes_count;//numero total de inodos
    int s_blocks_count;//numero total de bloques
    int s_free_blocks_count; //numero de bloques libres
    int s_free_inodes_count; //numero de inodos disponibles
    time_t s_mtime; //ultima fecha en que el sistema fue montado
    time_t s_umtime;// ultima fecha en la que el sistema fue desmontado
    int s_mnt_count; //veces que se ha montado el sistema
    int s_magic;//valor que identifica el sistema de archivos 0xEF53
    int s_inode_size;//Tamaño del inodo
    int s_block_size; //Tamaño del bloque
    int s_free_ino;//primer inodo libre
    int s_free_blo;//primer bloque libre
    int s_bm_inode_start;//inicio del bimap de nodos
    int s_bm_block_start; //inicio del bimap de bloques
    int s_inode_start;//guardara el inicio de la tabla de inodos
    int s_block_start;//guardara el inicio de la tabla de blockes

};

#endif