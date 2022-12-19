
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "../utilities/split.cpp"
#include "../utilities/rutaActual.cpp"
#include "../estructuras/Mbr.struct.cpp"
#include "../estructuras/Ebr.struct.cpp"
#include "../estructuras/PMontadas.struct.cpp"
#include <set>
#include "../utilities/RecuperacionDisco.cpp"

#include "../Globales/particionesGlobales.cpp"

class Mount{
    public:
        Mount();
        static void Montar(vector<string>);


};


void Mount::Montar(vector<string> ins){
     bool aceptado = true;
    string path = "";  //Ruta del disco en donde se encuentra la particion
    string name = "";  //Nombre de la particion en el disco
    for (size_t i = 1; i < ins.size(); i++)
    {
        vector<string> orden = Split(ins[i], "=");
        for (size_t i = 0; i < orden[0].size(); i++)
        {
            orden[0][i] = tolower(orden[0][i]);
        }
        
        if (orden[0] == "-name")
        {
            name = orden[1];
        }
        else if (orden[0] == "-path")
        {
            if (CadenaContiene(orden[1], "\"") != 0)
            {
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
            }
            path =rutaActual() + orden[1];
            if (orden[1] == "local")
            {
                path = rutaActual() + "Disco sin nombre.dk";
            }
        }
        else
        {
            cout << "Comando:_" << orden[0] << "_No existente" << endl;
            aceptado = false;
        }
    }
    if (path != "" && name != "" && aceptado){
        //verificar que el disco no halla sido montado antes
        for (size_t i = 0; i < particiones_montadas.size(); i++)
        {
            if(strcmp(particiones_montadas[i].nombre,name.c_str())==0){
                cout<<"La particion "<<name<<" ya ha sido montada-->"<<particiones_montadas[i].nombre<<endl;
                aceptado=false;
            }
        }
        if (aceptado)
        {
            //ESTO DE ABAJO OBTIENE LA LETRA DEL DISCO Y EL NUMERO DE PARTICION
            //EN BASE A,
            //ENTRADA(path ->ruta del nuevo disco,nombre_particion)  NUEVO DISCO A MONTAR
            //PARTICIONESMONTADAS(nombreParticion,rutaDisco,letraDisco,numeroParticion).
            const char letras[]={"abcdefghijklmnopqrstuvwxyz"};
            //int contador_=0;//indice de discos en letras
            //int numero=0; //numero de particion
            char nuevaLetra='+';
            int id_letra=0;
            //Carnet1 Carnet2 NumeroParticion LetraDisco
            for (size_t i = 0; i < particiones_montadas.size(); i++)
            {
                if(strcmp(particiones_montadas[i].path,path.c_str())==0){
                    nuevaLetra = particiones_montadas[i].letra[0];
                    id_letra = particiones_montadas[i].numero;
                    break;
                }
            }
            if(nuevaLetra == '+'){ //el disco no existe y hay que asignarle una nueva letra
                set<string> sinrepetir;
                for (size_t i = 0; i < particiones_montadas.size(); i++)
                {
                    sinrepetir.insert(particiones_montadas[i].path);
                }
                nuevaLetra = letras[sinrepetir.size()];
                id_letra = sinrepetir.size()+1;
            }
            //obtenemos el numero de particion de ese disco
            int numero =1;
            for (size_t i = 0; i < particiones_montadas.size(); i++)
            {
                if(strcmp(particiones_montadas[i].path,path.c_str())==0){
                    numero++;
                }
            }





            ////ESTO DE ARRIBA OBTIENE EL NUMERO Y LA LETRA DEL DISCO Y PARTICION
            //recuperar el disco para verificar que exista la particion
            MBR mbr = RecuperarDisco(path);
            //recorriendo el disco en busca de la particion
            bool Existe=false;
            int IndiceExtendida=-1;
            for (size_t i = 0; i < 4; i++)
            {
                if(strcmp(mbr.mbr_partition[i].part_name,name.c_str())==0){
                    Existe=true;
                }
                if(mbr.mbr_partition[i].part_type=='e'){
                    IndiceExtendida = i;
                }
            }
            //recorriendo las EBR para las particiones logicas
            char sc[path.size()];
            strcpy(sc, path.c_str());
            int inicio = mbr.mbr_partition[IndiceExtendida].part_start;
            if(IndiceExtendida!=-1){//existe la particion extendida
                while(true){
                        EBR ebrImpr;
                        FILE *file = fopen(sc,"rb+");
                        fseek(file,inicio,SEEK_SET);//leemos el EBR que se encuentra en el ultimo indice
                        fread(&ebrImpr,sizeof(ebrImpr),1,file);
                        fclose(file);
                        if(strcmp(ebrImpr.part_name,name.c_str())==0){//verificamos si hay coincidencia con el nombre de la EBR
                                Existe = true;
                                break;
                        }
                        //condicion de salida del while
                        if(ebrImpr.part_next!=-1){
                            inicio = ebrImpr.part_next;
                        }else{
                            break;
                        }
                }
            }


            if(Existe){
                //montando la pariticion nueva
                ParticionesMontadas pa;
                strcpy(pa.nombre,name.c_str());
                strcpy(pa.path,path.c_str());
                pa.numero=id_letra;
                pa.letra[0]=letras[numero-1];
                particiones_montadas.push_back(pa);
                cout<<"Particion Montada"<<endl;
            }else{
                cout<<"Particion no Existente en el disco"<<endl;
            }
            
        }
    }
    //Imprimiendo las particiones montadas
        for (size_t i = 0; i < particiones_montadas.size(); i++)
        {
            cout<<particiones_montadas[i].path<<"|"<<particiones_montadas[i].nombre<<"|"<<particiones_montadas[i].carnet<<particiones_montadas[i].numero<<particiones_montadas[i].letra<<endl;
        }
}

