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

#include "ising-model.h"



class simManager
{

private:
    vector<isingData> ising_sims;

public:
    simManager(){
        //No initialisation required yet.
    }; //Do not need to initialise this constructor with anything. no default required.

    int init_ising(string type, int size, double temperature){
        if (type == "2D_Square"){
            isingData new_sim = isingData(size);
            new_sim.temperature = temperature;
            ising_sims.push_back(new_sim);

        }
        return ising_sims.size() - 1; //Returns -1 if the type cannot be made, otherwise makes the sim.
    }

    int step_ising(int index){
        if (index >= 0 && index < ising_sims.size()){
            ising_sims[index].simulation_step();
            return 0;
        } else {
            return -1; //Invalid index
        }
    }

    int step_all(){
        for (int i = 0; i < ising_sims.size();i++){
            if (step_ising(i) == -1){
                return i;//Returned failed index.
            }
        }
        return 0;
    }

    int isingTemp(int index, double temperature){
        if (index >= 0 && index < ising_sims.size()){
            ising_sims[index].temperature = temperature;
            return 0;
        } else {
            return index; //Invalid index
        }
    }

    vector<int> get_spins(int index){
        if (index >= 0 && index < ising_sims.size()){
            return ising_sims[index].spins;
        } else {
            return vector<int>(); //Invalid index
        }
    }

    isingData get_ising_data(int index){
        if (index >= 0 && index < ising_sims.size()){
            return ising_sims[index];
        } else {
            return isingData(0); //Invalid index
        }
    }

};