#include <cmath>
#include "particles.hpp"
#include "solver.hpp"
//vnitrni fce pro vypocet sil pro verlet
void Solver::compute_forces(Particles& p, const MatParams& mat) {
    size_t num_particles = p.initial_positions.size();
    //for loop pres vsechny castice
    for (size_t i=0; i < num_particles; ++i) {
        //inic akumulace sil pro vypocet
        double f_accum_x = 0.0;
        double f_accum_y = 0.0;

        //nacteni pozic i-te castice
        double p0i_x = p.initial_positions[i][0];
        double p0i_y = p.initial_positions[i][0];
        double pi_x = p.positions[i][0];
        double pi_y = p.positions[i][1];

        //for loop pres vsechny sousedy
        for (size_t k = 0; k < p.neighbors[i].size(); ++k) {
            
        }

    }


}
//time step verlet
    //vstup pro okrajové podmínky
    //výpočet počátečních sil a zrychlení

    //vypocet noveho pulkroku rychlosti a celeho posuvu
    //vypocet sil, vazeb, energie
        //Výpočet sil působících na každou částici v aktuálním časovém kroku
            // kontrola zda vazby jsou aktivní
            // vypocet vektoru polohy a posuvu => vypocet delky vazby
            // vypocet relativniho prodlouzeni a kontrola zda nebylo prekroceno max
            // vypocet hustoty siloveho pole a akumulace elasticke potenc. energie
    // dokonceni casoveho kroku
