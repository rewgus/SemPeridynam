#include <iostream>
#include "particles.hpp"
#include "grid.hpp"

//hlavni fce
    //nastaveni hodnot materialovych param - E, rho atd.
    //natavení rozteče bodů, horizontu, tloušťky, objemu částice
    //nastaveni casovych kroku
    //tvorba obdelnikove site s kruhovou dirou --> vytvoření objektu particles
        //definice obdelniku
        //for cyklus, pro vzd od stredu vetsi nez R uloz castici
    //volame Grid, tvorba seznamu sousedu
    //zadani okrajovych podminek
    //vytvoření objektu solver
    //export počátečního stavu
    //casovy cyklus
        //vyvozeni napeti posouvanim x rad okrajovych castic v kazdem casovem kroku
        //volani resice time-step-verlet
        //volani fce na export dat
    // definice fce na export dat
        //export deformovanych souradnic
        //export skalarniho pole disipovane energie

// fce pro vysíťování desky s dírou, pozor - přidává body k již existující Particles deska
void vysitovaniDesky(Particles& deska, double tloustka, double Sx, double Sy, double R, double dl, int Nx, int Ny) {    
    for (int i = 0; i <= Ny; i++) {
        for (int j = 0; j <= Nx; j++) {
            if ((dl * j - Sx)*(dl * j - Sx) + (dl * i - Sy) * (dl * i - Sy) < R * R) { // x^2+y^2<R^2 - podmína pro díru
                continue; // přeskočí krok - add_particle() se nespustí - přeskočí body se souřadnicemi v díře
            }
            if ((i == 0 || i == Ny) && (j == 0 || j == Nx)) {
                deska.add_particle({j * dl, i * dl}, 0.25 * dl * dl * tloustka);
            }
            else if (i == 0 || i == Ny || j == 0 || j == Nx) {
                deska.add_particle({j * dl, i * dl}, 0.5 * dl * dl * tloustka);
            }
            else {
                deska.add_particle({j * dl, i * dl}, dl * dl * tloustka);
            }
        }
    }
}

// fce por vizualizaci - obsahuje cyklus pro porovnání s výchozí neupravenou sítí ((Nx+1)*(Ny+1)), složitost O(n^2)
void visualization(const Particles& deska, double dl, int Nx, int Ny) { // const zabrání úpravě
    for (int i = 0; i <= Ny; i++) {
        for (int j = 0; j <= Nx; j++) {
            bool found = false;
            double x = j * dl;
            double y = i * dl;
            for (std::size_t k = 0; k < deska.initial_positions.size(); k++) {
                if (x == deska.initial_positions[k][0] && y == deska.initial_positions[k][1]) {
                    found = true;
                    break;
                }
            }
            if (found) {
                std::cout << "O";
            } else {
                std::cout << ' ';
            }
        }
        std::cout << std::endl;
    }
}

int main()
{
    Particles board;

    Grid boardMap;

    // velikost desky
    double tloustka = 1;  // tloušťka
    double Lx = 100; 
    double Ly = 50;
    double Sx = Lx/2; //střed díry
    double Sy = Ly/2; 
    double R = 10; //poloměr díry

    // parametry sítě
    double dl = 1; // vzdálenost mezi body sítě 
    int Nx = Lx / dl; // interval bodů - máme x = 0 až x = 100 pro jeden řádek (počet sloupců bodů je Nx + 1) - nutné převést na int
    int Ny = Ly / dl; // interval bodů

// vysíťování obdelníku - síťuji po řádcích - síťuji i hraniční body - mají menší objem přiřazený k materiálovému bodu - proto obsahují if funkce - pokud jsou přesně na kraji, tak mají poloviční objem, v rohu mají čtvrtinový objem
    vysitovaniDesky(board, tloustka, Sx, Sy, R, dl, Nx, Ny);

    boardMap.build_map(board, dl, Nx, Ny);
  
    // //kontrola Nx, Ny a dalších šikovných veličin

    std::cout << boardMap.particle_indices[25 * (Nx + 1) + 25] <<std::endl;

    // std::cout << Nx << ' ' << Ny << std::endl;

    // std::cout << deska.positions.size() << std::endl;
    // std::cout << particles_volume_sum << std::endl;
    // std::cout << deska.initial_positions[100][0] << std::endl;

//vizualizace 

    visualization(board, dl, Nx, Ny);

    return 0;
}
