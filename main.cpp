#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <map>
#include <algorithm>
#include "AudioFile.h"
#include <limits>

#define FREQUENCY 600.0 // Hz
#define TI_LENGTH 0.06 // secondes
#define VOLUME 0.5 // volume du son (pour la création de fichiers uniquement)


/*=================================================================================*/
/*                             lookup morse                                        */
/*=================================================================================*/

/*on code le morse tel qu'on l'entend : un carractère equivaut à une unité de tps 
    (= : actif | . : innactif) sachant qu'un ta est 3x plus long qu'un ti. 
    de plus ti est l'unité de temps élémentaire du message (wikipedia)*/
const std::vector<std::vector<std::string>> lookup{
    
        {"A","=.==="},
        {"B","===.=.=.="},
        {"C","===.=.===.="},
        {"D","===.=.="},
        {"E","="},
        {"F","=.=.===.="},
        {"G","===.===.="},
        {"H","=.=.=.="},
        {"I","=.="},
        {"J","=.===.===.==="},
        {"K","===.=.==="},
        {"L","=.===.=.="},
        {"M","===.==="},
        {"N","===.="},
        {"O","===.===.==="},
        {"P","=.===.===.="},
        {"Q","===.===.=.==="},
        {"R","=.===.="},
        {"S","=.=.="},
        {"T","==="},
        {"U","=.=.==="},
        {"V","=.=.=.==="},
        {"W","=.===.==="},
        {"X","===.=.=.==="},
        {"Y","===.=.===.==="},
        {"Z","===.===.=.="},
        {"0","===.===.===.===.==="},
        {"1","=.===.===.===.==="},
        {"2","=.=.===.===.==="},
        {"3","=.=.=.===.==="},
        {"4","=.=.=.=.==="},
        {"5","=.=.=.=.="},
        {"6","===.=.=.=.="},
        {"7","===.===.=.=.="},
        {"8","===.===.===.=.="},
        {"9","===.===.===.===.="},
        {".","=.===.=.===.=.==="},
        {",","===.===.=.=.===.==="},
        {"?","=.=.===.===.=.="},
        {"'","=.===.===.===.===.="},
        {"!","===.=.===.=.===.==="},
        {"/","===.=.=.===.="},
        {"(","===.=.===.===.="},
        {")","===.=.===.===.=.==="},
        {"&","=.===.=.=.="},
        {":","===.===.===.=.=.="},
        {";","===.=.===.=.===.="},
        {"=","===.=.=.=.==="},
        {"+","=.===.=.===.="},
        {"-","===.=.=.=.=.===="},
        {"_","=.=.===.===.="}
    };


/*=================================================================================*/
/*                             fonction texte --> morse                            */
/*=================================================================================*/

//fonction pour écrire dans le bon format dans le fichier binaire
namespace little_endian_io
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}
using namespace little_endian_io;

//renvoie un vector des noms de fichier dans le dossier d'éxécution
std::vector<std::string> getbadnames(){
    system("ls >ls.txt");
    std::fstream file;
    file.open("ls.txt");
    std::vector<std::string> badnames;
    std::string line;
    while(getline(file,line)){
        badnames.push_back(line);
    }
    file.close();
    system("rm ls.txt");
    return badnames;
}

//retourne true si nom n'est pas en .wav
bool checkwav(std::string nom){
    int N = nom.length();
    if((nom[N-1] != 'v') or (nom[N-2] != 'a') or (nom[N-3] !='w') or (nom[N-4] !='.')) return true;
    else return false;

}

//retourne true si il y a conflit entre nom et badnames
bool checkname(std::string nom,std::vector<std::string> badnames){
    for(auto bad:badnames){
        if(nom.compare(bad)==0) return true;
    }
    return false;
}

//demande un nom de fichier jusqu'à qu'il soit valide et le retourne sous forme de string
std::string askname(){
    std::vector<std::string>badnames = getbadnames();
    std::string nom=" ";
    std::cout<<"entrez le nom du du fichier de sortie (avec le .wav) : ";
    std::getline(std::cin,nom);

    while(checkname(nom,badnames) or checkwav(nom)){
        std::cout<<"fichier déjà existant ou pas en .wav"<<std::endl<<"entrez le nom du du fichier de sortie (avec le .wav) : ";
        std::getline(std::cin,nom);
    }

    return nom;
}

