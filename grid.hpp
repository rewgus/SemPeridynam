// definice gridu
// hledání sousedů pomocí vnořených for cyklů, jeden x jeden y
    // definice okolí pro hledání
    // hledání částic v okolí
        // přeskočit sama sebe
    // výpočet vzdálenosti jednotlivých částic od částice pro niž běží for cykly
    // kontrola zda částice jsou v okolí delta částice for cyklu

#pragma once
#include <vector>
#include "particles.hpp"
#include <cmath>

struct Grid {
    std::vector<signed int> particle_indices;

    // vytvoření mapy pro možnost lokality bodu vůči jiným podle indexu
    void build_map(const Particles& p, double dl, int Nx, int Ny) { 
        particle_indices.resize((Nx + 1) * (Ny + 1));
        for (int i = 0; i <= Ny; i++) {
            for (int j = 0; j <= Nx; j++) {
                int i_map = i * (Nx + 1) + j;
                particle_indices[i_map] = -1;
            }
        }
        for (std::size_t k = 0; k < p.initial_positions.size(); k++) {
            int j = std::round(p.initial_positions[k][0] / dl);
            int i = std::round(p.initial_positions[k][1] / dl);
            int i_map = i * (Nx + 1) + j;
            particle_indices[i_map] = k;
        }
    }
};