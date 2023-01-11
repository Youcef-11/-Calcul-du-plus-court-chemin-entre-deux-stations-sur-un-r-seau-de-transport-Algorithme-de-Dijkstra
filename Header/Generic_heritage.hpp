#pragma once
#include "Generic_mapper.hpp" 



namespace travel{
    // Gestion des exceptions
    struct Mauvais_format{
        int id;
        int n_line;
        std::string filename;
        std::string line;

        Mauvais_format(int a, int b, const std::string &c,const std::string &d) : id(a), n_line(b), filename(c), line(d) {};
    };

    struct NoPath{
        int st1;
        int st2;
        NoPath(int a, int b) : st1(a), st2(b) {};
    };

    class Generic_heritage:  public Generic_mapper
    {

    public:

        // ces deux méthodes lance une exception de type travel::Mauvais_format, le compileur indique un warning lorsque je l'indique dans le prototpe comme indiqué en cours.
        virtual void read_stations(const std::string& _filename) override;
        virtual void read_connections(const std::string& _filename) override;

        // Cette méthode lance une exception de type NoPath, le compileur indique un warning lorsque je l'indique dans le prototpe
        virtual std::vector<std::pair<uint64_t,uint64_t> > compute_travel(uint64_t _start, uint64_t _end) override; 

        virtual std::vector<std::pair<uint64_t,uint64_t> > compute_travel(const std::string&, const std::string&) override;

        virtual std::vector<std::pair<uint64_t,uint64_t> > compute_and_display_travel(uint64_t _start, uint64_t _end) override;
        virtual std::vector<std::pair<uint64_t,uint64_t> > compute_and_display_travel(const std::string&, const std::string&) override;


    private:
        // Cette méthide lance une exception de type string, le compileur indique un warning lorsque je l'indique dans le prototpe
        std::pair<uint64_t, uint64_t> name_to_id (const std::string&, const std::string&);
        void update_dist(uint64_t n1, uint64_t n2, std::unordered_map<uint64_t, uint64_t> &, std::unordered_map<uint64_t, uint64_t> &);


    };
}