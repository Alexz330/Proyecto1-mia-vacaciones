
#include <vector>
#include <iostream>
#include <string>
#include <string.h>

#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>


#include "../Globales/particionesGlobales.cpp"
#include "../utilities/split.cpp"
#include "../utilities/verficadorCarpetas.cpp"
#include "../utilities/rutaActual.cpp"
#include "../estructuras/Mbr.struct.cpp"
#include "../estructuras/Ebr.struct.cpp"
#include "../estructuras/SuperBloque.struct.cpp"




using namespace std;
void reporteDISK(string path,string pathDestino){
    //recuperando el EBR de la ruta 
    MBR mbr = InicialiarMBR_Particiones(mbr);
    char sc[path.size()];
    strcpy(sc, path.c_str());
    FILE *arch = fopen(sc,"rb+");
    fread(&mbr,sizeof(mbr),1,arch);
    //Iniciando con el reporte en graphviz con el MBR
    string dot = "digraph G { \nconcentrate=True; \nrankdir=TB; bgcolor=\"#a5a5a5\";\n \nnode [shape=record];\n";
    dot+="MBR[label = \"MBR ";
    //ordenando el arreglo de particiones
    Partition aux[4];
    for (size_t i = 0; i < 4; i++)
    {
        aux[i]=mbr.mbr_partition[i];
    }
    //ordenando por el metodo burbuja
    Partition temporal;
	for (int i = 0;i < 4; i++){
		for (int j = 0; j< 4-1; j++){
			if (aux[j].part_start > aux[j+1].part_start){ // Ordena el array de mayor a menor, cambiar el "<" a ">" para ordenar de menor a mayor
			temporal = aux[j]; 
			aux[j] = aux[j+1]; 
			aux[j+1] = temporal;
			}
		}
	}
    //agregamos | particion o espacio libre en el disco hasta llegar a la extendida
    for (size_t i = 0; i < 4; i++)
    {
        float usado = (float)aux[i].part_size / (float)mbr.mbr_tamano;
        usado = usado*100;
        string nombre =aux[i].part_name; 
        //graficando las particiones logicas
        if(aux[i].part_type=='e'){//si es extendida
            dot+="| {extentida:"+ nombre +"\\n"+to_string(usado) +"% ";
            dot+=" | {";
            int posicionEBR = aux[i].part_start;
            int posicionFinal = aux[i].part_size;
            int finUltimaEBR = -1;
            while(true){
                //obteniendo el ebr
                fseek(arch,posicionEBR,SEEK_SET);
                EBR ebr;
                fread(&ebr,sizeof(ebr),1,arch);
                //colocando EBR
                dot+=" EBR ";
                //colocando la particion logica
                if(ebr.part_status!='0'){//si la particion esta activa
                    float porcentaje = (float)ebr.part_size/(float)mbr.mbr_tamano;
                    porcentaje = porcentaje *100;
                    string nombre = ebr.part_name;
                    if(ebr.part_next!=-1){
                        dot+=" | "+nombre+" \\n"+to_string(porcentaje)+"% |";
                    }else{
                        dot+=" | "+nombre+" \\n"+to_string(porcentaje)+"% ";
                    }
                    
                    //espacio disponible entre logicas
                    if(ebr.part_next!=-1){
                        int espacio = ebr.part_start+ebr.part_size;
                        if(ebr.part_next - espacio >1){//hay espacio entre particiones
                            porcentaje = (float)(ebr.part_next-espacio)/(float)mbr.mbr_tamano;
                            porcentaje = porcentaje *100;
                            dot+=" Espacio Libre \\n"+to_string(porcentaje)+"% |";
                        }
                    }
                }
                //Condicion de salida del while
                if(ebr.part_next!=-1){
                    posicionEBR = ebr.part_next;
                }else{
                    finUltimaEBR =ebr.part_start + ebr.part_size;
                    break;
                }
                
            }
            //espacio libre de la particion extendida
            if(finUltimaEBR!=-1){
                int fin = aux[i].part_start + aux[i].part_size;
                if((fin - finUltimaEBR) >1){
                    float porcentaje = (float)(fin - finUltimaEBR)/(float)(mbr.mbr_tamano);
                
                    porcentaje = porcentaje*100;
                    dot+="| Espacio Libre \\n"+ to_string(porcentaje)+"% ";
                }
            }
            dot+=" }} ";
        }else{
            if(usado >0.01){
                dot+="| "+ nombre +"\\n"+to_string(usado) +"% ";
            }

        }

        //espacio libre entre paticiones
        if(i+1<=4){//verificando el espacio entre la particion siguente y el size y lo agregamos
            int finalPrimera = aux[i].part_start+aux[i].part_size;//final
            if(aux[i+1].part_start-finalPrimera>1){//hay espacio libre entre particiones
                //agregando el espacio libre entre las particiones
                usado = (float)(aux[i+1].part_start-finalPrimera)/(float)mbr.mbr_tamano;
                usado = usado*100;
                dot+="| Libre \\n" + to_string(usado) + "%";

            }
        }
    }
    //espacio final en el disco
   // dot+="}}";
    if((mbr.mbr_tamano)-(aux[3].part_start + aux[3].part_size)>1){
        float porcentaje = (float)(mbr.mbr_tamano-aux[3].part_start - aux[3].part_size)/(float)(mbr.mbr_tamano);
        porcentaje = porcentaje*100;
        dot+="|Espacio Libre \\n "+to_string(porcentaje)+"%";
    }

    //cerrando la estrucutra del dot
    dot+="\"];\n}";
    VerificarCarpetas(pathDestino);
    fclose(arch);
    //escribiendo el dot
    ofstream file;
    file.open(rutaActual() + pathDestino+".dot");
    file << dot;
    file.close();  
    //creando la imagen dot

    string comando ="dot -Tjpg "+rutaActual()+pathDestino+".dot -o "+rutaActual()+pathDestino; 
    system(comando.c_str());
    

}
void reporteMBR(string path,string pathDestino){
    //recuperando el EBR de la ruta 
    MBR mbr = InicialiarMBR_Particiones(mbr);
    char sc[path.size()];
    strcpy(sc, path.c_str());
    FILE *arch = fopen(sc,"rb+");
    fread(&mbr,sizeof(mbr),1,arch);
    //Iniciando con el reporte en graphviz con el MBR
    string dot="digraph G { \nconcentrate=True; \nrankdir=LR; bgcolor=\"#a5a5a5\";\n \nnode [shape=record, style=rounded];\nedge [color=white]";
    dot+="MBR[label = \"MBR | {Nombre:|mbr_tamaño|mbr_fecha_creacion|mbr_disk_signature|Disk_fit} | {";
    vector<string> tem = Split(path,"/");
    string nombreDisco = tem[tem.size()-1];
    string fecha = ctime(&mbr.mbr_fecha_creacion);
    string fecha1="";
    for (size_t i = 0; i < fecha.size(); i++)
    {
        if(fecha[i]!='\n'){
            fecha1 +=fecha[i];
        }
    }
    fecha = fecha1;
    dot+=nombreDisco+"|"+ to_string(mbr.mbr_tamano) +"|"+fecha+"|"+to_string(mbr.mbr_disk_signature)+"|"+mbr.disk_fit+"}\"];\n";
    //particiones del MBR
    int indiceExtendida=-1;
    for (size_t i = 0; i < 4; i++){
        if(mbr.mbr_partition[i].part_status!='0'){
            string tipo(1,mbr.mbr_partition[i].part_type);
            dot+="MBR_particion_"+to_string(i)+"[label = \"Particion_"+to_string(i)+" | {particion_status_"+to_string(i)+":|particion_type_"+to_string(i)+"|particion_fit_"+to_string(i)+"|particion_stax_"+to_string(i)+"|particion_size_"+to_string(i)+"|particion_name_"+to_string(i)+"} | {";
            dot+="1|"+tipo+"|"+mbr.mbr_partition[i].part_fit+"|"+to_string(mbr.mbr_partition[i].part_start) +"|"+to_string(mbr.mbr_partition[i].part_size) +"|"+mbr.mbr_partition[i].part_name+"}\"];\n";
            //enlazando EL MBR con la particion
            dot+="MBR -> MBR_particion_"+to_string(i)+";\n";
        }
        if(mbr.mbr_partition[i].part_type=='e'){
            indiceExtendida = i;
        }
    }
    //graficar las EBR
    if(indiceExtendida!=-1){
        int start = mbr.mbr_partition[indiceExtendida].part_start;
        int contador=1;
        while(true){
            string i = to_string(contador);
            fseek(arch,start,SEEK_SET);
            EBR ebr = InicializarEBR(ebr);
            fread(&ebr,sizeof(ebr),1,arch);
            string fit_(1,ebr.part_fit);
            string status_(1,ebr.part_status);
            string name_=ebr.part_name;
            dot+="EBR_"+i+"[label = \"EBR_"+i+"| { Nombre:|particion_status_"+i+"|particion_fit_"+i+"|particion_start_"+i+"|particion_size_"+i+"|particion_next_"+i+"} | {";
            dot+=name_+"|"+status_+"|"+fit_+"|"+to_string(ebr.part_start) +"|"+to_string(ebr.part_size)+"|"+to_string(ebr.part_next)+"}\"];\n";
            //condicion de salida del while
            if(ebr.part_next==-1){
                break;
            }else{
                //enlazando con la siguente particion
                dot+="EBR_"+i+" -> "+"EBR_"+to_string(contador+1)+";\n";
                contador++;
                start = ebr.part_next;
            }
        }
    }
    //realizando los enlaces entre particiones
    //particion extendida con el inicio de las EBR
    if(indiceExtendida!=-1){
        dot+="MBR_particion_"+to_string(indiceExtendida)+" -> "+"EBR_1;\n";
    }
    //finalizando la extrucutra del dot
    dot+="}";
    //verificando las carpetas del origen 
    VerificarCarpetas(pathDestino);
    //Cerrando el archivo del disco
    fclose(arch);
    //creando Escribiendo el archivo dot en el disco
    ofstream file;
    file.open(rutaActual() + pathDestino+".dot");
    file << dot;
    file.close();  
    //creando la imagen dot
   
    string comando ="dot -Tjpg "+rutaActual()+pathDestino+".dot -o "+rutaActual()+pathDestino; 
    system(comando.c_str());

}


