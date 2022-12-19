
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "../utilities/split.cpp"
#include "../utilities/rutaActual.cpp"
#include "../estructuras/Mbr.struct.cpp"
#include "../utilities/verficadorCarpetas.cpp"
#include "../utilities/imprimirDisco.cpp"


using namespace std;

class MkDisk{
    public:
        static void CreateDisk(vector <string>);
       
};

void ReadDisk(string path, MBR mbr){
    char sc[path.size()];
    strcpy(sc, path.c_str());
    //colocando fecha e identificador al disco
    mbr.mbr_fecha_creacion = time(0);
    //abriendo el archivo en formato escritura
    FILE *file = fopen(sc, "wb");
    //coloco el punteron en el inicio
    fseek(file, 0, SEEK_SET);
    //escribiendo el objeto
    fwrite(&mbr, sizeof(mbr), 1, file);
    //Colocando el puntero en el tamaño deseado
    fseek(file, mbr.mbr_tamano - 1, SEEK_SET);
    //escribo algo en el puntero para mantener su tamaño
    fwrite("\0", 1, 1, file);
    //cierro el archivo
    fclose(file);
    cout << "Disco creado en: " << sc << endl;
}


void MkDisk::CreateDisk(vector <string> ins){
      //excribir un archivo
    //buscando ordenes obligatorias
    MBR mbr;
    bool aceptado = true;
    string size = "";
    char f = 'B';
    string u = "m";
    string path = "";
    string pathAux="";
    for (size_t i = 1; i < ins.size(); i++)
    {
        vector<string> orden = Split(ins[i], "=");
        for (size_t i = 0; i < orden[0].size(); i++)
        {
            orden[0][i] = tolower(orden[0][i]);
        }
        if (orden[0] == "-s")
        {
            if (atoi(orden[1].c_str()) > 0)
            {
                size = orden[1];
            }
            else
            {
                cout << "Tamaño indicado es 0 o negativo" << endl;
                aceptado = false;
            }
        }
        else if (orden[0] == "-f")
        {
            f = orden[1][0];
        }
        else if (orden[0] == "-u")
        {
            u = orden[1];
        }
        else if (orden[0] == "-path")
        {
            if (CadenaContiene(orden[1], "\"") != 0)
            {
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
            }
            pathAux = orden[1];
            path =rutaActual() + orden[1];
            if (orden[1] == "local")
            {
                path = rutaActual() + "/Disco sin nombre.dk";
            }
        }
        else
        {
            cout << "Comando:_" << orden[0] << "_No existente" << endl;
            aceptado = false;
        }
    }
    int multiplicador;
    if (u == "k" || u == "K")
    {
        multiplicador = 1024;
    }
    else if (u == "m" || u == "M")
    {
        multiplicador = 1048576;
    }
    else
    {
        aceptado = false;
        cout << "Comando: " << u << "No existe" << endl;
    }
    if (size != "" && path != "" && aceptado)
    {
        if(pathAux!="local"){
            VerificarCarpetas(pathAux);
        }
        cout<<"VERIFICACION DE CARPETAS TERMINADA"<<endl;
        mbr = InicialiarMBR_Particiones(mbr);
        mbr.mbr_tamano= atoi(size.c_str()) * multiplicador;
        mbr.disk_fit=f;
        ReadDisk(path, mbr);
        ImprimirDisco(mbr);
        cout << "Disco Creado correctamente" << endl;
    }
    else
    {
        cout << "Error en la entrada de datos" << endl;
    }
}

