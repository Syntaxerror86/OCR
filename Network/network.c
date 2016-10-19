// network.c
# include <stdio.h>

struct Network{
    int numLayers;
    int sizes[3];
    float biases[2];
    float weight[2];
};

void editNumLayers(struct Network *n){
    //Useless function just to see how it works
    //This function edit tje property numLayers of a Network
    n->numLayers = 5;
}

int main(){
    //Init Network
    struct Network network;
    // Init number of layers
    network.numLayers = 3;
    //Init sizes of the layers
    int sizes[] = {2, 3, 1};
    for(int i = 0; i < network.numLayers; ++i){
        network.sizes[i] = sizes[i];
    }
	return 0;
}
