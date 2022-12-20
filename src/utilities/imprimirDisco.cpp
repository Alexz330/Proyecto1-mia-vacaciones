#ifndef _IMPRIMIRDISCO_
#define _IMPRIMIRDISCO_
#include <iostream>
#include "../estructuras/Mbr.struct.cpp"
using namespace std;

void ImprimirDisco(MBR mbr){

    cout << "\033[1;" << "33" << "m[" <<"****************Datos del disco************"<< "] " <<endl;

    cout<<"Tamaño: "<<mbr.mbr_tamano<<endl;


    cout << "\033[1;" << "34" << "m[" <<"***********Particiones Primarias y Extendidas***********"<< "] " <<endl;
    
    for (size_t i = 0; i < 4; i++){   
        cout<<i<<")"<<endl;
        if(mbr.mbr_partition[i].part_status!='0'){
            cout << "\033[1;" << "32" << "m[" <<"tipo  :  "<<mbr.mbr_partition[i].part_type << "] " <<endl;
            cout << "\033[1;" << "32" << "m[" <<"fit   :  "<< mbr.mbr_partition[i].part_fit << "] " <<endl;
            cout << "\033[1;" << "32" << "m[" <<"inicio:  "<<mbr.mbr_partition[i].part_start << "] " <<endl;
            cout << "\033[1;" << "32" << "m[" <<"tamano:  "<<mbr.mbr_partition[i].part_size << "] " <<endl;
            cout << "\033[1;" << "32" << "m[" <<"nombre:  "<<mbr.mbr_partition[i].part_name << "] " <<endl;       
          
        }
    }
}
#endif