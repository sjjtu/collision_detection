#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <mpi.h>

#include "ball.h"
#include "Detector.h"
#include "BallSorter.h"

const double T = 30;
const int FPS = 50;
const double dt = 1.0/FPS;
const int Ntime = T*FPS;
const int Nballs = 160;
const int BOUND_LEFT = 0;
const int BOUND_RIGHT = 100;
const int BOUND_TOP = 100;
const int BOUND_BOT = 0;
const int MAX_VEL = 5;

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

void print_local_balls(int p, Ball *balls, int sz, std::string header=""){
    cout << header << endl;
    cout << "Processor " << p << " with balls: ";
    for(int i=0;i<sz;i++){
        cout << "    ball " << balls[i].id << ": x:" << balls[i].position_x << "  y:" << balls[i].position_y
        << " vx:" << balls[i].velocity_x << " vy:" << balls[i].velocity_y;
    }
    cout << endl;
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
        uniform_int_distribution<> vel_distr(-MAX_VEL,MAX_VEL);
        n = p*L+min(p,R)+i;
        
        balls[i] = Ball(1,1,pos[i].first, pos[i].second, vel_distr(g),vel_distr(g), n);
    }

    return balls;
    
}

float * balls_to_array(Ball *balls, int sz){
    float *arr = (float *) malloc(sz*7*__SIZEOF_FLOAT__); // 7 floats to charcterise one ball
    for(int i=0; i<sz;i++){
        arr[i*7+0] = balls[i].radius;
        arr[i*7+1] = balls[i].mass;
        arr[i*7+2] = balls[i].position_x;
        arr[i*7+3] = balls[i].position_y;
        arr[i*7+4] = balls[i].velocity_x;
        arr[i*7+5] = balls[i].velocity_y;
        arr[i*7+6] = balls[i].id;
    }
    return arr;
}

Ball * array_to_balls(float *arr, int sz_balls){
    Ball *balls = (Ball *) malloc(sz_balls*sizeof(Ball));
    for(int i=0; i<sz_balls;i++){
        balls[i].radius = arr[i*7+0];
        balls[i].mass = arr[i*7+1];
        balls[i].position_x = arr[i*7+2];
        balls[i].position_y = arr[i*7+3];
        balls[i].velocity_x = arr[i*7+4];
        balls[i].velocity_y = arr[i*7+5];
        balls[i].id = arr[i*7+6];
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

    /* generate balls
            divide x axis into P intervals; each processor generates balls in one interval */
    float dx = (BOUND_RIGHT - BOUND_LEFT) / P;
    float bleft = p*dx;
    float bright = bleft + dx;
    Ball *balls_local = generate_balls(I, p, L, R, bleft, bright, BOUND_BOT, BOUND_TOP);
    
    //print_local_balls(p, balls_local, I, "local balls");
    //Ball balls_local[] = {Ball(1,1,48, 50, 1, 0, 0), Ball(1,1,52,50,-1,0,1)};


    Ball *balls_global = (Ball *) malloc(Nballs * sizeof(Ball));
    float *temp_balls_global, *temp_balls_local;
    int start_ind; // starting index for processor p

    double starttime = MPI_Wtime();

    for(int n=0;n<Ntime;n++){ // loop through time

        /* Use MPI_Allgather to exchange all balls with other processors */
        temp_balls_local = balls_to_array(balls_local, I);
        temp_balls_global = (float *) malloc(Nballs * 7 * __SIZEOF_FLOAT__);
        MPI_Allgather(temp_balls_local, (I)*7, MPI_FLOAT, temp_balls_global, (I)*7, MPI_FLOAT, MPI_COMM_WORLD); // TODO: use allgatherv
        
        balls_global = array_to_balls(temp_balls_global, Nballs);
        BallSorter::sort_balls(balls_global, Nballs); // TODO: parallel sort

        /* Save current position of ALL balls */
        for (int i = 0; i < Nballs; i++){
            int global_ball_ind = (int) balls_global[i].id;
            coordinates[n][global_ball_ind][0] = balls_global[i].position_x;
            coordinates[n][global_ball_ind][1] = balls_global[i].position_y;
        }

        /* Split up balls */
        start_ind = p*L+min(p,R); // starting index for processor p
        memcpy(balls_local, balls_global+start_ind, I*sizeof(Ball));
        
        /* Collision detection */ // TODO: check collision between regions
        Detector det(balls_local, I, Nballs);
        

        //cout << "time step " << n << "\n";
        det.sweep_and_prune();
        det.update_velocity(n);
        det.collision_with_boundary(I,n, BOUND_LEFT, BOUND_RIGHT, BOUND_TOP, BOUND_BOT);
            
        /* Update position of local balls*/
        for (int i = 0; i < I; i++){
            balls_local[i].update(dt); 
        }
        
    }

    double endtime = MPI_Wtime();

    if(p==0) cout << "Nballs: " << Nballs << " NProc: " << P << " Ntime: " << Ntime << " Time (s): " << endtime - starttime << endl;

    if(p==0){write_to_file(coordinates);}

    MPI_Finalize();
}