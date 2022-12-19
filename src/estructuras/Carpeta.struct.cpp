#ifndef _BLOCK_CONTENT_
#define _BLOCK_CONTENT_
struct block_content
{
    char b_name[12];//Nombre de la carpeta o archivo
    int b_inodo;//apuntador con el inodo asociado al archivo o carpeta
};


struct Carpeta
{
    block_content b_content[4];//arr con el contenido de la carpeta
};
#endif