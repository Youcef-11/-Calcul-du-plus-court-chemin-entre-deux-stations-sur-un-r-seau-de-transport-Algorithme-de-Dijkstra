#include "../Header/Generic_heritage.hpp"
#include <fstream>
#include <limits>
#include <sstream>
using namespace std;




/**
 * @fn void Generic_heritage::read_stations(const std::string& _filename) 
 * @brief Lis les stations dans le fichier indiqué _filename 
 * @param const std::string& _filename : nom du fichier
 * @throw travel::Mauvais_format
 * @return void
 */
void travel::Generic_heritage::read_stations(const std::string& _filename) {


    // since c++ 11 we can pass a string to the constructor of ifstream (no need to convert it to a c string)
    ifstream entree(_filename);
    if (entree.fail()){
        throw _filename;
    }
    else{
        Station station;
        string line;
        vector<string> seg_list; // vecteur conetnant les informations de la ligne délémité par les ,
        unsigned int n_line = 0;
        string segment;
        while(!entree.eof()){
            getline(entree, line);
            istringstream word_stream(line);
                if (n_line > 0){
                    while(getline(word_stream, segment, ','))
                        {
                            seg_list.push_back(segment);
                        }
                    // Si le fichier n'est pas conforme (les données sont mal represesntée on lance une exception dd type Mauvais_Format) 
                    if (seg_list.size() != 5 && !entree.eof()) 
                        throw travel::Mauvais_format(1, n_line + 1 , _filename, line);
                        
                    station.name = seg_list[0];
                    station.line_id = seg_list[2];
                    station.address = seg_list[3];
                    station.line_name = seg_list[4];

                    // seg_list[1] correspond à l'id de la station
                    this->stations_hashmap[stoi(seg_list[1])] = station;
                    // Il faut maintenant vider le  vecteur.
                    seg_list.clear();
                }

                n_line++;
            }

        }
        entree.close();

    } 






/**
 * @fn void Generic_heritage::read_connections(const std::string& _filename) 
 * @brief Lis les connections dans le fichier indiqué _filename 
 * @param const std::string& _filename : nom du fichier
 * @throw travel::Mauvais_format
 * @return void
 */
void travel::Generic_heritage::read_connections(const std::string& _filename){
    ifstream entree(_filename);
     if (entree.fail()){
         throw _filename;
    } 

    else{
        string line;
        string cur;

        unordered_map<uint64_t,uint64_t> int_map; 
        unsigned int n_line = 0;
        unsigned int n_cur;
        int tab[3]; 


        while (!entree.eof()){
            getline(entree, line);
            istringstream mystream(line);
            n_cur = 0;
            while(getline(mystream, cur, ',')){
                if(n_line > 0){
                    if(n_cur < 3)  // On peut ici mettre a la place un try
                        tab[n_cur] = stoi(cur);
                } 

                n_cur++;
            }
            // Si la ligne du fichier csv contient  trop de données ou pas assez on lance une  exception
            if (n_cur != 3 && !entree.eof())
                throw travel::Mauvais_format(2, n_line +1, _filename, line);
            if (n_line >0){

                if (this->connections_hashmap.count(tab[0]) == 0){
                    int_map[tab[1]] = tab[2];
                    this->connections_hashmap[tab[0]] = int_map;
                    int_map.clear();
                }
                else {
                    this->connections_hashmap[tab[0]][tab[1]] = tab[2];
                }
                n_cur = 0;
            }
            n_line++;
        }
    }
}


/**
 * @fn uint64_t find_min(const unordered_map<uint64_t, uint64_t> &dist, vector<int> &Q)
 * @brief recherche la station a la distance minmal et supprime la station de Q (vecteur des stations pas visités)
 * @param const unordered_map<uint64_t, uint64_t> &dist : Map contenant les distances de chaques stations
 * @param const vector<int> &Q : Vecteur contenant toutes les stations qui n'ont pas encore été visités.
 * @return uint64_t
 */
uint64_t find_min(const unordered_map<uint64_t, uint64_t> &dist, vector<int> &Q){
    uint64_t min = INT64_MAX;
    vector<int>::iterator noeud; 

    for (vector<int>::iterator i = Q.begin(); i!= Q.end(); i++){
        if (dist.at(*i)< min){
            min = dist.at(*i);
            noeud = i;
        }

    }
    uint64_t noeud_min = *noeud;
    Q.erase(noeud);
    return noeud_min;
}
/**
 * @fn void travel::Generic_heritage::update_dist(uint64_t n1, uint64_t n2, unordered_map<uint64_t, uint64_t> &dist, unordered_map<uint64_t, uint64_t> & predecesseur)
 * @brief Mise à jour des distance et sauvegarde dans la map predecesseur (par référence) en notant le nouveau chemin le plus court.
 * @param uint64_t n1 : id station noeud minimum
 * @param uint64_t n2 : id station noeud enfant au minmium
 * @param unordered_map<uint64_t, uint64_t> &dist map contenant les distance à mettre a jour
 * @param unordered_map<uint64_t, uint64_t> &predecesseur map des noeud par où on passe 
 * @return void
 */
