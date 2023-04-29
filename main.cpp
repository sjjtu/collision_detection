#include <cstdlib>
#include <stdio.h>
#include "ball.h"
#include "Detector.h"
#include <iostream>
#include <random>
#include <algorithm>

const double T = 30;
const int FPS = 25;
const double dt = 1.0/FPS;
const int Ntime = T*FPS;
const int Nballs = 90;
const int BOUND_LEFT = 0;
const int BOUND_RIGHT = 100;
const int BOUND_TOP = 100;
const int BOUND_BOT = 0;

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

Ball * generate_balls(int num_balls) {
    vector<float> l_x;
    vector<float>  l_y;
    Ball *balls = new Ball[num_balls];
    for (int i = BOUND_LEFT+5; i < BOUND_RIGHT-5; ++i) { // no balls at boundary
        l_x.push_back(i+1.0);
    }
    for (int i = BOUND_BOT+5; i < BOUND_TOP-5; ++i) { // no balls at boundary
        l_y.push_back(i+1);
    }

    std::random_device rd;
    std::mt19937 g(rd());
 
    std::shuffle(l_x.begin(), l_x.end(), g);
    std::shuffle(l_y.begin(), l_y.end(), g);


    for(int i=0;i<num_balls;i++){
        uniform_int_distribution<> vel_distr(-10,10);
        balls[i] = Ball(1,1,l_x[i], l_y[i], vel_distr(g),vel_distr(g));
    }

    return balls;
    
}

int main(int args, char **kwargs) {
    
    auto coordinates = new double[Ntime][Nballs][2]; // init array holding storing values of coordinates

    Ball *ball = generate_balls(Nballs);
    Detector det(ball, Nballs);

    for(int n=0;n<Ntime;n++){ // loop through time
        //cout << "time step " << n << "\n";
        det.sweep_and_prune();
        det.update_velocity(n);
        det.collision_with_boundary(Nballs,n, BOUND_LEFT, BOUND_RIGHT, BOUND_TOP, BOUND_BOT);
        
        for (int i = 0; i < Nballs; i++){
            
            coordinates[n][i][0] = ball[i].position_x;
            coordinates[n][i][1] = ball[i].position_y;
            ball[i].update(dt); 
        }
    }


    write_to_file(coordinates);
}