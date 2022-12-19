#ifndef _IMPRIMIRDISCO_
#define _IMPRIMIRDISCO_
#include <iostream>
#include "../estructuras/Mbr.struct.cpp"
using namespace std;

void ImprimirDisco(MBR mbr){

    cout << "\033[1;" << "33" << "m[" <<"#########DATOS DEL DISCO#########"<< "] " <<endl;

    cout<<"Tamaño: "<<mbr.mbr_tamano<<endl;


    cout << "\033[1;" << "34" << "m[" <<"Particiones Primarias y Extendidas:"<< "] " <<endl;
    
    for (size_t i = 0; i < 4; i++){   
        cout<<i<<")"<<endl;
        if(mbr.mbr_partition[i].part_status!='0'){
            cout << "\033[1;" << "32" << "m[" <<"Type: "<<mbr.mbr_partition[i].part_type << "] " <<endl;
            cout << "\033[1;" << "32" << "m[" <<"Fit: "<< mbr.mbr_partition[i].part_fit << "] " <<endl;
            cout << "\033[1;" << "32" << "m[" <<"Start: "<<mbr.mbr_partition[i].part_start << "] " <<endl;
            cout << "\033[1;" << "32" << "m[" <<"Size: "<<mbr.mbr_partition[i].part_size << "] " <<endl;
            cout << "\033[1;" << "32" << "m[" <<"Name: "<<mbr.mbr_partition[i].part_name << "] " <<endl;
            
          
        }
    }
}
#endif