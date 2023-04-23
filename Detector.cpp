#include "Detector.h"
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <cmath>

Detector::Detector(Ball *ball, int nBalls){

    this->nBalls = nBalls;

    this->ball = ball;
    this->x_coord.reserve(this->nBalls);
    this->y_coord.reserve(this->nBalls);

    this->counter_table = vector<vector<int>> (nBalls, vector<int>(nBalls));

    for(int i=0;i<nBalls;i++){
        x_coord.push_back(i);
        y_coord.push_back(i);;

        
    }
}

void Detector::sort_axis(){ 
    sort(x_coord.begin(), x_coord.end(), [=](int a, int b)->bool{
        return (ball[a].position_x-ball[a].radius) < (ball[b].position_x - ball[b].radius);
    });

    sort(y_coord.begin(), y_coord.end(), [=](int a, int b)->bool{
        return (ball[a].position_y-ball[a].radius) < (ball[b].position_y - ball[b].radius);
    });
};

void Detector::sweep_and_prune() {
    sort_axis();

    this->status_list_x.clear();
    this->status_list_y.clear();

    this->counter_table = vector<vector<int>> (nBalls, vector<int>(nBalls));


    for(int i=0;i<x_coord.size();i++) {
        // X AXIS
        for(int j=0;j<status_list_x.size();++j){
            //cout << "comparing " << status_list_x[j] << " with " << x_coord[i] << endl;
            if(ball[x_coord[i]].position_x-ball[x_coord[i]].radius > ball[status_list_x[j]].position_x+ball[status_list_x[j]].radius) {
                //cout << " object is poped " << status_list_x[j] << endl;
                status_list_x.erase(status_list_x.begin() + j);
                j--;
            } else {
                //cout << "reported pair x " << status_list_x[j] << " with " << x_coord[i] << endl;
                // cout << "    x coordinates: " << ball[x_coord[i]].position_x << " and " << ball[status_list_x[j]].position_x << endl;
                counter_table[x_coord[i]][status_list_x[j]] += 1;
                counter_table[status_list_x[j]][x_coord[i]] += 1; //matrix is symmetric??
            }
        }
        //cout << "adding to status list " << x_coord[i] << endl;
        status_list_x.push_back(x_coord[i]);
        
    }

    for(int i=0;i<y_coord.size();i++) {
        // Y AXIS
        for(int j=0;j<status_list_y.size();++j){
            //cout << "comparing " << status_list_x[j] << " with " << x_coord[i] << endl;
            if(ball[y_coord[i]].position_y-ball[y_coord[i]].radius > ball[status_list_y[j]].position_y+ball[status_list_y[j]].radius) {
                //cout << " object is poped " << status_list_x[j] << endl;
                status_list_y.erase(status_list_y.begin() + j);
                j--;
            } else {
                //cout << "reported pair y " << status_list_y[j] << " with " << y_coord[i] << endl;
                // cout << "    y coordinates: " << ball[y_coord[i]].position_y << " and " << ball[status_list_y[j]].position_y << endl;
                counter_table[y_coord[i]][status_list_y[j]] += 1;
                counter_table[status_list_y[j]][y_coord[i]] += 1; //matrix is symmetric??
            }
        }
        //cout << "adding to status list " << x_coord[i] << endl;
        status_list_y.push_back(y_coord[i]);
    }
}

bool Detector::detect_collision(int ball_i, int ball_j){
    return pow(ball[ball_i].position_x-ball[ball_j].position_x, 2) 
        + pow(ball[ball_i].position_y-ball[ball_j].position_y, 2) <= 
                                                                            pow(ball[ball_i].radius + ball[ball_j].radius, 2); 
}

void Detector::update_velocity(int time_step) {
    for(int i=0;i<nBalls;i++) {
        for(int j=0;j<i;j++){ // only need lower half of matrix
            if (counter_table[i][j] == 2 && detect_collision(i, j)){
                printf("collision detected between %d and %d in time step %d \n", i, j, time_step);
                // Compute the total x and y momenta before the collision
                double px_before = ball[i].mass*ball[i].velocity_x+ball[j].mass*ball[j].velocity_x;
                double py_before = ball[i].mass*ball[i].velocity_y+ball[j].mass*ball[j].velocity_y;
                double ke_before = 0.5*ball[i].mass*(ball[i].velocity_x*ball[i].velocity_x+ball[i].velocity_y*ball[i].velocity_y) + 0.5*ball[j].mass*(ball[j].velocity_x*ball[j].velocity_x+ball[j].velocity_y*ball[j].velocity_y); //Kinetic energy before the collision
                double v_rel_x = ball[i].velocity_x-ball[j].velocity_x;
                double v_rel_y = ball[i].velocity_y-ball[j].velocity_y;
                double total_mass = ball[i].mass+ball[j].mass;
                double v1x_new = (px_before - ball[j].mass*v_rel_x)/total_mass;
                double v1y_new = (py_before - ball[j].mass*v_rel_y)/total_mass;
                double v2x_new = (px_before + ball[i].mass*v_rel_x)/total_mass;
                double v2y_new = (py_before + ball[i].mass*v_rel_y)/total_mass;
                double m1 = ball[i].mass;
                double m2 = ball[j].mass;
                double ke_after = 0.5*m1*(v1x_new*v1x_new + v1y_new*v1y_new) + 0.5*m2*(v2x_new*v2x_new + v2y_new*v2y_new);
                ball[i].velocity_x = v1x_new;
                ball[i].velocity_y = v1y_new;
                ball[j].velocity_x = v2x_new;
                ball[j].velocity_y = v2y_new;
            }
        }
    }
}
