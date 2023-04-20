#include "Detector.h"
#include <algorithm>

Detector::Detector(Ball* ball, int nBalls){
    for(int i=0;i<nBalls;i++){
        this->map_x_coord[ball[i].position_x-ball[i].radius] = ball[i]; // beginning point
        this->map_x_coord[ball[i].position_x+ball[i].radius] = ball[i]; // end point

        this->map_y_coord[ball[i].position_y-ball[i].radius] = ball[i]; // beginning point
        this->map_y_coord[ball[i].position_y-ball[i].radius] = ball[i]; // end point
    }
}

void Detector::sort_axis(){
    bool my_comp (int i,int j) { return (i<j); }
    sort(map_x_coord.first(), map_x_coord.end(), my_comp);
}

void Detector::update_velocity() {
    
}