#ifndef _INODO_
#define _INODO_

struct inodo
{
    int i_uid;//UID del usuario propietario del archivo o carpeta
    int i_gid;//GID del grupo al que pertenece el archivo o carpeta.
    int i_size; //tamaño del archivo en bytes
    char i_atime[16];//ultima fecha en que se leyo el nodo sin modificarlo
    char i_ctime[16];//fecha en que se creo el inodo
    char i_mtime[16];//ultima fecha en que se modifico el inodo
    int i_block[15];// 12 registros de bloques directos.13 número del bloque simple indirecto. El 14 número del bloque doble indirecto. El 15 número del bloque triple indirecto Si no son utilizados tendrá el valor -1. 
    char i_type;//Indica si es archivo o carpeta. Tendrá los siguientes valores: 1 = Archivo 0 = Carpeta
    int i_perm;/*Guardará los permisos del archivo o carpeta.
                Se trabajará a nivel de bits, estará dividido de la siguiente forma:
                Los primeros tres bits serán para el Usuario i_uid. 
                Los siguientes tres bits serán para el Grupo al que pertenece el usuario. 
                Y los últimostres bits serán para los permisos de Otros usuarios.
                Cada grupo de tres bits significa lo siguiente: 
                El primer bit indica el permiso de lectura R. 
                El segundo bit indica el permiso de escritura W. 
                El tercer bit indica el permisode ejecución X.
                */
};

#endif