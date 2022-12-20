
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "../utilities/rutaActual.cpp"
#include "../utilities/split.cpp"
//Exportantdo comandos
#include "../Comandos/MkDisk.cpp"
#include "../Comandos/RmDisk.cpp"
#include "../Comandos/FDisk.cpp"

#include "../Comandos/MKFS.cpp"
#include "../Comandos/Mount.cpp"
#include "../Comandos/UMount.cpp"
#include "../Comandos/Pause.cpp"
#include "../Comandos/Rep.cpp"




using namespace std;

class Analizador{
    public:
        vector<string> instrucciones;

        Analizador();

        void ExecInstruciones(vector<string>);
        vector<string> ObtenerArrlegloInstrucciones();
        void LeerIntrucciones(vector<string>);
};


Analizador::Analizador(){

}
void Analizador::ExecInstruciones(vector<string> instrucciones){
    string comando =  instrucciones[0];
    //inicio de comprobaciones
    if (comando == "MKDISK")
    {
        MkDisk::CreateDisk(instrucciones);
    }
    else if (comando == "RMDISK")
    {
        RmDisk::DeleteDisk(instrucciones);
    }
    else if (comando == "FDISK")
    {
        FDisk::AdminPartitions(instrucciones);
    }else if(comando=="MOUNT"){
        Mount::Montar(instrucciones);
    }else if(comando=="UNMOUNT"){
        UMount::Desmontar(instrucciones);
    }else if(comando=="MKFS"){
        MkFs::Formatear(instrucciones);
    }else if(comando=="EXEC"){
        this->LeerIntrucciones(instrucciones);
    }else if(comando=="PAUSE"){
        Pause::pausar();
    }else if(comando=="REP"){
        Rep::Reporte(instrucciones);
    }else

    {
        cout << "\033[1;" << "31" << "m[" <<"[Commando invalido]"<< "] " <<endl;


    }
 
};

vector<string> Analizador::ObtenerArrlegloInstrucciones(){


    cout << "\033[1;" << "34" << "m[" <<"Ingrese nuevo comando" << "] " <<endl;

    string entrada;
    getline(cin, entrada);
 
    vector<string> EntradaDividida = Split(entrada, " ");
    return EntradaDividida;
}

void Analizador::LeerIntrucciones(vector<string>instruccion){
     cout << "\033[1;" << "34" << "m[" <<"Leyendo instruciones" << "] " <<endl;

    string path = ""; 
    bool aceptado=true;
    for (size_t i = 1; i < instruccion.size(); i++)
    {
        vector<string> orden = Split(instruccion[i], "=");
        for (size_t i = 0; i < orden[0].size(); i++)
        {
            orden[0][i] = tolower(orden[0][i]);
        }
        if (orden[0] == "-path")
        {
            if (CadenaContiene(orden[1], "\"") != 0)
            {
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
            }
            path = orden[1];
            if (orden[1] == "local")
            {
                path = rutaActual() + "/Disco sin nombre.dsk";
            }
        }
        else
        {
               
            
            cout << "\033[1;" << "31" << "m[" <<"Comando:@@" << orden[0] << "@@No existente"<< "] " <<endl;
 
            aceptado = false;
        }
    }

    ifstream archivo(path.c_str());
    string linea;
   
    while (getline(archivo, linea)) {
        if(linea=="\r"||linea=="\n"){//no importa

        }else if(linea==""){

        }else if(linea[0]=='#'){//se trata de un comentario y solo se imprime
            //break;

        }else{//se trata de una instruccion
            cout << "\033[1;" << "32" << "m[" <<"Comando:"<<linea<<"@@"<< "] " <<endl;
       
            vector<string> EntradaDividida = Split(linea, " ");
            
            for (size_t i = 0; i < EntradaDividida[0].size(); i++)
            {
                EntradaDividida[0][i] = toupper(EntradaDividida[0][i]);
            }
            //eliminando comentarios entre lineas 
            vector<string> EntradaDividida2;
            for (size_t i = 0; i < EntradaDividida.size(); i++)
            {
                if(EntradaDividida[i][0]!='#'){
                    EntradaDividida2.push_back(EntradaDividida[i]);
                }else{
                    break;
                }
            }
            this->ExecInstruciones(EntradaDividida2);
        }
    }
}