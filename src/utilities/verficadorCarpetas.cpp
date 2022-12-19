#ifndef _VERIFICARCARPETA_
#define _VERIFICARCARPETA_

#include <dirent.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../utilities/split.cpp"
using namespace std;

//verifica que las carpetas del path esten creadas
void VerificarCarpetas(string path){
    //dividiendo el string en carpetas 
    vector<string> carpetas = Split(path,"/");
    string Nombre_Directorio = carpetas[1];
    for (size_t i = 2; i < carpetas.size(); i++)
    {
        //creando la carpeta espesifica
        DIR *pDir = opendir(Nombre_Directorio.c_str());
        bool Exists = false;
        if(pDir !=NULL){
            Exists = true;
            (void) closedir(pDir);
        }
        if(!Exists){
            int response;
            const char *Directorio_Char = Nombre_Directorio.c_str();
            response = mkdir(Directorio_Char,0777);
            if(response == -1){
                cout<<"Error al crear la carpeta: "<<Nombre_Directorio<<endl;
            }else{
                cout<<"la carpeta: "<<Nombre_Directorio<<" se ha creado "<<endl;
            }
        }
        Nombre_Directorio+="/"+carpetas[i];
    }  
}
#endif