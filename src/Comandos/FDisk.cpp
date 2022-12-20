#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "../utilities/split.cpp"
#include "../utilities/rutaActual.cpp"
#include "../estructuras/Mbr.struct.cpp"
#include "../estructuras/Ebr.struct.cpp"

#include "../utilities/verficadorCarpetas.cpp"
#include "../utilities/imprimirDisco.cpp"
#include "../utilities/RecuperacionDisco.cpp"
#include "../utilities/ActualizacionDisco.cpp"


using namespace std;


class FDisk{
    public:
        FDisk();
        static void AdminPartitions(vector<string>);
   
        
};


void FDisk::AdminPartitions(vector<string> ins){
    bool aceptado = true;
    string size = "";  //tamaño de la particion
    string f = "WF";   //tipo de ajuste para guardar la informacion(BB,FF,WF)
    string u = "k";    //unidad de medida del tamaño de la particion
    string path = "";  //verificar que exista--> Ruta del disco en el cual se hara la particion
    string type = "p"; //tipo de particion (primaria (P),logica (L),extendida(E)) -> si es logica verificar que existe una extendida
    string del = "";   //eliminar una particion, --> verificar que exista el path y name (Fast, full)
    string name = "";  //nombre de la particion a modificar io eliminar--> estos nombres no deben repetirse
    string add = "";   //agrega o quita espacio de la particion (+,-) usa el mismo valor de "u" para el multiplicador
    //asignando el valor correspondiente a las variables
    for (size_t i = 1; i < ins.size(); i++)
    {
        vector<string> orden = Split(ins[i], "=");
        for (size_t i = 0; i < orden[0].size(); i++){
            orden[0][i] = tolower(orden[0][i]);
        }
        if (orden[0] == "-s"){
            if (atoi(orden[1].c_str()) > 0){
                size = orden[1];
            }else{
                cout << "\033[1;" << "31" << "m[" <<"Tamaño indicado es menor o igual a 0" << "] " <<endl;

                aceptado = false;
            }
        }
        else if (orden[0] == "-f"){
            f = orden[1];
        }else if (orden[0] == "-u"){
            u = orden[1];
        }else if (orden[0] == "-t"){
            type = orden[1];
        }else if (orden[0] == "-delete"){
            del = orden[1];
        }
        else if (orden[0] == "-name"){
            name = orden[1];
        }else if (orden[0] == "-add"){
            add = orden[1];
        }else if (orden[0] == "-path"){
            if (CadenaContiene(orden[1], "\"") != 0){
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
            }
            path =rutaActual() + orden[1];
            if (orden[1] == "local"){
                path = rutaActual() + "/Disco sin nombre.dk";
            }
        }else{

            cout << "\033[1;" << "31" << "m[" <<"Comando:@@" << orden[0] << "@@No existente"<< "] " <<endl;
            aceptado = false;
        }
    }
    int multiplicador;
    if (u == "k" || u == "K"){
        multiplicador = 1024;
    }else if (u == "m" || u == "M"){
        multiplicador = 1048576;
    }else if (u == "b" || u == "B"){
        multiplicador = 1;
    }else{
        aceptado = false;
    }
    if (path != "" && name != "" && aceptado){
        //si no hay error en la entrada de datos se crean las carpetas
        int indice=-1;
        char sc[path.size()];
        strcpy(sc, path.c_str());
        //montar el disco
        MBR mbr;
        //inicializo el disco creado
        //mbr = InicialiarMBR_Particiones(mbr);
        mbr = RecuperarDisco(path);
        //realizar los cambios
        if (del != ""){ //eliminar particion
            //buscar la particion con el nombre dado  PRIMARIAS Y EXTENDIDAS
            bool existe=false;
            int indice_extendida=-1;
            for (size_t i = 0; i < 4; i++)
            {
                if(strcmp(mbr.mbr_partition[i].part_name, name.c_str())==0){
                    existe=true;
                    char respuesta = 'n';
                    cout << "\033[1;" << "34" << "m[" <<"La particion se eliminara SI(s)|NO(n)"<< "] " <<endl;
                
                    cin >> respuesta;
                    if (respuesta == 'S' || respuesta == 's'){
                        Partition vacia;
                        vacia.part_status='0';//status inactivo
                        vacia.part_type='-';
                        vacia.part_fit='-';
                        vacia.part_start=-1;
                        vacia.part_size=-1;
                        vacia.part_name[0] = '\0';
                        mbr.mbr_partition[i]=vacia;
                    }else{

                        cout << "\033[1;" << "31" << "m[" <<"Opcion invalida ...cancelando comando"<< "] " <<endl;
                    }
                }
                if(mbr.mbr_partition[i].part_type=='E'||mbr.mbr_partition[i].part_type=='e'){
                    indice_extendida=i;
                }
            }
            indice= indice_extendida;
            //Buscando en particiones logicas
            if(indice_extendida!=-1){//si existe una particion extendida
                int inicio = mbr.mbr_partition[indice_extendida].part_start;
                int anterior=-1;//inicio de la particion anterior
                int siguiente=-1;//inicio de la particion siguiente
                bool encontro=false;
                while(true){
                        EBR ebrImpr;
                        FILE *file = fopen(sc,"rb+");
                        fseek(file,inicio,SEEK_SET);//leemos el EBR que se encuentra en el ultimo indice
                        fread(&ebrImpr,sizeof(ebrImpr),1,file);
                        fclose(file);
                        if(strcmp(ebrImpr.part_name,name.c_str())==0){//verificamos si hay coincidencia con el nombre de la EBR
                            char respuesta = 'n';
                            cout << "\033[1;" << "34" << "m[" <<"La particion se eliminara SI(s)|NO(n)"<< "] " <<endl;

                            cin >> respuesta;
                            if (respuesta == 'S' || respuesta == 's'){// se confirmo la eliminacion
                                existe = true;
                                ebrImpr.part_status='0';//como el primer EBR no se elimina se marca como no activo
                                //verificando si hay un anterior-> si lo hay eliminamos el actual
                                if(anterior!=-1){//no es el EBR inicial-> se tienen que actualizar los punteros
                                    //leemos el EBR anterior para actualizar el puntero
                                    EBR ebrAnterior;
                                    FILE *file = fopen(sc,"rb+");
                                    fseek(file,anterior,SEEK_SET);
                                    fread(&ebrAnterior,sizeof(ebrAnterior),1,file);
                                    fclose(file);
                                    //verificamos si existe un puntero al siguiente EBR
                                    siguiente = ebrImpr.part_next;
                                    if(siguiente!=-1){//el puntero existe
                                        ebrAnterior.part_next = siguiente;//apuntamos el anterior al siguiente
                                    }else{//no hay un siguiente y part.next queda en -1
                                        ebrAnterior.part_next=-1;
                                    }
                                    //escribiendo el ebr anterior
                                    file=fopen(sc,"rb+");
                                    fseek(file,anterior,SEEK_SET);
                                    fwrite(&ebrAnterior,sizeof(ebrAnterior),1,file);
                                    fclose(file);
                                }

                                //escribir en disco la ebr
                                FILE *file = fopen(sc,"rb+");
                                fseek(file,ebrImpr.part_start,SEEK_SET);
                                fwrite(&ebrImpr, sizeof(ebrImpr), 1, file);
                                fclose(file);
                                break;
                            }else{

                                cout << "\033[1;" << "31" << "m[" <<"Comando cancelado"<< "] " <<endl;
                            }
                        }
                        //condicion de salida del while
                        if(ebrImpr.part_next!=-1){
                            anterior = inicio;
                            inicio = ebrImpr.part_next;
                        }else{
                            break;
                        }
                }

            }
            
            if(!existe){
  
                cout << "\033[1;" << "31" << "m[" <<"[ERROR->Particion no encontrada]"<< "] " <<endl;

            }
        }
        else if (add != ""){ //anañadir o disminuir espacio a la particion
            //buscando la particion
            bool existe=false;
            int indice_extendida=-1;
            for (size_t i = 0; i < 4; i++){
                //Guardando el indice de la particion extendida
                if(mbr.mbr_partition[i].part_type=='e'||mbr.mbr_partition[i].part_type=='E'){
                    indice_extendida=i;
                }
                if(strcmp(mbr.mbr_partition[i].part_name, name.c_str())==0){
                    existe=true;
                    //Verificando que el espacio añadidio no se mayor al espacio libre en el bloque actual
                    //OCUPADO-----PARTICION---LIBRE---OCUPADO
                    //no sea mayor que el segundo ocupado
                    //Buscando el indice que le sigue
                    int indice_maximo=mbr.mbr_tamano; 
                    for (size_t j = 0; j < 4; j++){
                        if(mbr.mbr_partition[i].part_start<mbr.mbr_partition[j].part_start){//si el inicio de la particion es la particion siguiente
                            //buscando el inicio de la particion inmediatamente despues
                            if((indice_maximo>mbr.mbr_partition[j].part_start || indice_maximo==mbr.mbr_tamano) && mbr.mbr_partition[j].part_start!=-1 ){
                                indice_maximo = mbr.mbr_partition[j].part_start;
                            }
                        }
                    }
                    //verificando que el nuevo tamaño este entre part_start y indice_maximo
                    int Tamano_Maximo=indice_maximo-mbr.mbr_partition[i].part_start;
                    if((atoi(add.c_str())*multiplicador+mbr.mbr_partition[i].part_size)>Tamano_Maximo){

                        cout << "\033[1;" << "31" << "m[" <<"[ERROR->No existe espacio disponible para aumentar]"<< "] " <<endl;
                    }else if((atoi(add.c_str())*multiplicador+mbr.mbr_partition[i].part_size)<0){
           
                        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> tamano ingresado invalido]"<< "] " <<endl;

                    }else{
                        mbr.mbr_partition[i].part_size=atoi(add.c_str())*multiplicador+mbr.mbr_partition[i].part_size;
                        cout << "\033[1;" << "32" << "m[" <<"Tamano del disco modificado exitosamente"<< "] " <<endl;
                    }
                }
            }
            //Buscando en particiones Logicas
            if(indice_extendida!=-1){//si existe una particion extendida
                int inicio = mbr.mbr_partition[indice_extendida].part_start;
                int tamanio= atoi(add.c_str())*multiplicador;
                while(true){
                            EBR ebrImpr;
                            FILE *file = fopen(sc,"rb+");
                            fseek(file,inicio,SEEK_SET);
                            fread(&ebrImpr,sizeof(ebrImpr),1,file);
                            fclose(file);
                            if(strcmp(ebrImpr.part_name,name.c_str())==0){
                                //se encontro la particion logica
                                bool condicion1=tamanio<=(ebrImpr.part_next-ebrImpr.part_start-ebrImpr.part_size);
                                bool condicion2 = (tamanio+ebrImpr.part_size)>=0;
                                if(tamanio<=(ebrImpr.part_next-ebrImpr.part_start-ebrImpr.part_size) && (tamanio+ebrImpr.part_size)>=0){
                                    //se añade el tamaño deseado VALUAR TAMAÑO NEGATIVO
                                    ebrImpr.part_size = ebrImpr.part_size+tamanio;
                                cout << "\033[1;" << "32" << "m[" <<"Tamano del disco modificado exitosamente"<< "] " <<endl;
                                    //escribiendo el ebrImpr
                                    FILE *file = fopen(sc,"rb+");
                                    fseek(file,ebrImpr.part_start,SEEK_SET);
                                    fwrite(&ebrImpr, sizeof(ebrImpr), 1, file);
                                    fclose(file);
                                    existe = true;
                                    break;
                                }else if((tamanio+ebrImpr.part_size)<0){
                        
                                      cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Tamano resultante es negativo]"<< "] " <<endl;
                                }else{
                              
                                    cout << "\033[1;" << "31" << "m[" <<"[ERROR-> No hay espacio suficiente para aumentar]"<< "] " <<endl;

                                }
                            }
                            //condicion de salida del while
                            if(ebrImpr.part_next!=-1){
                                inicio = ebrImpr.part_next;
                            }else{
                                break;
                            }
                    }
            }
            
            if(!existe){
  
                cout << "\033[1;" << "31" << "m[" <<"[ERROR-> No se pudo agregar espacion. Particion no encotrada]"<< "] " <<endl;

            }
        }else if (size!="" && (type=="P" || type=="E" || type=="p" || type=="e") ){ //crear particion extendida o primaria
            int tamanio = atoi(size.c_str())*multiplicador;//tamaño deseado de la nueva particion
            //buscando un tamaño disponible en el disco
            int indice=-1;
            //verificando que exista una posicion en el arreglo libre
            for (int i = 0; i < 4; i++)
            {
                if(mbr.mbr_partition[i].part_status=='0' && indice ==-1){
                    indice=i;
                    
                }
            }
            if(indice==-1){

                cout << "\033[1;" << "31" << "m[" <<"[ERROR-> no se pudo agregar espacion. Particion no encotrada]"<< "] " <<endl;

                
            }
            //buscando que el nombre de la particion sea diferente
            for (int i = 0; i < 4; i++)
            {
                if(strcmp(mbr.mbr_partition[i].part_name,name.c_str())==0){

                    cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Ya existe una particion con el mismo nombre]"<< "] " <<endl;

                    indice=-1;
                }
            }
            //verificando que no se creen mas de 1 particion extendida
            if (tolower(type[0])=='e'){
                for (size_t i = 0; i < 4; i++)
                {
                    if(mbr.mbr_partition[i].part_type==tolower(type[0])){
                        indice=-1;
                        
                        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Ya existe una particion Extendida]"<< "] " <<endl;

                    }
                }
                
            }
            //si encontro un espacio
            if(indice!=-1){
                int inicio= sizeof(mbr);
                int fin=mbr.mbr_tamano;
                bool Hay_Espacio=false;//hay espacio en el disco para la particion
                if((fin-inicio)>=tamanio){
                            Hay_Espacio=true;
                }
                bool entro=false;
                for (size_t i = 0; i < 4; i++)
                {
                    if(mbr.mbr_partition[i].part_status!='0'&& !entro){
                        fin = mbr.mbr_partition[i].part_start;
                        if((fin-inicio)>=tamanio){//busca espacio entre la particion activa y el final del mbr
                            entro=true;
                            Hay_Espacio=true;

                        }else{//buscar otro espacio
                            //inicia desde el final de la particion 
                            inicio= mbr.mbr_partition[i].part_start + mbr.mbr_partition[i].part_size; 
                            fin = mbr.mbr_tamano;
                            if((fin-inicio)>=tamanio){
                                Hay_Espacio=true;
                            }else{
                                Hay_Espacio=false;
                            }
                        }
                    }
                }
                if(Hay_Espacio){
                    Partition parti;
                    strcpy(parti.part_name,name.c_str());
                    parti.part_fit=f[0];
                    parti.part_size=tamanio;
                    parti.part_start=inicio;
                    parti.part_status='1';
                    parti.part_type= tolower(type[0]);
                    mbr.mbr_partition[indice]=parti;
                    //si la particion es extendida hay que escribir el EBR al inicio de la particion
                    if(tolower(type[0])=='e'){
                        EBR ebr = InicializarEBR(ebr);
                        ebr.part_start=inicio;
                        char sc[path.size()];
                        strcpy(sc, path.c_str());
                        //abriendo el archivo sin alterar
                        FILE *file = fopen(sc, "rb+");
                        //coloco el punteron en el inicio de la particion extendida
                        fseek(file, inicio, SEEK_SET);
                        //escribiendo el objeto
                        fwrite(&ebr, sizeof(ebr), 1, file);
                        //cierro el archivo
                        fclose(file);
                       
                        cout << "\033[1;" << "32" << "m[" <<"[EBR inicial de Particion Extendida creada]"<< "] " <<endl;

                    }
        
                        cout << "\033[1;" << "32" << "m[" <<"[Particion creada exitosamente]"<< "] " <<endl;

                }else{
                    
                    cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Insuficiente espacio en el disco para crear la particion]"<< "] " <<endl;

                }
            }
        }else if (size!="" && (type=="L" || type=="l") ){//Particion Logica
                    //Verificando que existe una particion extendida
                    for (size_t i = 0; i < 4; i++)
                    {
                        if(mbr.mbr_partition[i].part_type=='e'){
                            indice=i;
                        }
                    }
                    if(indice==-1){
                        
                        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> No existe ninguna particion extendida]"<< "] " <<endl;
                        
                        return;
                    }
                    int inicio = mbr.mbr_partition[indice].part_start;
                    //verificando que el nombre de la particion no exista
                    //verificando el espacio de la suma de particiones
                    int espacioUsado=0;
                    while(true){
                            //recuperando el ebr de la posicion indicada
                            EBR ebrImpr;
                            char sc[path.size()];
                            strcpy(sc, path.c_str());
                            FILE *file = fopen(sc,"rb+");
                            fseek(file,inicio,SEEK_SET);
                            fread(&ebrImpr,sizeof(ebrImpr),1,file);
                            fclose(file);
                            if(strcmp(ebrImpr.part_name,name.c_str())==0){
                                indice=-1;
                                cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Nombre de la particion ya existe]"<< "] " <<endl;

                            }
                            espacioUsado+=sizeof(EBR)+ebrImpr.part_size;//sumar ebr---------------------
                            //condicion de salida del while
                            if(ebrImpr.part_next!=-1){
                                inicio = ebrImpr.part_next;
                            }else{
                                break;
                            }
                    }
                    espacioUsado+=atoi(size.c_str())*multiplicador;
                    if(espacioUsado>mbr.mbr_partition[indice].part_size){
                        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> No hay suficiente espacio en la particion extendida para crear la particion logica"<< "] " <<endl;


                        indice=-1;
                    }
                    if(indice!=-1){//Existe una particion extendida
                        //Recuperando el EBR inicial
                        int tamanio = atoi(size.c_str())*multiplicador;
                        EBR ebr;
                        bool creada=false;
                        ebr.part_start = mbr.mbr_partition[indice].part_start;
                        int ebrSiguente = ebr.part_start;
                        int FinLogicas = -1;
                        while(!creada){
                            FILE *file = fopen(sc,"rb+");
                            fseek(file,ebrSiguente,SEEK_SET);
                            fread(&ebr,sizeof(ebr),1,file);
                            fclose(file);
                            if(ebr.part_status=='0'){//la particion esta desocupada
                                if(tamanio <=(sizeof(ebr)+ebr.part_size)){//hay espacio en la particion desocupada eliminada previamente 
                                    //creando la particionen dicho espacio
                                    ebr.part_status='1';
                                    ebr.part_size= tamanio;
                                    ebr.part_fit= f[0];
                                    strcpy(ebr.part_name,name.c_str());
                                    creada=true;
                                }else if(ebr.part_next==-1){//se trata de la primer o ultimo ebr desocupado creado
                                    //(fin de la particion) - (inicio de la particion vacia)+(sizeof(ebr)) ---> tamaño disponible
                                    if(tamanio <=((mbr.mbr_partition[indice].part_start + mbr.mbr_partition[indice].part_size) - (ebr.part_start)+sizeof(ebr))){
                                        //se crea la particion en ese espacio
                                        ebr.part_status='1';
                                        if(ebr.part_start==-1){//es la primera particion
                                            ebr.part_start = mbr.mbr_partition[indice].part_start;
                                        }
                                        ebr.part_size = tamanio;
                                        strcpy(ebr.part_name,name.c_str());
                                        ebr.part_fit= f[0];
                                        creada=true;
                                    }
                                }
                            }
                            //VERIFICANDO SI HAY ESPACIO ENTRE EL FINAL DE LA PARTICION Y EL INICIO DE LA OTRA
                            if(ebr.part_next!=-1){
                                if((ebr.part_next-ebr.part_start-ebr.part_size)>=tamanio){//verificando si hay espacio entre particiones
                                    //actualizando el puntero actual a la siguiente particion
                                    EBR nueva = InicializarEBR(nueva);
                                    strcpy(nueva.part_name , name.c_str());
                                    nueva.part_start = ebr.part_size+ebr.part_start;
                                    nueva.part_status='1';
                                    nueva.part_size=tamanio;
                                    nueva.part_next = ebr.part_next;
                                    ebr.part_next = nueva.part_start;
                                    ebr.part_fit=f[0];
                                    //la particion ebr se escribe depues en el if(creada)
                                    //escribiendo la particion nueva
                                    FILE *file = fopen(sc,"rb+");
                                    fseek(file,nueva.part_start,SEEK_SET);
                                    fwrite(&nueva, sizeof(nueva), 1, file);
                                    fclose(file);
                                    creada= true;
                                }
                            }
                            //condicion de salida del while si no se ha creado una particion
                            if(ebr.part_next!=-1){
                                ebrSiguente = ebr.part_next;
                            }else{
                                FinLogicas =ebr.part_start + ebr.part_size;
                                break;
                            }
                        }
                        if(!creada){//si no existe un espacio libre y no es la primera-> Crear una nueva
                            EBR nueva = InicializarEBR(ebr);
                            nueva.part_start=FinLogicas;
                            nueva.part_status='1';
                            nueva.part_size=tamanio;
                            nueva.part_next=-1;
                            strcpy(nueva.part_name , name.c_str());
                            nueva.part_fit = f[0];
                            //escribiendo en el disco
                            FILE *file = fopen(sc,"rb+");
                            fseek(file,FinLogicas,SEEK_SET);
                            fwrite(&nueva, sizeof(nueva), 1, file);
                            fclose(file);
                            //escribiendo el puntero anterior
                            file = fopen(sc,"rb+");
                            ebr.part_next= nueva.part_start;
                            fseek(file,ebr.part_start,SEEK_SET);
                            fwrite(&ebr, sizeof(ebr), 1, file);
                            fclose(file);
                            
                            

                        }
                        if (creada){ //se escribe el ebr si ya se creo   
                            FILE *file = fopen(sc,"rb+");
                            fseek(file,ebr.part_start,SEEK_SET);
                            fwrite(&ebr, sizeof(ebr), 1, file);
                            fclose(file);
                            
                        }
                        //actaulizando la particion anterior
                           
                        
                        
                    }    
        }
        //escribir el disco
        AcutalizarDisco(path,mbr);
        //REPORTE DEL DISCO
        ImprimirDisco(mbr);
        cout<<"Informacion de ebr"<<endl;
        cout << "\033[1;" << "32" << "m[" <<"*********[Informacion de ebr]***********"<< "] " <<endl;

        if(indice!=-1){
            //Imprimiendo las EBR
            int inicio = mbr.mbr_partition[indice].part_start;
            int contador=0;
            while(true){
                cout<<"EBR numero: "<<contador<<endl;
        cout << "\033[1;" << "32" << "m[" <<"####[EBR numero: "<<contador<< "]] " <<endl;

                contador++;
               
                EBR ebrImpr;
                FILE *file = fopen(sc,"rb+");
                fseek(file,inicio,SEEK_SET);
                fread(&ebrImpr,sizeof(ebrImpr),1,file);
                fclose(file);

                cout<<"name  : "  <<ebrImpr.part_name<<endl;
                cout<<"state : "  <<ebrImpr.part_status<<endl;
                cout<<"start : "  <<ebrImpr.part_start<<endl;
                cout<<"size  : "  <<ebrImpr.part_size<<endl;
                cout<<"next  : "  <<ebrImpr.part_next<<endl;
                cout<<"fit   : "  <<ebrImpr.part_fit<<endl;
                //condicion de salida del while
                if(ebrImpr.part_next!=-1){
                    inicio = ebrImpr.part_next;
                }else{
                    break;
                }
            }
        }
    }else{

        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Nombre de la particion ya existe]"<< "] " <<endl;

        
            
    }
}



