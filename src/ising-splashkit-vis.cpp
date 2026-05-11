#include "ising-model.h"

#include <splashkit.h>

#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800


void draw_ising_grid(const isingData &data,int padding){
    int cell_size = ceil((WINDOW_WIDTH - 2*padding)/data.size);

    for( int i = 0; i<data.size; i++){
        for (int j = 0; j<data.size; j++){
            int index = i*data.size + j;
            if (data.spins[index]==1){
                fill_rectangle(color_red(),padding+j*cell_size,padding+i*cell_size,cell_size,cell_size);
            }
            else{
                fill_rectangle(color_blue(),padding+j*cell_size,padding+i*cell_size,cell_size,cell_size);
            }
        }
    }
}



int main(){
    isingData data;
    double energy;

    data.temperature = 0.1;
    energy = data.calculate_total_energy();


    open_window("Ising Model",800,800);
    clear_screen(color_white());
    draw_ising_grid(data,20);

    while (!quit_requested()){
        for (int i = 0; i<100000; i++){
        data.simulation_step();
        }
        clear_screen(color_white());
        draw_ising_grid(data,0);
        refresh_screen(60);
        process_events();

    }

    return 0;
}