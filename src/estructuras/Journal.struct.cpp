#ifndef _JORUNAL_
#define _JOURNAL_
typedef struct{
   char Journal_Tipo_Operacion[10] ; //mkfs,mkfile,...etc
   char Journal_tipo; //Carpeta(0), Archivo(1).
   char Journal_nombre [200]; //..../home....../home/chepe........../home/chepe/datos.txt
   char Journal_contenido [200]; //....este es el contenido de mi archivo de texto.....-
    char Journal_fecha[16]; // 18/06/2019 10:30
    char Journal_propietario [10]; //Quien creó el archivo o carpeta
    int Journal_permisos; //777...664
   }JOURNAL;

#endif