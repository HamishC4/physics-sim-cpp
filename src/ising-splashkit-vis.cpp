
#include "sim-manager.h"

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
    simManager manager;
    if (manager.init_ising("2D_Square",200,0.01) == -1){
        std::cout << "Failed to initialise ising sim" << std::endl;
        return -1;
    }

    if (manager.init_ising("2D_Square",200,300) == -1){
    std::cout << "Failed to initialise ising sim" << std::endl;
    return -1;
    }
    window win0 = open_window("GUI_test",800,800);
    window win1 = open_window("Ising Model 1",800,800);
    window win2 = open_window("Ising Model 2",800,800);


    bool winopen1 = true;
    bool winopen2 = true;
    bool winopen0 = true;


    set_current_window(win2);
    clear_screen(color_white());
    draw_ising_grid(manager.get_ising_data(1),20);

    set_current_window(win1);
    clear_screen(color_white());
    draw_ising_grid(manager.get_ising_data(0),20);


    while ((winopen1||winopen1||winopen0)){

        process_events();

        if(winopen2 && window_close_requested(win2)){
            close_window(win2);
            winopen2 = false;
            write_line(to_string(winopen1 ||winopen2));
        } else if (winopen2){
            set_current_window(win2);
            clear_screen(color_white());
            draw_ising_grid(manager.get_ising_data(1),0);
            refresh_screen(60);
        }

        if(winopen0 && window_close_requested(win0)){
            close_window(win0);
            winopen0 = false;
        }

        if(winopen1 && window_close_requested(win1)){
            close_window(win1);
            winopen1 = false;
            write_line(to_string(winopen1 ||winopen2));
        } else if (winopen1){
            set_current_window(win1);
            clear_screen(color_white());
            draw_ising_grid(manager.get_ising_data(0),0);
            refresh_screen(60);
        } 




        for (int i = 0; i<10000; i++){
            manager.step_all();
        }






    }



    return 0;
}