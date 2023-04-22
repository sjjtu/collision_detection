#include "Detector.h"
#include <algorithm>
#include <stdio.h>
#include <iostream>

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

    // for(int i:x_coord) cout<<i<< " ";
    // cout << "\n";

    // for(int i:y_coord) cout<<i<< " ";
    // cout << "\n";

    this->status_list_x.clear();
    this->status_list_y.clear();

    this->counter_table = vector<vector<int>> (nBalls, vector<int>(nBalls));

    vector<int> to_remove_x;

    for(int i=0;i<x_coord.size();i++) {
        // X AXIS
        for(int j=0;j<status_list_x.size();++j){
            //cout << "comparing " << status_list_x[j] << " with " << x_coord[i] << endl;
            if(ball[x_coord[i]].position_x-ball[x_coord[j]].radius > ball[x_coord[j]].position_x+ball[x_coord[j]].radius) {
                //cout << " object is poped " << status_list_x[j] << endl;
                status_list_x.erase(status_list_x.begin() + j);
                j--;
            } else {
                cout << "reported pair " << status_list_x[j] << " with " << x_coord[i] << endl;
                cout << "    x coordinates: " << ball[x_coord[i]].position_x << " and " << ball[status_list_x[j]].position_x << endl;
                counter_table[i][j] += 1;
            }
        }
        //cout << "adding to status list " << x_coord[i] << endl;
        status_list_x.push_back(x_coord[i]);
    }

    for(int i=0;i<y_coord.size();i++) {
        // Y AXIS
        for(int j=0;j<status_list_y.size();++j){
            //cout << "comparing " << status_list_x[j] << " with " << x_coord[i] << endl;
            if(ball[y_coord[i]].position_y-ball[y_coord[j]].radius > ball[y_coord[j]].position_y+ball[y_coord[j]].radius) {
                //cout << " object is poped " << status_list_x[j] << endl;
                status_list_y.erase(status_list_y.begin() + j);
                j--;
            } else {
                cout << "reported pair " << status_list_y[j] << " with " << y_coord[i] << endl;
                cout << "    y coordinates: " << ball[y_coord[i]].position_y << " and " << ball[status_list_y[j]].position_y << endl;
                counter_table[i][j] += 1;
            }
        }
        //cout << "adding to status list " << x_coord[i] << endl;
        status_list_y.push_back(y_coord[i]);
    }
}

void Detector::update_velocity(int time_step) {
    for(int i=0;i<nBalls;i++) {
        for(int j=0;j<nBalls;j++){
            if (counter_table[i][j] == 2){
                printf("collision detected between %d and %d in time step %d \n", i, j, time_step);
                ball[i].velocity_x = 0;
                ball[i].velocity_y = 0;
                ball[j].velocity_x = 0;
                ball[j].velocity_y = 0;
            }
        }
    }
}