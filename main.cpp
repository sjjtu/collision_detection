#include <stdio.h>
#include "ball.h"
#include "Detector.h"
#include <iostream>

const double T = 10;
const int FPS = 25;
const double dt = 1.0/FPS;
const int Ntime = T*FPS;
const int Nballs = 4;

void write_to_file(double content[Ntime][Nballs][2]){
    FILE *fp;
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
                    Ball(1,1, 0, 0, 2, 1), 
                    Ball(1,1, 0, 10, 2, -1),
                    Ball(1,1, 5, 0, -1, 2),
                    Ball(1,1, 5, 10, -1, -2),
                    };
    Detector det(ball, Nballs);

    for(int n=0;n<Ntime;n++){ // loop through time
        cout << "time step " << n << "\n";
        det.sweep_and_prune();
        det.update_velocity(n);
        
        for (int i = 0; i < Nballs; i++){
            
            coordinates[n][i][0] = ball[i].position_x;
            coordinates[n][i][1] = ball[i].position_y;
            ball[i].update(dt); 
        }
    }


    write_to_file(coordinates);
}