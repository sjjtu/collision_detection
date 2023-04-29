#include<unordered_map>
#include "ball.h"
#include <vector>
using namespace std;

class Detector{
    public:
        Ball *ball;
        int nBalls;
        vector<int> status_list_x; // list of active ball for x axis
        vector<int> status_list_y; // list of active ball for y axis

        vector<int> x_coord;
        vector<int> y_coord;

        vector<int> collision_candidates; // storing the result of S&P
        vector<vector<int>> counter_table;

        Detector(Ball *ball, int nBalls);
    
    public:
        void sweep_and_prune();
        bool detect_collision(int ball_i, int ball_j);
        void update_velocity(int time_step);
        void sort_axis();
        void collision_with_boundary(int nballs, int time_step,double x_left,double x_right,double y_top,double y_bottom);

};