void reporteSuperBloque(string path,string pathDestino,string nombreParticion){
    bool sePuede = true;
    //recuperando el EBR de la ruta 
    MBR mbr = InicialiarMBR_Particiones(mbr);
    char sc[path.size()];
    strcpy(sc, path.c_str());
    FILE *arch = fopen(sc,"rb+");
    fread(&mbr,sizeof(mbr),1,arch);
    // //Iniciando con el reporte en graphviz con el MBR
    string dot = "digraph G { \nconcentrate=True; \nrankdir=TB; \nnode [shape=record];\n";
    dot+="sb[label = \"SUPER BLOQUE | {filesystem_type:|s_inodes_count:|s_blocks_count:|s_free_blocks_count:|s_free_inodes_count:|s_mtime:|s_umtime:|s_mnt_count:|s_magic:|s_inode_size:|s_block_size:|s_first_ino:|s_first_blo:|s_bm_inode_start:|s_bm_block_start:|s_inode_start:|s_block_start:} | {";
    //recorriendo particiones primarias y extendidas
    int indiceExtendida=-1;
    int indice = -1;
    for (size_t i = 0; i < 4; i++)
    {
        string nombre =  mbr.mbr_partition[i].part_name;
        if(nombre == nombreParticion && mbr.mbr_partition[i].part_type!='e' ){
            indice = i;
        }
        if(mbr.mbr_partition[i].part_type=='e'){
            indiceExtendida = i;
        }
    }
    if(indice!=-1){ //es primaria
        //nos colocamos al inicio para recuperar el superbloque
        fseek(arch,mbr.mbr_partition[indice].part_start,SEEK_SET);
        superbloque super;
        fread(&super,sizeof(super),1,arch);
        //añadiendo los datos al dot
        string s_filesystem_type=to_string(super.s_filesystem_type);//numero que identifica el sistema de archivos utilizado
        string s_inodes_count = to_string(super.s_inodes_count);//numero total de inodos
        string s_blocks_count = to_string(super.s_blocks_count);//numero total de bloques
        string s_free_blocks_count = to_string(super.s_free_blocks_count); //numero de bloques libres
        string s_free_inodes_count = to_string(super.s_free_inodes_count); //numero de inodos disponibles
        string s_mtime =ctime(&super.s_mtime); //ultima fecha en que el sistema fue montado
        string s_umtime=ctime(&super.s_umtime);// ultima fecha en la que el sistema fue desmontado
        string s_mnt_count = to_string(super.s_mnt_count); //veces que se ha montado el sistema
        string s_magic = to_string(super.s_magic);//valor que identifica el sistema de archivos 0xEF53
        string s_inode_size=to_string(super.s_inode_size);//Tamaño del inodo
        string s_block_size=to_string(super.s_block_size); //Tamaño del bloque
        string s_free_ino=to_string(super.s_free_ino);//primer inodo libre
        string s_free_blo=to_string(super.s_free_blo);//primer bloque libre
        string s_bm_inode_start=to_string(super.s_bm_inode_start);//inicio del bimap de nodos
        string s_bm_block_start=to_string(super.s_bm_block_start); //inicio del bimap de bloques
        string s_inode_start=to_string(super.s_inode_start);//guardara el inicio de la tabla de inodos
        string s_block_start=to_string(super.s_block_start);//guardara el inicio de la tabla de blockes
        dot+=s_filesystem_type+"|"+s_inodes_count+"|"+s_blocks_count+"|"+s_free_blocks_count+"|"+s_free_inodes_count+"|"+s_mtime+"|"+s_umtime+"|"+s_mnt_count+"|"+s_magic+"|"+s_inode_size+"|"+s_block_size+"|";
        dot+=s_free_ino+"|"+s_free_blo+"|"+s_bm_inode_start+"|"+s_block_start+"|"+s_inode_start+"|"+s_block_start+"}";
    }else if(indiceExtendida!=-1){
        int inicio = mbr.mbr_partition[indiceExtendida].part_start;
        while(true){
            fseek(arch,inicio,SEEK_SET);
            EBR ebr = InicializarEBR(ebr);
            fread(&ebr,sizeof(ebr),1,arch);
            superbloque super;
            fseek(arch,inicio+sizeof(ebr),SEEK_SET);
            fread(&super,sizeof(super),1,arch);
            //añadiendo los datos al dot
            string s_filesystem_type=to_string(super.s_filesystem_type);//numero que identifica el sistema de archivos utilizado
            string s_inodes_count = to_string(super.s_inodes_count);//numero total de inodos
            string s_blocks_count = to_string(super.s_blocks_count);//numero total de bloques
            string s_free_blocks_count = to_string(super.s_free_blocks_count); //numero de bloques libres
            string s_free_inodes_count = to_string(super.s_free_inodes_count); //numero de inodos disponibles
            string s_mtime =ctime(&super.s_mtime); //ultima fecha en que el sistema fue montado
            string s_umtime=ctime(&super.s_umtime);// ultima fecha en la que el sistema fue desmontado
            string s_mnt_count = to_string(super.s_mnt_count); //veces que se ha montado el sistema
            string s_magic = to_string(super.s_magic);//valor que identifica el sistema de archivos 0xEF53
            string s_inode_size=to_string(super.s_inode_size);//Tamaño del inodo
            string s_block_size=to_string(super.s_block_size); //Tamaño del bloque
            string s_free_ino=to_string(super.s_free_ino);//primer inodo libre
            string s_free_blo=to_string(super.s_free_blo);//primer bloque libre
            string s_bm_inode_start=to_string(super.s_bm_inode_start);//inicio del bimap de nodos
            string s_bm_block_start=to_string(super.s_bm_block_start); //inicio del bimap de bloques
            string s_inode_start=to_string(super.s_inode_start);//guardara el inicio de la tabla de inodos
            string s_block_start=to_string(super.s_block_start);//guardara el inicio de la tabla de blockes
            dot+=s_filesystem_type+"|"+s_inodes_count+"|"+s_blocks_count+"|"+s_free_blocks_count+"|"+s_free_inodes_count+"|"+s_mtime+"|"+s_umtime+"|"+s_mnt_count+"|"+s_magic+"|"+s_inode_size+"|"+s_block_size+"|";
            dot+=s_free_ino+"|"+s_free_blo+"|"+s_bm_inode_start+"|"+s_block_start+"}";
            //condicion de salida del 
            if(ebr.part_next!=-1){
                inicio = ebr.part_next;
            }else{
                break;
            }
        }
    }else{
        sePuede = false;
        cout << "\033[1;" << "31" << "m[" <<"[ERROR-> Error al generar reporte]"<< "] " <<endl;
        

        cout<<"Nombre comparado:"<<nombreParticion<<endl;
        cout<<"Indices"<<indice<<"-"<<indiceExtendida<<endl;
    }

    if(sePuede){
        dot+="\"];\n}";
        ofstream file;
        file.open(rutaActual() + pathDestino+".dot");
        file << dot;
        file.close();  
        //creando la imagen dot

        string comando ="dot -Tjpg "+rutaActual()+pathDestino+".dot -o "+rutaActual()+pathDestino; 
        system(comando.c_str());
    }
    
}
class Rep{

