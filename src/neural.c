
#define NUM_HIDDEN_NEURONS 100
#define NUM_HIDDEN_LAYERS 3
#define NUM_INPUTS 708
#define NUM_OUTPUTS 3
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define PI 3.141592653589793238462

typedef struct neuron {
    double weights[NUM_HIDDEN_NEURONS];
    double bias;
    double output;
} neuron;
typedef struct ineuron {
    double weights[NUM_INPUTS];
    double bias;
    double output;
} ineuron;
typedef struct network {
    ineuron input_layer[NUM_HIDDEN_NEURONS];
    neuron  hidden_layers[NUM_HIDDEN_LAYERS-1][NUM_HIDDEN_NEURONS];
    neuron  output_layer[NUM_OUTPUTS];
} network;



double r2()
{
    return (double)rand() / (double)RAND_MAX ;
}
double weight_init_value(int index){
    if(index % 2 == 1){
        return sqrt(-2*log(r2()))*cos(2*PI*r2());
    }else{
        return sqrt(-2*log(r2()))*sin(2*PI*r2());
    }
}
int init_network(network *network){
    
    double INIT_MULTIPLIER = sqrt(2.0/NUM_INPUTS);
    for (int i = 0; i < NUM_HIDDEN_NEURONS; i++)
    {
        network->input_layer[i].bias = 0;
        for(int j = 0; j < NUM_INPUTS;j++){
            network->input_layer[i].weights[j] = weight_init_value(j) * INIT_MULTIPLIER;
        }
    }
    for (int i = 0; i < NUM_HIDDEN_LAYERS-1; i++)
    {
        for (int j = 0; j < NUM_HIDDEN_NEURONS; j++)
        {
            network->hidden_layers[i][j].bias = 0;
            for (int k = 0; k < NUM_HIDDEN_NEURONS; k++)
            {
                network->hidden_layers[i][j].weights[k] = weight_init_value(k) * INIT_MULTIPLIER;
            }
            
        }
        
    }
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        network->output_layer[i].bias = 0;
        for (int j = 0; j < NUM_HIDDEN_NEURONS; j++)
        {
            network->output_layer[i].weights[j] = weight_init_value(j) * INIT_MULTIPLIER;
        }
        
    }

}


int main(){
    srand(time(NULL));  

    network *current_network = malloc(sizeof(network));
    network *next_network = malloc(sizeof(network));

    FILE *network_file = fopen("../assets/network", "r+");
    fread(current_network,sizeof(network),1,network_file);
    long size = ftell(network_file);
    if (0 == size) {
        init_network(current_network);
    }
    printf("%f",current_network->hidden_layers[0][1].weights[0]);
    fwrite(current_network,sizeof(network),1,network_file);
}