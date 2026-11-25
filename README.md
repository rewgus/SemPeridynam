# SemPeridynam

Semestrální projekt z předmětu **Programování inženýrských aplikací**: C++ solver pro simulaci deformace a křehkého porušení obdélníkové desky s kruhovým otvorem pomocí vazbové (*bond-based*) peridynamiky.

Deska je reprezentována částicemi s přiřazenými objemy. Levý okraj je pevný, pravý má předepsaný pohyb. Síly vznikají interakcemi částic v horizontu `delta`. Vazby se při překročení kritického relativního prodloužení nevratně přetrhnou a umožní vznik a šíření trhliny.

## Struktura projektu

| Soubor / složka | Úloha |
| --- | --- |
| `main.cpp` | Nastavení geometrie, materiálu, časového kroku, zatížení a exportu; spuštění simulace. |
| `particles.hpp` | Data částic a vazeb; `add_particle()` a generování desky pomocí `boardMeshing()`. |
| `grid.hpp` | Mapa souřadnic na indexy částic, hledání sousedů, zadání okrajových podmínek a výpis geometrie do terminálu. |
| `solver.hpp` | Materiálové parametry `MatParams`, výpočet `c0` a `s0`, deklarace řešitele. |
| `solver.cpp` | Výpočet hustoty vnitřních sil, přetrhávání vazeb a časová integrace Velocity Verlet. |
| `io.hpp`, `io.cpp` | Výpočet ukazatele poškození a zápis výsledků do VTK. |
| `paraview/` | Výstupní snímky simulace; složka se vytvoří při exportu. |

## Překlad a spuštění

V terminálu WSL/Linux spusť z kořenové složky projektu:

```bash
cmake -B build-linux && cmake --build build-linux
```

Stačí kompilátor s podporou C++17 a CMake 3.10; další knihovny nejsou potřeba. Pro prohlížení výsledků slouží ParaView.

Ve složce build-linux vznikne soubor "peridynamika", který lze spustit v prostředí wsl pomocí:

```bash
./peridynamika
```

## Vstupy a jejich úprava

Vstupy jsou zadány přímo v `main.cpp`; program nečte konfigurační soubor ani argumenty příkazové řádky. Používají se jednotky **SI: m, s, kg, N, Pa**.

| Parametr | Výchozí hodnota | Význam |
| --- | --- | --- |
| `Lx`, `Ly` | `100e-3`, `50e-3` m | Rozměry desky. |
| `tloustka` | `1e-3` m | Tloušťka desky. |
| `R` | `10e-3` m | Poloměr otvoru. |
| `Sx`, `Sy` | `Lx / 2`, `Ly / 2` | Střed otvoru; lze změnit. |
| `dl` | `1e-3` m | Rozteč částic v obou směrech. |
| `delta` | `3 * dl` | Horizont interakcí. |
| `E` | `210e9` Pa | Youngův modul. |
| `rho` | `7850` kg/m³ | Hustota. |
| `Gc` | `12500` J/m² | Kritická energie porušení na jednotku plochy. |
| `dt` | `1e-7` s | Časový krok. |
| `total_steps` | `50000` | Počet časových kroků. |
| `num_steps` | `100` | Parametr četnosti exportu; `output_frequency = total_steps / num_steps`. |

Doba simulace je `dt * total_steps`, ve výchozím nastavení **0,005 s**. Rozměry `Lx` a `Ly` vol jako celočíselné násobky `dl`. Pro export musí platit `1 <= num_steps <= total_steps`. Při změně rozteče, horizontu či materiálu je potřeba znovu posoudit stabilitu časového kroku.

### Okrajové podmínky

Rozsahy uprav v těchto voláních v `main.cpp`:

```cpp
boardMap.set_fixed_boundary_type(board, 0, 4, 0, Ny);
boardMap.set_fixed_displacement(
    board, dt, total_steps, 10e-3, 0, Nx - 4, Nx, 0, Ny
);
```

První volání fixuje pět sloupců vlevo. Druhé předepisuje pěti sloupcům vpravo celkové posunutí **10 mm ve směru x** a nulové ve směru y. Posunutí měníš argumenty `delta_x`, `delta_y`; realizuje se konstantní rychlostí `delta_x / (dt * total_steps)`, obdobně pro y.

Meze jsou **indexy sloupců a řádků**, nikoli souřadnice v metrech. Oba konce rozsahu se zahrnují; musí ležet v `[0, Nx]` a `[0, Ny]`. Typ částice je uložen v `boundary_type`: `0` volná, `1` pevná, `2` předepsaný pohyb.

## Použité algoritmy

1. **Generování částic:** pravidelné rozmístění bodů; body uvnitř otvoru se vynechají. Na vnější hraně desky mají částice poloviční objem, v rozích čtvrtinový. U kruhového otvoru se objemy částečných buněk nekorigují.
2. **Hledání sousedů:** plochá mapa převádí řádek a sloupec na index částice; prázdná místa mají hodnotu `-1`. Prohledává se lokální čtvercové okolí a následně kontroluje vzdálenost vůči horizontu.
3. **Síly a porušení:** příspěvky aktivních vazeb se sčítají s vahou objemu souseda. Vazba se přetrhne při `s > s0`, kde `s = (l - l0) / l0`. Nové vazby nevznikají.
4. **Časová integrace:** explicitní Velocity Verlet aktualizuje rychlosti a polohy; předepsaný pohyb okrajů se provede před novým výpočtem sil.

Změny materiálových vztahů patří do `MatParams::vypocet_zavislych_param()` v `solver.hpp` a do `Solver::compute_forces()` v `solver.cpp`.

## Výstupy

Do složky `paraview/` v pracovním adresáři se ukládají soubory `frame_XXXXXX.vtk` ve formátu ASCII VTK. Obsahují:

- aktuální souřadnice částic; třetí souřadnice je nulová,
- vektorové pole `velocity` [m/s],
- skalární pole `damage = 1 - počet aktivních vazeb / počet původních vazeb` v rozsahu 0 až 1.

`damage` vyjadřuje podíl přetržených vazeb, nikoli disipovanou energii. Částici bez sousedů export přiřadí hodnotu 0. Konzole vypisuje průběh exportu, rozměry mřížky a původní geometrii pomocí znaků `O`.

Číslo v názvu snímku je index cyklu, nikoli fyzikální čas. Export probíhá po provedení kroku: `frame_000000.vtk` odpovídá času `dt`. Počáteční stav ani poslední krok se neukládají samostatně. Při opakovaném spuštění se shodné názvy přepíší; starší snímky s jinými názvy ve složce zůstanou.

V ParaView otevři sérii `frame_*.vtk`, stiskni **Apply**, zvol zobrazení **Points** a obarvení podle `damage` nebo `velocity`. Průběh zobrazíš tlačítkem **Play**; export neobsahuje metadata fyzikálního času.

## Poznámky

README.md bylo vytvořeno s pomocí AI