//fonction encodage principale
void encode(){
    //demande d'un nom du fichier de sortie
    const std::string nom = askname();

    //demande du texte à traduire
    std::string texte;
    std::cout<< "Entrez le à texte à encoder: ";
    std::getline (std::cin, texte);

    //on met tous les carractères en uppercase c'est plus simple car le morse ne fait pas la différence
    std::transform(texte.begin(), texte.end(), texte.begin(),::toupper);
    //on enlève les éventuels espaces au début
    while(texte[0]==' '){
        texte.erase(0);
    } 
    
    //texte converti en morse écrit
    std::string textemorse;

    for(auto x :texte){
        if(x==' '){ 
            // un espace est d'au moins 5 silence mais 7 sont recommandés (wikipedia) ici on ne met que 4 silences car à la fin de chaque lettre on met déjà 3 silences.
            textemorse+="....";
        }
        else{
            std::string temp = {x};
            bool fini(false);
            int i = 0;
            while((fini==false)or(i>=lookup.size()) ){
                if(temp.compare(lookup[i][0])==0) {
                    fini=true;
                }
                else{ 
                    i++;
                }
            }
            // gestion du cas où un carractère non supporté est rentré : si un carratère n'est pas dans la lookup on retourne à l'imput du texte
            if(i>=lookup.size()){
                std::cout<< "le carractère : " << x <<" n'est pas supporté"<<std::endl;
                std::cout<< "retour à l'imput de texte à traduire" << std::endl;
                encode();
            }
            //le carractère est dans la lookup donc on ajoute sa traduction en morse dans textemorse
            else{
                textemorse += lookup[i][1]; // carractère morse
                textemorse += "...";        //3 silences entre les lettres est un standard (wikipedia)
            }
        }
    }
    //std::cout<<textemorse<<std::endl;

    
    /////////////////////////////////////
    // conversion de textemorse en .wav//
    /////////////////////////////////////
    


    //création du fichier "nom" l'instance ofstream est nommée f.
    std::ofstream f( nom, std::ios::binary );

    // écriture du header
    f << "RIFF----WAVEfmt ";     // début, le ---- est pour la taille du fichier, on le remplira plus tard
    write_word( f,     16, 4 );  // il faut le mettre ....
    write_word( f,      1, 2 );  // PCM - integer samples
    write_word( f,      1, 2 );  // 1 chanel (mono)
    write_word( f,  44100, 4 );  // sampleRate (Hz)
    write_word( f,  88200, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
    write_word( f,      4, 2 );  // taille du bloc data
    write_word( f,     16, 2 );  // nombre de bits par sample

    //écriture du data chunck header
    size_t data_chunk_pos = f.tellp();//on note l'endroit où on devra écrire la taille du chunk après
    f << "data----";

    //écriture des samples

    const double SampleRate       = 44100;
    const double duration         = textemorse.length()*TI_LENGTH; //en secondes
    //std::cout<<"durée théorique du message = "<< duration<< " s"<<std::endl;
    const double amplitude        = 32760;//amplitude pour un sinus sur 16 bits

    const int N = SampleRate*TI_LENGTH; //nombre de samples par son élémentaire
 
    int J = textemorse.length();
    for(int j = 0; j<J; j++){
        if((textemorse[j]=='=')){ // si c'est un = on regarde si c'est un ti ou un taaa (on pourrait juste mettre 2 ti à la suite mais les sinus ne se recollent pas bien et ça grésille...)
            if(j==(J-1)){//pour éviter un out_of_range
                for(int i=0; i <= N ; i++){
                double sample = VOLUME*amplitude*sin ((2. * M_PI * FREQUENCY * i) / SampleRate);
                write_word(f, (int)(sample), 2);  
                }
            }
            else{
                if(textemorse[j+1]=='='){
                    j++;
                    for(int i=0; i <= 2*N ; i++){
                        double sample = VOLUME*amplitude*sin ((2. * M_PI * FREQUENCY * i) / SampleRate);
                        write_word(f, (int)(sample), 2);  
                    } 
                }
                else if(textemorse[j+1]=='.'){
                    for(int i=0; i <= N ; i++){
                        double sample = VOLUME*amplitude*sin ((2. * M_PI * FREQUENCY * i) / SampleRate);
                        write_word(f, (int)(sample), 2);  
                    }
                }
                else{
                    std::cout<<"bug de construction de textemorse (il contient un carractère diff de = ou .)"<<std::endl;
                    exit(EXIT_FAILURE);
                }
            }      
        }
        else if(textemorse[j]=='.'){
            for(int i=0; i <= N; i++){
               double silent = 0;
               write_word(f, (int)(silent), 2);
            }
        }
        else{
            std::cout<<"bug de construction de textemorse (il contient un carractère diff de = ou .)"<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    // on stocke la taille du fichier totale (donc là ou on est à l'écriture)
    size_t file_length = f.tellp();

    // on complète le data chunk header
    f.seekp(data_chunk_pos + 4);
    write_word(f,file_length - data_chunk_pos + 8);

    // met dans le header la bonne taille du chunk riff, qui est de  (file size - 8) bytes
    f.seekp (0 + 4 );
    write_word(f,file_length - 8, 4);


    std::cout<< "encodage fini, le nom du fichier est " << nom <<std::endl;
    exit(EXIT_SUCCESS);

}



/*=================================================================================*/
/*                             fonction morse --> texte                            */
/*=================================================================================*/


//retourn le rang du premier sample non nul
const long int get_start(AudioFile<double>& file){

    long int i = 0;
    while(file.samples[0][i] == 0 && i<file.getNumSamplesPerChannel()){
        i++;
    }
    if(i>=(file.getNumSamplesPerChannel()-10)){
        std::cout<<"erreur : le fichier est silencieux"<<std::endl;
        exit(EXIT_FAILURE);
    }
    return i;
}


//nettoye les zéros non voulus (dus à la forme de l'audio et pas à des silences)
void cleanup(AudioFile<double>& file,const long int& start,const int& hp){

    //on parcours les samples pour trouver les 0 isolés
    for(long int i = start; i < file.getNumSamplesPerChannel(); i++){
        if(file.samples[0][i] == 0){
            //on a détécté un 0
            long int i_0 = i;
            
            while( (file.samples[0][i] == 0) && (i < file.getNumSamplesPerChannel()) ){
                i++;
                
            }
            if(i-i_0 <= 10){

                for(int j = i_0; j < i ;j++){
                    file.samples[0][j]=1;
                }
            }
        }
    } 
}




//donne une approximation de la 1/2 periode (pour avoir une idée de la fréquence du sinus):on regarde la taille d'un creu:
int get_halfperiod(AudioFile<double>& file,const long int& start){


    long int st = start;
    bool refsign = (file.samples[0][st]>=0);//signe de départ de start
    bool sign = refsign; //signe actuel
    while((sign == refsign) && (st<file.getNumSamplesPerChannel())){
        st++;
        sign = (file.samples[0][st]>=0);
    }
    //maitenant start est au début d'une demi periode on veut savoir sa longueur:

    refsign = sign;
    long int end = st+1;
    while((sign == refsign) && (st<file.getNumSamplesPerChannel())){
        end++;
        sign = (file.samples[0][end]>=0);
    }
    //std::cout<<"half_period = "<< end - st<< "(cela correspond à "<<  file.getSampleRate()/(2*(end - st)) <<" Hz)" <<std::endl;
    
    return end - st;
    
}



//retourne la durée en samples d'un point (trait = 3 points)
long int get_dot_period(AudioFile<double>& file,long int start){

    long int dot_period = file.getNumSamplesPerChannel();
    long int end;

    for(long int i = start ; i<file.getNumSamplesPerChannel();i++){
        if(file.samples[0][i]==0){
            start = i;
            end = i;
            while(file.samples[0][end]==0 && end<file.getNumSamplesPerChannel()){
                end++;
            }
            if (end - start < dot_period) dot_period = end - start;
            i = end;
        }
    }

    //std::cout<<"dot_period = " <<dot_period<<std::endl;
    return dot_period;
}

//retourne le dernier sample avec du son +1 (ou la longueur du fichier si le dernier sample a du son)
long int get_end(AudioFile<double>& file,const long int& start){
    long int end = file.getNumSamplesPerChannel()-1;
    while(file.samples[0][end]==0 && end>start){
        end--;
    }
    if(end<=start){
        std::cout<<"ERREUR: le fichier est silencieux"<<std::endl;
        exit(EXIT_FAILURE);
    }
    return end+1;
}

void decode(){

    AudioFile<double> file; // créer une intance d'une classe de la librairie

    std::string path;
    std::cout<<"entrez le path du fichier.wav à décoder: \n"; 
    getline(std::cin,path);
    std::cout<<"le path est : "<<path<<std::endl;
    bool ok = file.load(path);
    if(!ok){
       std::cout<<"ERREUR : problème de chargement du fichier" <<std::endl;
       exit(EXIT_FAILURE);
    }
    else{
        std::cout<<"chargement réussi"<<std::endl;
    }
    
    //on marque quand l'audio commence vraiment (on ignore les premiers 0 si il y en a)
    const long int start = get_start(file);
    //std::cout<<"start = "<<start<<" samples ou "<<start/file.getSampleRate()<<" secondes"<<std::endl;

    //détection de la 1/2 periode du sinus en nbre de samples :

    const int half_period = get_halfperiod(file,start);

    //enlever les zéros non voulus (ceux créés au milieu du son par le sinus)
    cleanup(file,start,half_period);
    

    //calcul de la durée d'un point dans ce fichier morse (la plus petite unité de temps)
    long int dot_period = get_dot_period(file,start);

    //dernier sample avec du son
    long int end = get_end(file,start);

    //création d'un vector de mots, mot = vector de lettres , lettres = strings
    std::vector<std::vector<std::string>> textemorse;
    textemorse.push_back({""});//on crée le premier mot avec une lettre vide

    
    for(long int i = start; i<end; i++){
        //si c'est un silence
        if(file.samples[0][i]==0){
            long int j = i+1;
            while( (j < end) && (file.samples[0][j] == 0)){
                j++;
            }
            long int lght = j-i;//longeur du silence

            //c'est un espace
            if(lght > 4.8*dot_period){
                //on rajoute un nouveau mot avec une première lettre vide
                textemorse.push_back({""});
            }
            //c'est une nouvelle lettre
            else if(lght > 2.5*dot_period){
                //on rajoute une lettre 
                textemorse.back().push_back("");
            }
            //c'est un espace dans une lettre
            else{
                //on rajoute un . dans la dernière lettre du dernier mot
                textemorse.back().back().append(".");
            }
            i = j-1;// on avance i à j-1
        }
        //si ce n'est pas un silence
        else{
            long int j = i+1;
            while( (j < end) && (file.samples[0][j] != 0) ){
                j++;
            }
            long int lght = j-i;//longeur du non-silence
            // on avance i à j-1:
            i = j-1;
            //si c'est un tah
            if(lght>=2.5*dot_period){
                textemorse.back().back().append("===");
            }
            //si c'est un ti
            else if(lght<=1.5*dot_period){
                textemorse.back().back().append("=");
            }
        }
    }

    //transcription du fichier morse audio en morse 
    for(auto x : textemorse){
        for(auto y : x){
            std::cout<< y <<"/";
        }
        std::cout<<"  /";
    }
    std::cout<<std::endl; 

    //convertir le morse en texte
    std::string texte;
    std::vector<std::string> inconnus;
    for(auto x : textemorse){
        for(auto y : x){
            bool fini(false);
            int i = 0;
            while((fini==false) && (i<lookup.size()) ){

                if(y.compare(lookup[i][1])==0) {
                    fini=true;
                }
                else{ 
                    i++;
                }
            }
            // si un carractère est inconnu on rajoute un "?"
            if(i>=lookup.size()){
                inconnus.push_back(y);
                texte += "°?°";
            }
            //le carractère est dans la lookup donc on ajoute sa traduction au texte
            else{
                texte += lookup[i][0]; //on ajoute au texte le carractère correspondant
            }
        }
        //ajout d'un espace entre chaque mot
        texte+=' ';
    }
    if(!inconnus.empty()){
        std::cout<<"les carractères suivants sont inconnus :\n";
        for(auto x : inconnus){
            std::cout<<x<<std::endl;
            }
        std::cout<<"ils ont étés marqués par des °?° dans le texte traduit\n\n";
        } 
    std::cout<<"texte traduit :\n"<< texte <<std::endl;     
    
}



/*=================================================================================*/
/*                                      main                                       */
/*=================================================================================*/

bool choice(){
    std::string entree;
    std::cout<<"encodage ou décodage [e/d]? "; 
    getline(std::cin,entree); 
    std::cout<<std::endl;
    while((entree[0] != 'e') && (entree[0] != 'd')){
        std::cout<<"entrez 'e' pour décodage ou 'd' pour décodage"<<std::endl;
        std::cout<<"encodage ou décodage [e/d]? "; 
        getline(std::cin,entree);
        std::cout<<std::endl;
    }
    if (entree[0]=='e'){
        return true;
    }
    else{
        return false;
    }  
}

int main(){   

    //on demande si on veut encoder ou décoder un texte morse
    bool choix = choice();
    if(choix){
        encode();
    }
    else{
        decode();
    }

    return 0;
}