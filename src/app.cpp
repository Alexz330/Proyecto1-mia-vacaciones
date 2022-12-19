
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "./Analizador/Analizador.cpp"

using namespace std;

int main(){

    Analizador ins;     
    while (true)
    {
        vector<string> instrucciones = ins.ObtenerArrlegloInstrucciones();
        //podiendo instruccion principal a mayusculas
        for (size_t i = 0; i < instrucciones[0].size(); i++)
        {
            instrucciones[0][i] = toupper(instrucciones[0][i]);
        }
        ins.ExecInstruciones(instrucciones);
        
    }

    return 0;
}