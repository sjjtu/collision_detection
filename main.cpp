#include <stdio.h>
#include "ball.h"

const int T = 30;
const int FPS = 25;
const double dt = 1.0/FPS;
const int Ntime = T*FPS;
const int Nballs = 2;

void write_to_file(double content[Ntime][Nballs][2]){
    FILE *fp;
    /* your computations */
    fp = fopen("out.txt","w");
    for (int n = 0; n < Ntime; n++) {
        for (int i = 0; i < Nballs; i++) {
            fprintf(fp, "%f ", content[n][i][0]);
            fprintf(fp, "%f ", content[n][i][1]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

int main() {
    
    auto coordinates = new double[Ntime][Nballs][2]; // init array holding storing values of coordinates
    
    Ball ball[] = {
                    Ball(1,1, 0, 0, 1, 2), 
                    Ball(1,1, 10, 0, -1, 2)
                    };
    
    for(int n=0;n<Ntime;n++){ // loop through time
        for (int i = 0; i < Nballs; i++){
            coordinates[n][i][0] = ball[i].position_x;
            coordinates[n][i][1] = ball[i].position_y;
            ball[i].update(dt); 
        }
    }

    write_to_file(coordinates);
    
}