    public:
        static void Reporte(vector<string>);
       
};


void Rep::Reporte(vector<string> ins){
    bool aceptado = true;
    string name=""; //nombre del reporte a realizar[MBR,disk,inode,Journaling,block,bm_inode,bm_block,tree,sb,file,ls] -- obligatorio
    string path=""; //ruta en donde se colocara el reporte-- obligatorio
    string id = "";  //id de disco montado carnet numero letra- para realizar el repote, si el reporte es del disco se usa el disco de la particion-- obligatorio
    string ruta=""; //ruta sobre el cual se hara el analisis -> opcional -> file y ls
    string root=""; //uso del reporte TREE .> indice del inodo que se utilizara como raiz para el reporte --> si no se espesifica se utiliza el sistema de archivos entero

    for (size_t i = 1; i < ins.size(); i++)
    {
        vector<string> orden = Split(ins[i], "=");
        for (size_t i = 0; i < orden[0].size(); i++)
        {
            orden[0][i] = tolower(orden[0][i]);
        }
        
        if(orden[0]=="-name"){
            for (size_t i = 0; i < orden[1].size(); i++)
            {   
                orden[1][i] = tolower(orden[1][i]);
            }
            name = orden[1];
        }else if (orden[0] == "-path"){
            if (CadenaContiene(orden[1], "\"") != 0)
            {
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
                orden[1].replace(orden[1].find("\"", 0), string("\"").length(), "");
            }
            path =orden[1];
            if (orden[1] == "local")
            {
                path = rutaActual() + "/Reporte sin nombre";
            }
        }else if(orden[0]=="-ruta"){
            
        }else if (orden[0] == "-id"){
            id = orden[1];
        }else{
                        cout << "\033[1;" << "31" << "m[" <<"Comando:@@" << orden[0] << "@@Incorrecto"<< "] " <<endl;

            aceptado = false;
        }
    }
    if (id != "" && name!="" && path!="" && aceptado){ //si se cumplen las condiciones iniciales
        //Reportes espesificos
        if(name=="mbr" | name=="disk" | name=="sb"){//mbr y disk
            string pathMBR="";
            string nombreParticion = "";
            //obtener el path en base al id de la particion montada
            for (size_t i = 0; i < particiones_montadas.size(); i++){
                string idParticion =to_string(particiones_montadas[i].carnet) +to_string(particiones_montadas[i].numero) +particiones_montadas[i].letra;
                if(strcmp(idParticion.c_str(),id.c_str())==0){
                    pathMBR = particiones_montadas[i].path;
                    nombreParticion = particiones_montadas[i].nombre;
                break;
                }
            }
            if(pathMBR!=""){
                if(name=="mbr"){
                    reporteMBR(pathMBR,path);
                }else if(name=="disk"){
                    reporteDISK(pathMBR,path);
                }else if(name=="sb"){
                    if(nombreParticion!=""){
                        reporteSuperBloque(pathMBR,path,nombreParticion);
                    }else{

                        cout<<"El nombre de la particion no esta en las montadas"<<endl;
                    }
                }
            }else{
                cout<<"el id:"<<id<< " de particion no se encuentra en las particiones montadas"<<endl;
            }
        }
        //FALTA
    
    }else{
        cout<<"Faltan parametros en la entrada"<<endl;
    }
}


