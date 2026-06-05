/*
 * A visualisation of the 3D or 2D ising model that can be dynamically updated. 
 * Written by Hamish Cranston. 
 * The plotting features use Mathplot (https://github.com/sebsjames/mathplot.git) from sebsjames on github.
 * The scatterplot and graph projects were used as examples to help create the overall project.
 */
#include <iostream>
#include <fstream>
#include <cmath>
#include <array>
#include "utilities.h"

#include <sm/mathconst>
#include <sm/vec>
#include <sm/vvec>

#include <mplot/Visual.h>
#include <mplot/ColourMap.h>
#include <mplot/ScatterVisual.h>
#include <mplot/GraphVisual.h>
#include <thread>
#include <atomic>
#include <mutex>
#include   <string>
#include <sstream>

#include "sim-manager.h"
//Definitions
#define DIMENSION_X 10
#define DIMENSION_Y 10
#define DIMENSION_Z 10
#define DEFAULT_DELAY 100
#define DEFAULT_TEMP 2.0
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 800

//Atomics. These are used to communicate variables safely between threads and prevent race conditions, etc.
std::atomic<double> cli_temp{DEFAULT_TEMP}; //Default temp is 2.0.
std::atomic<double> cli_delay{DEFAULT_DELAY}; //Set default delay
std::atomic<bool> resize_request{false}; //This is used as a signpost for the resize.
std::atomic<int> x_n{10},y_n{10},z_n{10};


//Enum for CLI menu options
enum menu_options
{
    ADJUST_SIZE=1,
    ADJUST_TEMP=2,
    ADJUST_DELAY=3,
    QUIT = 4
};

//Print menu function.
void print_menu()
{
    std::cout << "Ising Simulation Menu: "<<std::endl;
    std::cout << "1. Adjust array size"<<std::endl;
    std::cout << "2. Adjust array temperature"<<std::endl;
    std::cout << "3. Adjust simulation display delay"<<std::endl;
    std::cout << "4. Quit simulation"<<std::endl;
}

//This thread runs the CLI and will write values to the atomics when needed. It can adjust size, temperature, and delay step parameters of the simulation.
void cli_thread(std::atomic<bool>& running)
{
    //Temporary variables
    double temp;
    int del;
    int x = DIMENSION_X;
    int y = DIMENSION_Y;
    int z = DIMENSION_Z;

    menu_options option_menu;

    do{
        print_menu();
        option_menu = (menu_options)read_integer("Choose a menu option.",1,4);

        switch(option_menu)
        {
            case ADJUST_SIZE:
                x = read_integer("X: ",1,30);
                y = read_integer("Y: ",1,30);
                z = read_integer("Z: ",1,30);

                x_n = x;
                y_n = y;
                z_n = z;

                resize_request = true;//Bool flag for change.

            break;
            case ADJUST_TEMP:
                temp = read_double("Enter temperature: ");
                cli_temp = temp;
            break;
            case ADJUST_DELAY:
                del = read_integer("Enter display delay steps: ");
                cli_delay = del;
            break;
            case QUIT:
            running=false;
            break;
        }

    }while(running && option_menu !=QUIT);

}




