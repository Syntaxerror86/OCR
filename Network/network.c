// network.c
# include <stdio.h>

int networkLayers;
int networkSizes[3];
int biases[2];
int weights[2];

void initNetword(int sizes[], int layers){
    // Init number of layers
	networkLayers = layers;
    //Init sizes of layers
    for(int i = 0; i < layers; ++i){
        networkSizes[i] = sizes[i];
    }
}


int main(){
	int sizes[] = {2, 3, 1};
	initNetword(sizes, 3);
	return 0;
}
