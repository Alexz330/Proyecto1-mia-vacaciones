
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "../Globales/particionesGlobales.cpp"
#include "../utilities/split.cpp"
using namespace std;

class UMount{
    public:
        UMount();
       static void Desmontar(vector<string>);
};

void UMount::Desmontar(vector<string> ins){
        bool aceptado = true;
    string id = "";  //id de disco montado carnet numero letra
    for (size_t i = 1; i < ins.size(); i++)
    {
        vector<string> orden = Split(ins[i], "=");
        for (size_t i = 0; i < orden[0].size(); i++)
        {
            orden[0][i] = tolower(orden[0][i]);
        }
        
        if (orden[0] == "-id")
        {
            id = orden[1];
        }else{
            cout << "Comando:_" << orden[0] << "_No existente" << endl;
            aceptado = false;
        }
    }
    if (id != ""&& aceptado){
        //verificar que el disco ya este montado antes
        for (size_t i = 0; i < particiones_montadas.size(); i++)
        {
            string identificador=to_string(particiones_montadas[i].carnet) +to_string(particiones_montadas[i].numero) +particiones_montadas[i].letra;
            if(strcmp(id.c_str(),identificador.c_str())==0){
                aceptado=false;
                particiones_montadas.erase(particiones_montadas.begin()+i);
                cout<<"Particion: "<<identificador<<" Desmontada"<<endl;
            }
        }
        if (aceptado)
        {
            cout<<"Particion no Montada"<<endl;
            
        }
    }
    //Imprimiendo las particiones montadas
    for (size_t i = 0; i < particiones_montadas.size(); i++){
        cout<<particiones_montadas[i].path<<"|"<<particiones_montadas[i].nombre<<"|"<<particiones_montadas[i].carnet<<particiones_montadas[i].numero<<particiones_montadas[i].letra<<endl;
    }
}