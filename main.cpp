#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <mpi.h>

#include "ball.h"
#include "Detector.h"
#include "BallSorter.h"

const int T = 30;
const int FPS = 50;
const double dt = 1.0/FPS;
constexpr int Ntime = T*FPS;
const int BOUND_LEFT = 0;
const int BOUND_RIGHT = 500;
const int BOUND_TOP = 500;
const int BOUND_BOT = 0;
const int MAX_VEL = 5;
const double TOL = 1E-2;

void write_meta(double meta[5]){
    FILE *fp;
    fp = fopen("meta.txt","w");
    for (int n = 0; n < 5; n++) {
        fprintf(fp, "%f ", meta[n]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void write_to_file(double content[][Ntime][2], int Nballs){
    FILE *fp;
    fp = fopen("out.txt","w");
    for (int n = 0; n < Ntime; n++) {
        for (int i = 0; i < Nballs; i++) {
            fprintf(fp, "%f ", content[i][n][0]);
            fprintf(fp, "%f ", content[i][n][1]);
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

void print_local_balls_id(int p, Ball *balls, int sz, std::string header=""){
    cout << header << endl;
    cout << "Processor " << p << " with balls: ";
    for(int i=0;i<sz;i++){
        cout << "    ball " << balls[i].id;
    }
    cout << endl;
}

Ball * generate_balls(int num_balls, int p, int P, int L, int R, float bleft, float bright, float bbot, float btop) {
    vector<pair<float, float>> pos;
    Ball *balls = new Ball[num_balls];
    int c = 0;
    int thres_l = 0;  // threshold for left and right boundary, only for first p
    if(p==0) thres_l = 2;
    int thres_r = 0;  // threshold for left and right boundary, only for last p
    if(p==P-1) thres_r = 2;
    for (float j = bbot+2; j < btop-2; j+=2.1) { // no balls at boundary and no touching balls
        for (float i = bleft+thres_l; i < bright-thres_r; i+=2.1) { // no balls at boundary and no touching balls
            pos.push_back(make_pair(i+0.0, j+0.0));  
            c++; 
        }
    }
    //cout << "generating " << c << " balls in " << bleft << " " << bright << " and " <<bbot << " " << btop << endl;
    if(c < num_balls) {
        printf("field too small!");
        exit(1);
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

float * merge(float *l, float *r, int s){
    int i = 0;
    int j = 0;
    int end = 2*s;
    float *temp = (float *) malloc(end*7*__SIZEOF_FLOAT__);
    for(int k=0;k<end;k++){
        if(i<s && (j>=s || l[7*i+2]<r[7*j+2])){
            temp[k*7+0] = l[i*7+0];
            temp[k*7+1] = l[i*7+1];
            temp[k*7+2] = l[i*7+2];
            temp[k*7+3] = l[i*7+3];
            temp[k*7+4] = l[i*7+4];
            temp[k*7+5] = l[i*7+5];
            temp[k*7+6] = l[i*7+6];
            i++;
        } else {
            temp[k*7+0] = r[j*7+0];
            temp[k*7+1] = r[j*7+1];
            temp[k*7+2] = r[j*7+2];
            temp[k*7+3] = r[j*7+3];
            temp[k*7+4] = r[j*7+4];
            temp[k*7+5] = r[j*7+5];
            temp[k*7+6] = r[j*7+6];
            j++;
        }
    }
    return temp;
}

int main(int argc, char **argv) {

    /* Find problem size N from command line */
    if (argc < 2) {
        printf("No size N given\n");
        exit(1);
    }
    int Nballs = atoi(argv[1]);
    int P, p, tag;
    MPI_Status status;
    tag = 123;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);

    if(Nballs % P != 0) {
        cout << "N must be multiple of P\n";
        exit(1);
    }

    /*We assume linear data distribution. The formulae according to the lecture are: */
    int L = (int) Nballs/P;
    int R = Nballs % P;
    int I = (int) (Nballs+P-p-1)/P; //(number of local elements)

    /* generate balls: divide x axis into P intervals; each processor generates balls in one interval */
    float dx = ((float) BOUND_RIGHT - BOUND_LEFT) / P;
    float bleft = p*dx;
    float bright = bleft + dx;
    Ball *balls_local = generate_balls(I, p, P, L, R, bleft, bright, BOUND_BOT, BOUND_TOP);
    // print_local_balls(p, balls_local, I, "local balls");
    // Ball *balls_local = (Ball *) malloc(sizeof(Ball));
    // if(p==0) balls_local[0] = {Ball(1,1,52,50,-1,0,1)};
    // if(p==1) balls_local[0] = {Ball(1,1,48, 50, 1, 0, 0)};

    double starttime = MPI_Wtime();

    Ball *balls_global = (Ball *) malloc(Nballs * sizeof(Ball));
    //float *temp_balls_global, *temp_balls_local;
    float *temp_recv, *temp_send;
    int start_ind; // starting index for processor p
    int left_ghost; // number of left boundary balls
    int right_ghost; // number of right boundary balls

    
    int height = (int) log2(P); // make sure that P is power 2! //TODO: check that?
    if(height != log2(P)) {
        cout << "P must be power of 2!\n";
        exit(1);
    }
    int dest_p;
    int sending_sz;

    temp_recv = (float *) malloc(Nballs/2.0*7*__SIZEOF_FLOAT__);
    temp_send = (float *) malloc(Nballs*7*__SIZEOF_FLOAT__);

    //vector<vector<vector<double>>> coordinates(Nballs, vector<vector<double>>(Ntime, vector<double>(2))); // init array holding storing values of coordinates
    auto coordinates = new double[Nballs][Ntime][2]; // init array holding storing values of coordinates

    for(int n=0;n<1;n++){ // loop through time

        /* Use parallel merge sort to sort and exchange balls with all processors.*/
        BallSorter::sort_balls(balls_local, I);

        sending_sz = I;
        temp_send = balls_to_array(balls_local, I);
        for(int i=0;i<height;i++){
            
            dest_p = p ^ (int) pow(2,i);
            MPI_Sendrecv(temp_send, sending_sz*7, MPI_FLOAT, dest_p, tag, 
                        temp_recv, sending_sz*7, MPI_FLOAT, dest_p, tag, 
                        MPI_COMM_WORLD, &status);
            temp_send = merge(temp_recv, temp_send, sending_sz);
            //memcpy(temp_send, temp_merge, sending_sz*7*2*__SIZEOF_FLOAT__);
            sending_sz *= 2;
        }

        balls_global = array_to_balls(temp_send, Nballs);

        /* Use MPI_Allgather to exchange all balls with other processors */
        // temp_balls_local = balls_to_array(balls_local, I);
        // temp_balls_global = (float *) malloc(Nballs * 7 * __SIZEOF_FLOAT__);
        // MPI_Allgather(temp_balls_local, (I)*7, MPI_FLOAT, temp_balls_global, (I)*7, MPI_FLOAT, MPI_COMM_WORLD);

        // balls_global = array_to_balls(temp_balls_global, Nballs);
        // BallSorter::sort_balls(balls_global, Nballs);

        free(temp_send);
        //free(temp_recv);

        /* Save current position of ALL balls */
        if(p==0){
            for (int i = 0; i < Nballs; i++){
                int global_ball_ind = (int) balls_global[i].id;
                coordinates[global_ball_ind][n][0] = balls_global[i].position_x;
                coordinates[global_ball_ind][n][1] = balls_global[i].position_y;
            }
        }

        // /* Split up balls */
        start_ind = p*L+min(p,R); // starting index for processor p
        left_ghost = 0;
        right_ghost = 0;
        // include all balls right to the boundary that might collide with local balls
        while(p<P-1 && right_ghost <= I && balls_global[start_ind+I-1].position_x+balls_global[start_ind+I-1].radius >= balls_global[start_ind+I+right_ghost].position_x-balls_global[start_ind+I+right_ghost].radius - TOL){
            right_ghost++;
            //cout << "time " << n << " adding ball to porcessor " << p<< endl;
        }
        // same with left boundary
        while(0<p && left_ghost <= I && balls_global[start_ind].position_x-balls_global[start_ind].radius <= balls_global[start_ind-left_ghost-1].position_x+balls_global[start_ind-left_ghost-1].radius +TOL){
            left_ghost++;
            //cout << "time " << n << " adding ball to porcessor " << p<< endl;
        }
        balls_local = (Ball *) malloc((I+left_ghost+right_ghost)*sizeof(Ball));
        memcpy(balls_local, balls_global+start_ind-left_ghost, (I+left_ghost+right_ghost)*sizeof(Ball));
        
        //cout << "left ghost balls: " << left_ghost << " right ghost balls: " << right_ghost << " local: " << I << endl;
        /*Local Collision detection including boundary balls*/
        Detector det(balls_local, (I+left_ghost+right_ghost), Nballs);
        //cout << "time step " << n << "\n";
        det.sweep_and_prune();
        det.update_velocity(n);
        det.collision_with_boundary(n, BOUND_LEFT, BOUND_RIGHT, BOUND_TOP, BOUND_BOT);
            
        /* Update position of local balls without boundary balls*/
        
        for (int i = 0; i < I; i++){
            balls_local[i] = balls_local[i+left_ghost]; // overwrite left ghosts
            balls_local[i].update(dt); 
        }        

    //     //cout << n << endl;
    }

    double endtime = MPI_Wtime();

    if(p==0) cout << "Nballs: " << Nballs << " NProc: " << P << " Ntime: " << Ntime << " Time (s): " << endtime - starttime << endl;

    if(p==0){
        write_to_file(coordinates, Nballs); 
        double meta[] = {BOUND_LEFT, BOUND_RIGHT, BOUND_BOT, BOUND_TOP, FPS}; 
        write_meta(meta);
    }

    MPI_Finalize();
}