void travel::Generic_heritage::update_dist(uint64_t n1, uint64_t n2, unordered_map<uint64_t, uint64_t> &dist, unordered_map<uint64_t, uint64_t> & predecesseur){
    uint64_t new_poids = dist[n1] + this->connections_hashmap[n1][n2];
    if (dist[n2] > new_poids){
        dist[n2] = new_poids;
        predecesseur[n2] = n1;
    }
    


}

/**
 * @fn std::vector<std::pair<uint64_t,uint64_t>> travel::Generic_heritage::compute_travel(uint64_t _start, uint64_t _end)
 * @brief Calcul du chemin minmum
 * @param uint64_t _start : id station début
 * @param uint64_t _end : id station fin
 * @throw int, NoPath
 * @return std::vector<std::pair<uint64_t,uint64_t>>
 */
std::vector<std::pair<uint64_t,uint64_t>> travel::Generic_heritage::compute_travel(uint64_t _start, uint64_t _end){ 
    // On test d'abord si les deux stations existent. si elle n'existent pas, on lance une exception    
    if(this->stations_hashmap.find(_start) == this->stations_hashmap.end())
        throw _start;    
    if(this->stations_hashmap.find(_end) == this->stations_hashmap.end())
        throw _end;

     

    // Map qui contient les distances
    unordered_map<uint64_t, uint64_t> distance;
    unordered_map<uint64_t, uint64_t> predecesseur;
    vector<pair<uint64_t,uint64_t>> meilleur_chemin;      //Vecteur des poids de la station start aux autres stations.
    vector<int> Q;                                      //Vecteur contenant les noeuds des stations.

    for (auto st : this->connections_hashmap){
        distance[st.first] = INT64_MAX;
        Q.push_back(st.first);
    }

    distance.at(_start) =  0;
    uint64_t noeud_min;
    while(!Q.empty() && noeud_min != _end){  
       noeud_min = find_min(distance, Q);
       for(const auto &x : this->connections_hashmap[noeud_min])
            update_dist(noeud_min, x.first, distance, predecesseur);
    }

    // Si a la fin le noeud_minimium n'est pas end, c'est que Q est vide et donc il n'existe pas de chemin de _start à _ind
    if (noeud_min != _end) 
        throw travel::NoPath(_start, _end);

    // Calcul du meilleur chemin 
    while (_end != _start){
        meilleur_chemin.insert(meilleur_chemin.begin(), make_pair(_end, distance[_end]));
        _end = predecesseur[_end];
    }
    meilleur_chemin.insert(meilleur_chemin.begin(), make_pair(_end, distance[_end]));

    return meilleur_chemin;
}
/**
 * @fn string seconds_to_minutes(uint64_t int_secondes)
 * @brief Fonction pour transformer et afficher des secondes en minutes 
 * @param uint64_t int_seconde : temps en secondes
 * @return string
 */
string seconds_to_minutes(uint64_t int_secondes){
    string temps;
    uint64_t min = int_secondes/60;
    uint64_t s = int_secondes%60;
    string minutes;
    string secondes;
    if (min == 1) 
        minutes = " minutes et ";
    else 
        minutes = " minutes et ";
    if (s == 1 || s == 0)
        secondes = " seconde";
    else 
        secondes = " secondes";


    if (min == 0)
        temps = to_string(s) + secondes;
    else
        temps = to_string(min) + minutes + to_string(s) + secondes;

    return temps;
}

/**
 * @fn std::vector<std::pair<uint64_t,uint64_t>> travel::Generic_heritage::compute_and_display_travel(uint64_t _start, uint64_t _end)
 * @brief Affichage de l'itinéraire le plus rapid
 * @param uint64_t _start : id station début
 * @param uint64_t _end : id station fin
 * @return std::vector<std::pair<uint64_t,uint64_t>>
 */
