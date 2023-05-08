#include<unordered_map>
#include "ball.h"
#include <vector>
using namespace std;

class Detector{
    public:
        Ball *ball;
        int nBalls, nBalls_global;
        unordered_map <int, Ball*> index_map;
        vector<Ball*> status_list_x; // list of pointers to active ball for x axis
        vector<Ball*> status_list_y; // list of active ball for y axis

        vector<Ball*> x_coord;
        vector<Ball*> y_coord;

        vector<int> collision_candidates; // storing the result of S&P
        vector<vector<int>> counter_table;

        Detector(Ball *ball, int nBalls, int nBalls_global);
    
    public:
        void sweep_and_prune();
        bool detect_collision(int ball_i, int ball_j);
        void update_velocity(int time_step);
        void sort_axis();
        void collision_with_boundary(int nballs, int time_step,double x_left,double x_right,double y_top,double y_bottom);

};