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

#include "ising-model.h"



class simManager
{

private:
    //All data is private.
    vector<isingData> ising_sims;

public:

    //Default constructor
    simManager()
    {
        //No initialisation required yet.
    }; 



    //Initialise a new simulation
    int init_ising(int x, int y, int z, double temperature)
    {
        isingData new_sim = isingData(x,y,z);
        new_sim.set_temperature(temperature);
        ising_sims.push_back(new_sim);
        return ising_sims.size() - 1; //Returns -1 if the type cannot be made, otherwise makes the sim.
    }


    //Step the ising simulation
    int step_ising(int index)
    {
        if (index >= 0 && index < ising_sims.size())
        {
            ising_sims[index].simulation_step();
            return 0;
        } else {
            return -1; //Invalid index
        }
    }

    //Simulation step for all active simulations
    int step_all()
    {
        for (int i = 0; i < ising_sims.size();i++)
        {
            if (step_ising(i) == -1)
            {
                return i;//Returned failed index.
            }
        }
        return 0;
    }

    //Used to set temperature at a specific index.
    int isingTemp(int index, double temperature)
    {
        if (index >= 0 && index < ising_sims.size())
        {
            ising_sims[index].set_temperature(temperature);
            return 0;
        } else {
            return index; //Invalid index
        }
    }

    //Used to access the whole object to modify, etc.
    const vector<int>& get_spins(int index) const 
    {
        if (index >= 0 && index < ising_sims.size())
        {
            return ising_sims[index].get_spins();
        } 
        throw std::out_of_range("get_spins: invalid index " + std::to_string(index));
    }

    //Method to access spins
    const isingData& get_ising_data(int index)
    {
        if (index >= 0 && index < ising_sims.size())
        {
            return ising_sims[index];
        } 
        throw std::out_of_range("get_ising_data: invalid index " + std::to_string(index));
    }

    //Use this to reinitialise the ising lattice at a given index. Used for resizing.
    int reinit_ising(int index,int x, int y, int z)
    {
        if (index >= 0 && index < ising_sims.size())
        {
            double old_temp = ising_sims[index].get_temperature();

            isingData new_sim = isingData(x,y,z);
            new_sim.set_temperature(old_temp);
            ising_sims[index]  = new_sim;

            return index; // Return the index of the re-initialised sim
        } else {
            return -1; //Invalid index
        }



    }


};