std::vector<std::pair<uint64_t,uint64_t>> travel::Generic_heritage::compute_and_display_travel(uint64_t _start, uint64_t _end){
    vector<pair<uint64_t,uint64_t>> meilleur_chemin  = this->compute_travel(_start, _end);      // Affichage du meilleur chemin
    const auto &stations = this->stations_hashmap;                                              // Création d'une référence vers la map pour plus de clarté dans le code
    bool nouvel_correspondance = true;

    cout << "L'itineraire le plus rapide pour aller de la station " << stations.at(_start).name << " a la station "
    << stations.at(_end).name << " vous prendra : "
    << seconds_to_minutes(meilleur_chemin.back().second) 
    << " :\n=========================================================" << endl;

    for (vector<pair<uint64_t,uint64_t>>::iterator i = meilleur_chemin.begin(); i!= meilleur_chemin.end()-1; i++){

        // On créer des réferences pour éviter de réecrir entierment le nom
        const Station & s = stations.at(i->first);         
        const Station & next_s = stations.at((i+1)->first); 

        //(i+1)->second - i->second == 0 ||
        // Il est possible dans l'itinéraire qu'il faille marcher jusqu'a un autre quai de la meme station (par exemple pour le sens retour)
        if (next_s.name == s.name){
            cout << " | Marchez jusqu'au quai de la ligne " << next_s.line_id << " de la station " << next_s.name << " dans le sens " << next_s.line_name << " [" << seconds_to_minutes((i+1)->second - i->second) << "]" << endl <<" |" << endl;
            nouvel_correspondance = true;
        }
        else{
            if ((next_s.line_id != s.line_id && next_s.line_name != s.line_name) || nouvel_correspondance) 
                cout << " | Prendre la ligne numero " << s.line_id << " de la station " << s.name << " dans le sens " << s.line_name <<endl <<" | Restez sur cette ligne en suivant cette itinéraire : " << endl;  
            cout << " | \t" << s.name << " <=> " << next_s.name << " [" << seconds_to_minutes((i+1)->second - i->second) << "]" << endl <<" |" << endl ;
            nouvel_correspondance = false;
        }
    }
    cout << " | Vous etes arrive !"<< endl;
    return meilleur_chemin;
}

/**
 * @fn string lower_str(const string& s)
 * @brief mets une chaine de caratère string en minuscule
 * @param const string& s
 * @return string
 */
string lower_str(const string& s){
    string lower_s;

    // On pourrait également gérer les accents mais cela devient plus compliqué (UTF-8)
    for(auto i : s) 
        lower_s.push_back(tolower(i));

    return lower_s;
}
/**
 * @fn std::pair<uint64_t, uint64_t> travel::Generic_heritage::name_to_id(const std::string& _start, const std::string& _end)
 * @brief converti des noms de stations avec leur id 
 * @param const string& _start : station de début 
 * @param const string& _end : station de fin
 * @throw string
 * @return std::pair<uint64_t, uint64_t>
 */
std::pair<uint64_t, uint64_t> travel::Generic_heritage::name_to_id(const std::string& _start, const std::string& _end){
    uint64_t start = 0;
    uint64_t end = 0;

    bool found_start = false;
    bool found_end = false;

    // On transforme d'abord les strings en minuscule puis on les compare aux stations tarnsformées aussi pour être insensible à la casse
    string lower_start = lower_str(_start);
    string lower_end =  lower_str(_end);

    // Boucle qui parcourt la map, 
    for (auto i : this->stations_hashmap){
        if (lower_str(i.second.name) == lower_start){
            start = i.first;    
            found_start = true;
        }
        // On met un if au lieu d'un else if ici pour autoriser la recherche du chemin dont la station d'arrivé est la station de départ.
        if (lower_str(i.second.name) == lower_end){
            end = i.first;
            found_end = true;
        }

        // Si on a trouvé les deux stations, on quitte la boucle pour ne pas faire des calcus inutiles
        if (found_start && found_end)
            break;
    }

    // Si le nom de station donné n'existe pas on crée une exception
    if (!found_start)
        throw _start;
        
    if (!found_end)
        throw _end; 
    
    return make_pair(start, end);
}
/**
 * @fn std::pair<uint64_t, uint64_t> travel::Generic_heritage::name_to_id(const std::string& _start, const std::string& _end)
 * @brief calcul le chemin le plus court a partir des noms de stations
 * @param const string& _start : station de début 
 * @param const string& _end : station de fin
 * @throw string
 * @return std::pair<uint64_t, uint64_t>
 */
std::vector<std::pair<uint64_t,uint64_t>> travel::Generic_heritage::compute_travel(const std::string& _start, const std::string& _end){
    auto ids = this->name_to_id(_start, _end);  // Une pair
    uint64_t start = ids.first;
    uint64_t end = ids.second;

    return this->compute_travel(start, end);
}



/**
 * @fn std::pair<uint64_t, uint64_t> travel::Generic_heritage::name_to_id(const std::string& _start, const std::string& _end)
 * @brief calcul et affiche le chemin le plus court a partir des noms de stations
 * @param const string& _start : station de début 
 * @param const string& _end : station de fin
 * @throw string
 * @return std::pair<uint64_t, uint64_t>
 */
std::vector<std::pair<uint64_t,uint64_t> > travel::Generic_heritage::compute_and_display_travel(const std::string& _start, const std::string& _end) {
    auto ids = this->name_to_id(_start, _end);  // Une pair
    uint64_t start = ids.first;
    uint64_t end = ids.second;

    return this->compute_and_display_travel(start, end);

}