
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

class RmDisk{
    public:
        RmDisk();
        static void DeleteDisk(vector<string>);
};

void EliminarArchivo(string ruta, string nombre)
{
    char nuevaruta[ruta.size() + nombre.size() + 1];
    for (size_t i = 0; i < ruta.size(); i++)
    {
        nuevaruta[i] = ruta[i];
    }
    //for (size_t i = 0; i < nombre.size(); i++)
    //{
     //   nuevaruta[i + ruta.size()] = nombre[i];
    //}
    if (remove(nuevaruta) == 0)
    {
        cout << "Archivo borrado correctamente" << endl;
    }
    else
    {
        cout << "Archivo no encontrado: " <<nuevaruta << endl;
    }
}


void RmDisk::DeleteDisk(vector<string> ins){
    bool aceptado = true;
    string path = "";
    for (size_t i = 1; i < ins.size(); i++)
    {
        vector<string> orden = Split(ins[i], "=");
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
    if (path != "" && aceptado)
    {
        EliminarArchivo(path, "");
        //cout << "Disco Eliminado correctamente" << endl;
    };
}