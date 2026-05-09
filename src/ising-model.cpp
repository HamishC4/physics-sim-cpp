#include "ising-model.h"


int main(){
    isingData data;
    double energy;
    data.print_ising_grid();

    energy = data.calculate_total_energy();
    std::cout << "Energy: " << energy << std::endl;

    std::cout << "Run simulation for 100 steps:" << std::endl;

    for (int i = 0; i<1000;i++){

        for (int j =0; j<100;j++){
        data.simulation_step();
        }
        data.print_ising_grid();
        std::cout << i << "=============================" << std::endl;
        std::cout <<"Energy: " << data.energy << std::endl;
        //Use this to clear the terminal so that it evolves in place.
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::system("clear");
       
    }


    return 0;
}