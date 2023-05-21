#include <time.h>
#include <stdlib.h>
#define NUM_HIDDEN_NEURONS 64
#define NUM_HIDDEN_LAYERS
typedef struct neuron {
     int bias;
     int activation;
     int weights[NUM_HIDDEN_NEURONS];


} neuron;

int relu(int sum){
    if(sum > 0){
        return sum;
    }
    return 0;
}
int d_relu(int sum){
    if(sum > 0){
        return 1;
    }
    return 0;
}
int input[64];


int main()
{
    srand(time(NULL));
    
}