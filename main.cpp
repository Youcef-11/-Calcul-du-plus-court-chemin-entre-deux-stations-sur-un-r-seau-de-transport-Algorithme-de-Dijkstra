#include <iostream>
#include "Header/Generic_heritage.hpp"
#include "Header/Grade.hpp"
#include <unordered_map>
#include <string>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using namespace travel;

const int MAX_ATTEMPT = 10; 

bool is_numeric(const string & s){
    for (const auto &c : s)
        if(isalpha(c)) return false;
    return true;
}

int main(){


    Generic_heritage c;
    try{
        c.read_stations("data/s.csv");
        c.read_connections("data/c.csv");
    }
    catch(const Mauvais_format& e){
        cerr << "Erreur à la ligne " << e.n_line << " du fichier " << e.filename << endl;
        cerr << "\"" << e.line << "\"" << endl;
        if (e.id == 1)
            cerr << "Le format accepté est : a,b,c,d,e" << endl;
        else if (e.id == 2)
            cerr << "Le format accepté est : a,b,c" << endl;
        
        return e.id;
    }
    catch(const string & s){
        cerr<<"Impossible d'ouvrir le fichier : " << s << endl;
        cerr<<"Fin du programme" << endl;
        // On arrete le programme si on ne peut pas lire le fichier
        return -1;
    }

    int nb_attempt = 0;

    string st1; 
    string st2;

    bool restart = false;
    do{
        restart = false;
        nb_attempt++;

        cout << "Votre station de départ (nom ou id): ";
        getline(cin, st1);

        cout << "Votre station d'arrivée (nom ou id): ";
        getline(cin,st2);
        cout << endl;
        try{
            if (is_numeric(st1) && is_numeric(st2))
                 c.compute_and_display_travel(stoi(st1), stoi(st2));

            else if (!is_numeric(st1) && !is_numeric(st2))
                c.compute_and_display_travel(st1, st2);

            else{
                cerr << "Format invalide, veuillez entrer deux noms ou deux identifiants de stations." << endl;
                restart = true;
            }
        } 

        catch(string e){
            cerr << "La station \"" << e << "\" n'existe pas, veuillez entrer un nom de station valide." << endl;
            restart = true;
        }
        catch(uint64_t e){
            cerr << "La station \"" << e << "\" n'est pas dans la base de donnée, veuillez entrer un numero de station valide. " << endl;
            restart = true;
        }
        catch(NoPath e){
            cerr << "Aucun chemin trouvé de la station avec pour identifiant " << e.st1 << " a la station." << e.st2 << endl;
        }
    
    }while (restart && nb_attempt < MAX_ATTEMPT);

    cout << "A bientot !" << endl;

    return 0;
}


        