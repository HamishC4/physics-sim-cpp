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

using namespace std;

/*Hamish Cranston 
Version 1.0 9/05/2026
Underlying physics and simulation basics learnt from https://courses.physics.illinois.edu/phys498cmp/sp2022/Ising/IsingModel.html

*/


#define DEFAULT_COUPLING_CONSTANT 1.0

const double kB = 1; // Boltzmann constant in J/K. This has been normalised to 1.

class isingData
{
    
private: //Private variables
    //Probability generation engine.
    std::mt19937 gen;
    std::uniform_real_distribution<> dis_sel{0.0, 1.0};
    std::uniform_int_distribution<> dis_spin;
    //Spin vectors
    vector<int> spins; 
    vector<double> magnetic_fields;
    vector<vector<std::pair<int,double>>> neighbours;
    //Attributes
    int x_dim;
    int y_dim;
    int z_dim;
    double temperature;
    double energy;

    public:

    //Default Constructor here.
        isingData(int x,int y,int z)
        {
        
        if (x <= 0 || y <= 0 || z <= 0) 
        {
            throw std::invalid_argument("isingData dimensions must be > 0");
        }
        temperature = 273.15;
        x_dim = x;
        y_dim = y;
        z_dim = z;
        energy = 0;

        std::random_device rd;
        gen = std::mt19937(rd());
        std::uniform_int_distribution<> dis(0, 1);

        for (int i = 0; i<x_dim;i++)
        {
            for (int j = 0; j<y_dim;j++)
            {
                for (int k = 0;k<z_dim;k++)
                {
                    spins.push_back(dis(gen)==0 ? -1 : 1); // Randomly assign spin up or down
                    magnetic_fields.push_back(0); // No magnetic field
                    vector<std::pair<int,double>> neighbour_list;

                    // X-axis neighbors (wrapped)
                    if (x_dim > 1)
                    {
                        neighbour_list.push_back({((i + 1) % x_dim) * y_dim * z_dim + j * z_dim + k, DEFAULT_COUPLING_CONSTANT});
                        neighbour_list.push_back({((i - 1 + x_dim) % x_dim) * y_dim * z_dim + j * z_dim + k, DEFAULT_COUPLING_CONSTANT});
                    }
                    // Y-axis neighbors (wrapped)
                    if (y_dim > 1)
                    {
                        neighbour_list.push_back({i * y_dim * z_dim + ((j + 1) % y_dim) * z_dim + k, DEFAULT_COUPLING_CONSTANT});
                        neighbour_list.push_back({i * y_dim * z_dim + ((j - 1 + y_dim) % y_dim) * z_dim + k, DEFAULT_COUPLING_CONSTANT});
                    }
                    // Z-axis neighbors (wrapped)
                    if(z_dim > 1)
                    {
                        neighbour_list.push_back({i * y_dim * z_dim + j * z_dim + ((k + 1) % z_dim), DEFAULT_COUPLING_CONSTANT});
                        neighbour_list.push_back({i * y_dim * z_dim + j * z_dim + ((k - 1 + z_dim) % z_dim), DEFAULT_COUPLING_CONSTANT});
                    }
                    neighbours.push_back(neighbour_list);
                }
            }

        }
                dis_spin = std::uniform_int_distribution<>(0,spins.size()-1);
    }


    // Calculate the total energy according to the hamiltonian. This will work for any graph, not just the grid!
    double calculate_total_energy()
    {
        energy = 0;
        for (int i=0; i<spins.size(); i++)
        {
            for (int j = 0; j<neighbours[i].size();j++)
            {
                energy += -neighbours[i][j].second * spins[i] *spins[neighbours[i][j].first];
            }
            energy += - magnetic_fields[i]*spins[i];
        }
        
        energy = energy/2;

        return energy;
    }
    //Return the change in energy for calculating a spin flip for an index. This is our selection criteria. WOrks for all graphs.
    double calculate_energy_change(int index)
    {
        //We use this approach rather than recalculating energy for the whole array, as it is much faster.
        //Only need to look at one index.
        double energy_change = 0;
        for (int j = 0; j<neighbours[index].size();j++)
        {
            energy_change += 2*neighbours[index][j].second * spins[index] *spins[neighbours[index][j].first];
        }

        energy_change = energy_change/2 + 2*magnetic_fields[index]*spins[index];;
        return energy_change;
    }

    //Simulation step method
    void simulation_step()
    {
        //Select random index
        int index = dis_spin(gen);
        //Select random number between 0 and 1
        double selection_prob = dis_sel(gen);
        //Calculate change in energy if index is flipped
        double energy_change = calculate_energy_change(index);
        //Calculate the boltzmann factor for this. 
        double exponential = exp(-energy_change / (kB * temperature));
        //Only select if the exponential is above the selection prob.
        if (selection_prob < exponential) 
        {
            spins[index] *= -1;
            energy += energy_change;
        }
        
    }

    //Functions to return private variables to other scopes
    //All gets return const, as other functions expect this!
    double get_temperature() const 
    {
        return temperature;
    }

    const vector<int>& get_spins() const 
    {
        return spins;
    }

    vector<double> get_fields() const 
    {
        return magnetic_fields;
    }

    double get_energy() const 
    {
        return energy;
    }

    vector<vector<std::pair<int,double>>> get_neighbours() const 
    {
        return neighbours;
    }

    int get_x_dim() const 
    {
        return x_dim;
    }

    int get_y_dim() const 
    {
        return y_dim;
    }

    int get_z_dim() const 
    {
        return z_dim;
    }


    // Function for other scopes to set a new temperature
    //Return 0 if success.

    int set_temperature(double new_temp)
    {
        temperature = new_temp;
        return 0;
    }

    int set_field(int index, double new_field)
    {
        if (index >= 0 && index < magnetic_fields.size())
        {
            magnetic_fields[index] = new_field;
            return 0;
        } else {
            return -1; //Invalid index
        }
    }


};