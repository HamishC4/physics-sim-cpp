#include <splashkit.h>
#include <format>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <ostream>
#include <random>
#include <cmath>
#include <chrono>
#include <thread>

/*Hamish Cranston 
Version 1.0 9/05/2026
Underlying physics and simulation basics learnt from https://courses.physics.illinois.edu/phys498cmp/sp2022/Ising/IsingModel.html

*/

#define DEFAULT_GRID_SIZE 10

#define DEFAULT_COUPLING_CONSTANT 1.0

const double kB = 1.380649e-23; // Boltzmann constant in J/K

class isingData
{

public:
    vector<int> spins;
    vector<double> magnetic_fields;
    vector<vector<std::pair<int,double>>> neighbours;

    double temperature;
    double energy;

    //Default constructor makes a rectangular grid of 10x10 with no field. 
    isingData(){
        temperature = 273.15;
        energy = 0;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        for (int i = 0; i<DEFAULT_GRID_SIZE;i++){
            for (int j = 0; j<DEFAULT_GRID_SIZE;j++){
                spins.push_back(dis(gen)==0 ? -1 : 1); // Randomly assign spin up or down
                magnetic_fields.push_back(0); // No magnetic field
                vector<std::pair<int,double>> neighbour_list;
                //Had to correct this. We only want to have neighbours linked one way.
                if (i < DEFAULT_GRID_SIZE - 1) neighbour_list.push_back({(i+1)*DEFAULT_GRID_SIZE+j, DEFAULT_COUPLING_CONSTANT}); // Right neighbor
                if (j < DEFAULT_GRID_SIZE - 1) neighbour_list.push_back({i*DEFAULT_GRID_SIZE+(j+1), DEFAULT_COUPLING_CONSTANT}); // Down neighbor
                neighbours.push_back(neighbour_list);
            }

        }
    }

    //Basic printing function.
    void print_ising_grid(){
        for (int i = 0; i<DEFAULT_GRID_SIZE;i++){
            for (int j = 0; j<DEFAULT_GRID_SIZE;j++){
                std::cout << (spins[i*DEFAULT_GRID_SIZE+j] == 1 ? "↓ " : "↑ ");
            }
            std::cout << std::endl;
        }
    }
    // Calculate the total energy according to the hamiltonian. This will work for any graph, not just the grid!
    double calculate_total_energy(){
        energy = 0;
        for (int i=0; i<spins.size(); i++){
            for (int j = 0; j<neighbours[i].size();j++){
                energy += -neighbours[i][j].second * spins[i] *spins[neighbours[i][j].first];
            }
        }

        return energy;
    }
    //Return the change in energy for calculating a spin flip for an index. This is our selection criteria. WOrks for all graphs.
    double calculate_energy_change(int index){
        //We use this approach rather than recalculating energy for the whole array, as it is much faster.
        //Only need to look at one index.
        double energy_change = 0;
        for (int j = 0; j<neighbours[index].size();j++){
            energy_change += 2*neighbours[index][j].second * spins[index] *spins[neighbours[index][j].first];
        }
        return energy_change;
    }

    void simulation_step(){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis_sel(0.0,1.0);
        std::uniform_int_distribution<> dis(0, spins.size() - 1); //Select which spin to flip!
        int index = dis(gen);
        double selection_prob = dis_sel(gen);
        double energy_change = calculate_energy_change(index);
        double exponential = exp(-energy_change*1/(kB*temperature));
        if (selection_prob < exponential){
            spins[index] *= -1; // Flip the spin
            energy += energy_change; // Update the energy
        }
        
    }

};