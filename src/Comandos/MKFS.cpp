
#include <vector>
#include <string.h>
#include <iostream>
#include "../estructuras/Mbr.struct.cpp"
#include "../estructuras/Ebr.struct.cpp"
#include "../estructuras/Bloque.struct.cpp"
#include "../estructuras/Inodo.struct.cpp"
#include "../estructuras/Journal.struct.cpp"
#include "../estructuras/SuperBloque.struct.cpp"
#include "../estructuras/Carpeta.struct.cpp"

#include "../utilities/split.cpp"
#include "../Globales/particionesGlobales.cpp"

using namespace std;

class MkFs{
    public:
        MkFs();
        static void Formatear(vector<string>);
        
};


void formatearExtructuras(int inicioParticion, int n,FILE * arch){
    superbloque sbloque;
    sbloque.s_filesystem_type=3;
    sbloque.s_inodes_count=n;
    sbloque.s_blocks_count=3*n;
    sbloque.s_free_blocks_count=3*n;//RESTO DESPUES
    sbloque.s_free_inodes_count=n; //RESTO DESPUES
    sbloque.s_mtime=time(0);
    sbloque.s_umtime=0;
    sbloque.s_mnt_count=1;//
    sbloque.s_magic=0xEF53;
    sbloque.s_inode_size=sizeof(inodo);
    sbloque.s_block_size=sizeof(Carpeta);
    sbloque.s_free_ino=2;//el primer inodo libre va a ser el 2
    sbloque.s_free_blo=2;     //100+64+(10*64)=804 byte
    sbloque.s_bm_inode_start=inicioParticion + sizeof(superbloque) + (n*sizeof(JOURNAL)); //revisar esta parte
    sbloque.s_bm_block_start=sbloque.s_bm_inode_start + n ;
    sbloque.s_inode_start=sbloque.s_bm_block_start+3*n;
    sbloque.s_block_start=sbloque.s_inode_start+n*sizeof(inodo);

    fseek(arch,inicioParticion, SEEK_SET); //INICIO DE LA PARTICION
    fwrite(&sbloque,sizeof(superbloque),1, arch); //escribimos el superbloque en la particion


    //ahora creo los bitmap de inodos Y BLOQUES
    char bitinodos[n];
    char bibloques[3*n];
    int iniciobitnodos=inicioParticion + sizeof(superbloque) + (n*sizeof(JOURNAL));
    for(int i=2;i<n;i++){

        bitinodos[i]='0'; //valor por defecto
    }
    //----EL BIT DE INODO 0 PASA A ESTAR OCUPADO POR LA CARPETA ROOT
    bitinodos[0]='1';
    bitinodos[1]='1';
    fseek(arch,iniciobitnodos, SEEK_SET);
    fwrite(&bitinodos,sizeof(bitinodos),1,arch);//ver si es sizeof(bitinodos)

    //-------escribo mi bitmap de bloques
    int iniciobloques=iniciobitnodos+n;
    for(int i=2;i<3*n;i++){
        bibloques[i]='0';
    }
    bibloques[0]='1';//---1 para direnciar carpeta
    bibloques[1]='2';//--2 para diferenciar archivo

    fseek(arch,iniciobloques, SEEK_SET);
    //fwrite(&bibloques,sizeof(char),sizeof(bibloques),arch);//ver si es sizeof(bibloques)//o si es sizeof(char)
    fwrite(&bibloques,sizeof(bibloques),1,arch);//ver si es sizeof(bibloques)//o si es sizeof(char)


    //-----------ESCRIBO JOURNALING

    int iniciojournal=(inicioParticion + sizeof(superbloque));
    JOURNAL journalicial;
    //-------------INGRESO DATOS POR DEFECTO PARA JOURNAL
    journalicial.Journal_Tipo_Operacion[0]='-';
    journalicial.Journal_tipo='-';
    journalicial.Journal_nombre[0]='\0';
    journalicial.Journal_contenido[0]='\0';
    journalicial.Journal_fecha[0]='\0';
    journalicial.Journal_propietario[0] = '\0';
    journalicial.Journal_permisos=-1;

    //ESCRIBO TODOS LOS JOURNAL
    for (int i = 0; i < n; i++) {

        fseek(arch, (iniciojournal + (i * sizeof(JOURNAL))), SEEK_SET);
        fwrite(&journalicial, sizeof(JOURNAL), 1, arch);
    }

    //------------ESCRIBO JOURNAL DE LA CARPETA
    JOURNAL journalicial2;
    strcpy(journalicial2.Journal_Tipo_Operacion,"mkfs");
    journalicial2.Journal_tipo='0';//carpeta
    strcpy(journalicial2.Journal_nombre,"/");
    strcpy(journalicial2.Journal_contenido,"-");
    strcpy(journalicial2.Journal_fecha,"4/03/2020");
    strcpy(journalicial2.Journal_propietario,"1");
    journalicial2.Journal_permisos=664;
    //---ESCRIBO EL JOURNAL DE LA CARPETA EN EL ARCHIVO
    int posicioninical=inicioParticion+sizeof(superbloque);
    fseek(arch,posicioninical+0*sizeof(JOURNAL),SEEK_SET); //apunta al journal
    fwrite(&journalicial2,sizeof(JOURNAL),1,arch);


    //------------ESCRIBO JOURNAL DEL ARCHIVO
    JOURNAL journalicial3;
    strcpy(journalicial3.Journal_Tipo_Operacion,"mkfs");
    journalicial3.Journal_tipo='1';//archivo
    strcpy(journalicial3.Journal_nombre,"/users.txt");
    strcpy(journalicial3.Journal_contenido,"1,G,root\n1,U,root,root,123\n");
    strcpy(journalicial3.Journal_fecha,"4/03/2020");
    strcpy(journalicial3.Journal_propietario,"1");
    journalicial3.Journal_permisos=664;
    //---ESCRIBO EL JOURNAL DE LA CARPETA EN EL ARCHIVO

    fseek(arch,posicioninical+1*sizeof(JOURNAL),SEEK_SET); //apunta al journal
    fwrite(&journalicial3,sizeof(JOURNAL),1,arch);


    //AHORA LEO EL SUPERBLOQUE PARA ESCRIBIR EN SUS BLOQUES E INODOS
    superbloque auxsuperbloque;

    fseek(arch,inicioParticion,SEEK_SET);
    fread(&auxsuperbloque,sizeof(superbloque),1,arch);


    //UN INODO PARA LA CARPETA RAIZ
    inodo raiz;
    raiz.i_uid=1;
    raiz.i_gid=1;
    raiz.i_size=0;//REVISAR
    string fechacarpeta="04/03/2020";
    string fechacarpeta2="04/03/2020";
    string fechacarpeta3="04/03/2020";
    memset(raiz.i_atime,0,sizeof(raiz.i_atime));
    memset(raiz.i_mtime,0,sizeof(raiz.i_atime));
    memset(raiz.i_ctime,0,sizeof(raiz.i_atime));
    strcpy(raiz.i_atime,fechacarpeta.c_str());
    strcpy(raiz.i_mtime,fechacarpeta2.c_str());
    strcpy(raiz.i_ctime,fechacarpeta3.c_str());
    for(int i=0;i<15;i++){
        raiz.i_block[i]=-1;
    }
    raiz.i_type='0'; //es carpeta
    raiz.i_perm=664;
    raiz.i_block[0]=0;

    //---creo el bloque carpetas
    Carpeta carpetaroot;
    block_content contenraiz;
    strcpy(contenraiz.b_name, ".");//definimos la carpeta actual
    contenraiz.b_inodo = 0; //apuntador
    carpetaroot.b_content[0] = contenraiz;
    strcpy(contenraiz.b_name, "..");//definimos al padre de la carpeta
    carpetaroot.b_content[1] = contenraiz;

    //content para archivo users.txt
    strcpy(contenraiz.b_name, "users.txt");//agregamos nombre
    contenraiz.b_inodo = 1;//apuntadora hacia inodo asociado
    carpetaroot.b_content[2] = contenraiz;//guardamos el inodo archivo a los apuntadores del inodo de la carpeta raiz
    memset(&contenraiz.b_name, 0, sizeof(contenraiz.b_name));//definimos la carpeta actual
    contenraiz.b_inodo = -1;
    carpetaroot.b_content[3] = contenraiz;
    //------GUARDAMOS EL INODO Y EL BLOQUE DE LA CARPETA EN EL DISCO
    fseek(arch, auxsuperbloque.s_inode_start, SEEK_SET);
    fwrite(&raiz, sizeof(inodo), 1, arch);
    auxsuperbloque.s_free_inodes_count--;//disminuyo en 1 los inodos ocupados
    fseek(arch, auxsuperbloque.s_block_start, SEEK_SET);
    fwrite(&carpetaroot, 64, 1, arch);
    auxsuperbloque.s_free_blocks_count--;//disminuyo en 1 los bloques de archivos ocupados


    //------------------------creo user.txt


    //-------------------AHORA CREO INODO PARA EL ARCHIVO USER----

    char datosarchivo[]="1,G,root\n1,U,root,root,123\n";//cadena a quemar en el archivo
    inodo archivousuarios;
    archivousuarios.i_gid=1;
    archivousuarios.i_size=sizeof(datosarchivo);//el tamaño de la cadena que voy a quemar
    archivousuarios.i_uid=1;

    string fech="04/03/2020";
    string fech2="04/03/2020";
    string fech3="04/03/2020";
    memset(archivousuarios.i_atime,0,sizeof(archivousuarios.i_atime));
    memset(archivousuarios.i_mtime,0,sizeof(archivousuarios.i_atime));
    memset(archivousuarios.i_ctime,0,sizeof(archivousuarios.i_atime));
    //archivousuarios.i_ctime[0]='00';
    strcpy(archivousuarios.i_atime,fech.c_str());
    strcpy(archivousuarios.i_mtime,fech2.c_str());
    strcpy(archivousuarios.i_ctime,fech3.c_str());
    for(int i=0;i<15;i++){
        archivousuarios.i_block[i]=-1;
    }
    archivousuarios.i_perm=664;
    archivousuarios.i_block[0]=1; //su bloque directo va al bloque 1 que es donde va a estar el archivo USERS

    archivousuarios.i_type='1'; //es archivo

    //termino de crear el inodo de archivos usuario

    //------------------------AHORA CREO BLOQUE PARA EL ARCHIVO USER

    Bloque bloquearchivos;
    strcpy(bloquearchivos.b_content,datosarchivo);
    //-------------------------------TERMINAMOS DE CREAR EL BLOQUE DE ARCHIVOS


    //------GUARDAMOS EL INODO Y EL BLOQUE DEL ARCHIVO USR EN EL DISCO
    fseek(arch, auxsuperbloque.s_inode_start+sizeof(inodo), SEEK_SET);
    fwrite(&archivousuarios, sizeof(inodo), 1, arch);
    auxsuperbloque.s_free_inodes_count--;//disminuyo en 1 los inodos ocupados
    fseek(arch, auxsuperbloque.s_block_start+64, SEEK_SET);
    fwrite(&bloquearchivos, 64, 1, arch);
    auxsuperbloque.s_free_blocks_count--;//disminuyo en 1 los bloques ocupados



    //REESCRIBO EL SUPERBLOQUE
    fseek(arch, inicioParticion, SEEK_SET);
    fwrite(&auxsuperbloque, sizeof(superbloque), 1, arch);
    fclose(arch); //cierro el archivo
    cout << "\033[1;" << "32" << "m[" <<"[Particion ha sido formateada con exito]"<< "] " <<endl;


    //   imprimirdatossuperbloque(auxmont);

}