int main()
{
    std::atomic<bool> running{true};
    // Make sim objects. 
    simManager manager;
    //Initialise variables
    int sim_delay = 100;
    std::vector<double> energies;
    double average_energy = 0;
    std::stringstream ss;
    std::vector<double> temps;
    std::vector<double> average_energies; 
    bool temp_exist = false;

    //Initialise a simulation 
    if (manager.init_ising(DIMENSION_X,DIMENSION_Y,DIMENSION_Z,1) == -1)
    {
        std::cout << "Failed to initialise ising sim" << std::endl;
        return -1;
    }
    sm::vvec<sm::vec<float, 3>> points(DIMENSION_X*DIMENSION_Y*DIMENSION_Z);
    sm::vvec<float> data(DIMENSION_Y*DIMENSION_X*DIMENSION_Z);

    //This creates the overall visual window
    mplot::Visual v(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "3D Ising Model Simulation");


    //This creates the label objects (visual text models)
    mplot::VisualTextModel<>* energy_label = nullptr;
    mplot::VisualTextModel<>* temperature_label = nullptr;
    mplot::VisualTextModel<>* delay_label = nullptr;
    mplot::VisualTextModel<>* average_energy_label = nullptr;

    //This adds the labels to the visual environment. 
    v.addLabel("Temperature: ",{0.0f,0.0f,0.0f},temperature_label,0.01f);
    v.addLabel("Delay: ",{0.0f,-0.02f,0.0f},delay_label,0.01f);
    v.addLabel("Energy: ",{0.0f,-0.04f,0.0f},energy_label,0.01f);
    v.addLabel("Average Energy: ",{0.0f,-0.06f,0.0f},average_energy_label,0.01f);

    


    //Sets the closest distance the camera can see
    v.zNear = 0.001;
    //I had these, then got rid of them.
    v.showCoordArrows (false);
    // Set a white background:
    v.bgcolour = {1.0f, 1.0f, 1.0f, 1.0f};
    //These make it nicer
    v.lightingEffects();

    sm::vec<float, 3> offset = { 0.0, 0.0, 0.0 };

    // Set up scatter visual object - the main grid.
    auto sv = std::make_unique<mplot::ScatterVisual<float>> (offset);

    auto gv = std::make_unique<mplot::GraphVisual<double>> (sm::vec<float>({1.41*(DIMENSION_X/2) * 0.1f,1.41*(DIMENSION_Y/2) * 0.1f,0.0f}));
    v.bindmodel (sv);
    v.bindmodel (gv);
    //Initialise the energy graph.
    sm::vvec<double> init_t = {DEFAULT_TEMP};
    sm::vvec<double> init_e = {0.0};
    gv->setsize(1.0f, 1.0f);
    gv->setdata(init_t, init_e);
    gv->auto_rescale_x = true;
    gv->auto_rescale_y = true;
    //Add this so y dynamically rescales
    gv->auto_rescale_fit = true;
    gv->xlabel = "Temperature K";
    gv->ylabel = "Energy (arb)";

    //Finalise settings
    gv->finalize();
    //Data coordinates and colour coords.
    sv->setDataCoords (&points);
    sv->setScalarData (&data);

    //Radius of the points
    sv->radiusFixed = 0.03f;
    sv->cm.setType (mplot::ColourMapType::Plasma); //Colourmap
    //Build the visual model and set it to be somewhat transparent.
    sv->setAlpha (0.5f);
    sv->finalize();
    //Add the graph and scatter models to the view.
    auto svp = v.addVisualModel (sv); 
    auto gvp = v.addVisualModel (gv);
    //Set colour scaling of the scatter plot.
    svp->colourScale.compute_scaling (-0.45f, 0.45f);

    //This declares the CLI thread and starts it running.
    std::thread cli(cli_thread,std::ref(running));

    //The currently used dimension in the main thread. Start with initial values.
    int active_x = DIMENSION_X;
    int active_y = DIMENSION_Y;
    int active_z = DIMENSION_Z;

    //While loop that runs simulation
    while (!v.readyToFinish() && running) 
    {
        manager.isingTemp(0,cli_temp.load());//Load ensures we use the atomic as intended
        sim_delay = cli_delay.load();//Load the number of delay steps.

        //We use a boolean atomic for this, so we do not regenerate a resize every single step of the simulation.
        if (resize_request.exchange(false))
        {
            //Load new dimensions
            active_x = x_n.load();
            active_y = y_n.load();
            active_z = z_n.load();
            manager.reinit_ising(0, active_x, active_y, active_z);
            //Resize data and points
            points.resize(active_x * active_y * active_z);
            data.resize(active_x * active_y * active_z);
            //Reassign the data and scalar attributes of the scattered plot.
            svp->setDataCoords(&points);
            svp->setScalarData(&data);

            //Find the maximum sized axis - this will set the graph position
            int max_axis = 0;
            if(active_x > active_y && active_x > active_z)
            {
                max_axis = active_x;
            } else if (active_y > active_x && active_y > active_z)
            {
                max_axis = active_y;
            } else {
                max_axis = active_z;
            }

            //Calculate the new offset
            sm::vec<float, 3> new_offset = { 
                (float)(1.5*(max_axis / 2) * 0.1f), //1.41 is approximately root 2, accounts for radius of the cube or square.
                (float)(1.5*(max_axis / 2) * 0.1f), 
                0.0f //Z stays the same as its position is fixed in the view.
            };

            // On resize, set an absolute position for the graph, and clear the current data.
            sm::mat<float, 4> new_view;
            new_view.set_identity();  // local variable, not protected
            new_view.translate(new_offset);//Translate the view
            gvp->setViewMatrix(new_view); //Set the view of the graph
            temps.erase(temps.begin(),temps.end()); //Clear data
            average_energies.erase(average_energies.begin(),average_energies.end());
            gvp->clear_graph_data();
            gvp->abscissa_scale.reset();//Reset scale
            gvp->ord1_scale.reset();
            gvp->clearTexts();
        }

        //Run the number of steps in the delay so we can actually see changes.
        for (int n = 0;n<sim_delay;n++)
        {
            manager.step_all();
        }
        //Get the spins from the simulation by constant reference (dont want to edit them)
        const vector<int>& spins = manager.get_spins(0);

        //Draw the array using for loop.
        size_t idx = 0;
        for (int i = 0; i < active_x; ++i) 
        {
            for (int j = 0; j < active_y; ++j) 
            {  
                for (int k = 0; k < active_z; ++k)
                {
                    float x = (0.1f * (i - active_x / 2) + 0.1f);
                    float y = 0.1f * (j - active_y / 2);
                    float z = 0.1f * (k - active_z / 2);


                    points[idx] = {x, y, z};
                    data[idx] = spins[idx];
                    idx++;
                }

            }
        }

        
        //Calculate running average energy, and wipe the first element if it is longer than 100.
        if (energies.size() > 100)
        {
            energies.erase(energies.begin());
        }

        //Add the current energy to the simulation buffer.
        energies.push_back(manager.get_ising_data(0).get_energy());
        //Set average energy to zero and then calculate the mean.
        average_energy = 0;
        for (int i = 0;i<energies.size();i++)
        {
            average_energy += energies[i];
        }
        average_energy /= energies.size();

        //Add to the plot vectors

        temp_exist = false;
        //If the temperature exists, rewrite its energy value
        for (int i = 0; i<temps.size();i++)
        {
            if (temps[i] == manager.get_ising_data(0).get_temperature())
            {
                average_energies[i] = average_energy;
                temp_exist = true;
                break;
            }
        }
        //If the temperature does not exist, push back a new element to both.
        if(temp_exist == false)
        {
            temps.push_back(manager.get_ising_data(0).get_temperature());
            average_energies.push_back(average_energy);
        }

        // Sort the two value in pairs so they are ordered correctly.
        std::vector<std::pair<double,double>> temp_energy_pairs;
        for (size_t i = 0; i < temps.size(); i++) {
            temp_energy_pairs.push_back({temps[i], average_energies[i]});
        }
        std::sort(temp_energy_pairs.begin(), temp_energy_pairs.end());

        sm::vvec<double> sorted_temps(temp_energy_pairs.size());
        sm::vvec<double> sorted_energies(temp_energy_pairs.size());
        for (size_t i = 0; i < temp_energy_pairs.size(); i++) 
        {
            sorted_temps[i] = temp_energy_pairs[i].first;
            sorted_energies[i] = temp_energy_pairs[i].second;
        }

        //If there are temperatures and energies, update the graph.
        if (!temps.empty()) 
        {
            gvp->setlimits_x(sm::range<double>{sorted_temps.front(),sorted_temps.back()},true);
            gvp->setlimits_y(sm::range<double>{sorted_energies.front(),sorted_energies.back()},true);
            gvp->update(sorted_temps, sorted_energies, 0);
        }



        //Write Labels
        ss << "Average Energy: " << average_energy;
        average_energy_label->setupText(ss.str());
        ss.str("");
        ss << "Temperature: " << manager.get_ising_data(0).get_temperature() << " K";
        temperature_label->setupText(ss.str());
        ss.str("");
        ss << "Delay: " << sim_delay;
        delay_label->setupText(ss.str());
        ss.str("");
        ss << "Energy: " << manager.get_ising_data(0).get_energy();
        energy_label->setupText(ss.str());
        ss.str("");

        // Reinitialise on each loop.
        svp->reinit();
        if (v.readyToFinish()) { break; }  // guard before render
        v.wait (0.008);
        v.render();
    }
    //Things to deconstruct when program is over.
    running = false;
    //Stop the thread from running. Use detatch as it allows the thread to run in background.
    cli.detach();
    //_exit must be used due to issues with the openGL API - it crashes with segfault otherwise. Becuase openGL API wants control.
    _exit(0);
    return 0;
}
