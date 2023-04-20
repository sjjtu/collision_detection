#include<list>
#include<unordered_map>
using namespace std;

class Detector{
    public:
        list<Ball> status_list;
        list<Ball> collision_candidates; // storing the result of S&P
        unordered_map<double, Ball> map_x_coord; // key: x_coordinate, value: Ball 
        unordered_map<double, Ball> map_y_coord; // key: y_coordinate, value: Ball 
        Detector(Ball* ball, int nBalls);
    
    private:
        void sweep_and_prune();
        void update_velocity();
        void sort_axis();

};