void MkFs::Formatear(vector<string> ins){
bool aceptado = true;
    string id = "";  //id de disco montado carnet numero letra
    string type="full"; //tipo de formateo(fast,full)
    string fs="2fs";//sistema de archivos a formatear(2fs,3fs)
    for (size_t i = 1; i < ins.size(); i++)
    {
        vector<string> orden = Split(ins[i], "=");
        for (size_t i = 0; i < orden[0].size(); i++)
        {
            orden[0][i] = tolower(orden[0][i]);
        }
        
        if (orden[0] == "-id"){
            id = orden[1];
        }else if(orden[0]=="-type"){
            type= orden[1];
        }else if(orden[0]=="-fs"){
            fs=orden[1];
        }else{
          
        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Comando:@@" << orden[0] << "@Invalido]"<< "] " <<endl;

            aceptado = false;
        }
    }
    if (id != ""&& aceptado){
        //verificar que el disco ya este montado para obtener los datos
        int indice_PartMount=-1;
        for (size_t i = 0; i < particiones_montadas.size(); i++)
        {
            string identificador=to_string(particiones_montadas[i].carnet) +to_string(particiones_montadas[i].numero) +particiones_montadas[i].letra;
            if(strcmp(id.c_str(),identificador.c_str())==0){
                indice_PartMount = i;
                break;
            }
        }
        if (indice_PartMount != -1){
            //Procedemos a realizar el formateo
            //obteniendo los datos de la particion a travez del MBR
            MBR mbr=InicialiarMBR_Particiones(mbr);
            string path = particiones_montadas[indice_PartMount].path; 
            char sc[path.size()];
            strcpy(sc,path.c_str());
            FILE * arch = fopen(sc,"rb+");
            if(arch==NULL){
        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Error!\n No se puede leer el disco\n]"<< "] " <<endl;

               
                
            }else{
                fread(&mbr,sizeof(mbr),1,arch);
                //recorriendo las particiones primarias para obtener el tamaño e inicio de la particion
                int indiceExtendida=-1;
                int IndicePariticion=-1;
                int inicio=-1;
                int tamanio= -1;
                for (size_t i = 0; i < 4; i++)
                {
                    int nombresIguales=strcmp(mbr.mbr_partition[i].part_name,particiones_montadas[indice_PartMount].nombre);
                    if(nombresIguales == 0){
                        IndicePariticion=i;
                    }
                    if(mbr.mbr_partition[i].part_type=='e'){
                        indiceExtendida=i;
                    }
                }
                if(IndicePariticion!=-1 || indiceExtendida != -1){
                    //buscando en las particiones primarias
                    if(IndicePariticion!=-1){
                        inicio=mbr.mbr_partition[IndicePariticion].part_start;
                        tamanio = mbr.mbr_partition[IndicePariticion].part_size;
                    }
                    if(IndicePariticion==-1 && indiceExtendida !=-1){//FORMATEAR LA PARTICION SI FUERA LOGICA
                        //Buscar la particion logica
                        int buscar =mbr.mbr_partition[indiceExtendida].part_start;
                        fseek(arch,buscar,SEEK_SET);
                        while(true){
                            EBR ebr = InicializarEBR(ebr);
                            fread(&ebr,sizeof(ebr),1,arch);
                            if(strcmp(ebr.part_name,particiones_montadas[indice_PartMount].nombre)){
                                inicio = ebr.part_start+sizeof(EBR);
                                tamanio = ebr.part_size;
                                break;
                            }
                            if(ebr.part_next!=-1){
                                buscar = ebr.part_next;
                            }else{
                                break;
                            }
                        }
                    }
                    fseek(arch,inicio,SEEK_SET);
                    if(fs=="full"){//SI ES FULL TENGO QUE BORRAR TODO Y LLENAR DE CEROS
                        char buff;
                        buff='\0';
                        for(int i=0;i<tamanio;i++) { //lleno de ceros el archivo
                            fwrite(&buff,sizeof(buff),1,arch);
                            fseek(arch, inicio + i, SEEK_SET);
                        }
                    }
    
                    int n=((tamanio - sizeof(superbloque))/
                            (4+sizeof(JOURNAL)+sizeof(inodo)+3*sizeof(Bloque)));
                    formatearExtructuras(inicio,n,arch);

                }else{
        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Particion no encontrada en el disco]"<< "] " <<endl;
                    
        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Particion no encontrada en el disco]"<< "] " <<endl;
        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> "<<"Nombre de la particion:"<<particiones_montadas[indice_PartMount].nombre<<"]"<< "] " <<endl;
        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> "<<"ruta del disco:"<<particiones_montadas[indice_PartMount].path<<"]"<< "] " <<endl;


                   
                }
                
            }
            
            
        }else{
            
        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> La particion no ha sido montada]"<< "] " <<endl;

        }
    }
}

