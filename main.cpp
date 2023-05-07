#include <cstdlib>
#include <stdio.h>
#include "ball.h"
#include "Detector.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <mpi.h>

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

Ball * generate_balls(int num_balls, int p, int L, int R, int bleft, int bright, int bbot, int btop) {
    vector<pair<float, float>> pos;
    Ball *balls = new Ball[num_balls];
    for (float j = bbot+2; j < btop-2; j+=2.1) { // no balls at boundary and no touching balls
        for (float i = bleft+2; i < bright-2; i+=2.1) { // no balls at boundary and no touching balls
            pos.push_back(make_pair(i+0.0, j+0.0));   
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
 
    std::shuffle(pos.begin(), pos.end(), g);

    int n; // (global index for given (p,i)
    for(int i=0;i<num_balls;i++){
        uniform_int_distribution<> vel_distr(-10,10);
        n = p*L+min(p,R)+i;
        
        balls[i] = Ball(1,1,pos[i].first, pos[i].second, vel_distr(g),vel_distr(g), n);
    }

    return balls;
    
}

int main(int argc, char **argv) {

    int P, p, tag;
    MPI_Status status;
    tag = 123;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);

    // We assume linear data distribution. The formulae according to the lecture are:
    int L = (int) Nballs/P;
    int R = Nballs % P;
    int I = (int) (Nballs+P-p-1)/P; //(number of local elements)
    //int n = p*L+min(p,R)+i; // (global index for given (p,i)

    auto coordinates = new double[Ntime][Nballs][2]; // init array holding storing values of coordinates

    cout << I << "\n";
    /* generate balls
            divide x axis into P intervals; each processor generates balls in one interval */
    float dx = (BOUND_RIGHT - BOUND_LEFT) / P;
    float bleft = p*dx;
    float bright = bleft + dx;
    Ball *ball = generate_balls(I, p, L, R, bleft, bright, BOUND_BOT, BOUND_TOP);
    //Ball ball[] = {Ball(1,1,48, 50, 1, 0, 0), Ball(1,1,52,50,-1,0,1)};
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

    if(p==0){write_to_file(coordinates);}

    MPI_Finalize();
}