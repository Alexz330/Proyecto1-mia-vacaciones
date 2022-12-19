
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class Pause{
    public:
        static void pausar();
};

void Pause::pausar(){
    string entrada;
    cin>>entrada;
}