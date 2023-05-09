#include "BallSorter.h"
#include <algorithm>



void BallSorter::sort_balls(Ball *balls, int nballs) {
    std::sort(balls, balls+nballs, [=](Ball a, Ball b)->bool{
        return ((a).position_x-(a).radius) < ((b).position_x - (b).radius);
    });
}

