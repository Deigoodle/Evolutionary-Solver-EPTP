#include <tuple>
#include <vector>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;

class Solver {
public:
    // solution struct
    struct Solution {
        vector<int> chromosome;
        unsigned long size;
        int fitness;
        int tour_time;
        bool feasible;
    };

    //variables
    int max_iterations;
    const int starting_node = 0;
    unsigned long n;
    vector<int> node_dwell_times;
    vector<vector<int>> edge_travel_times;
    int population_size;
    vector<Solution> population;
    Solution best_solution;
    chrono::duration<double, milli> exec_time;

    // constructor, destructor
    Solver(int n, 
           vector<int> node_dwell_times, 
           vector<vector<int>> edge_travel_times, 
           int max_iterations);

    ~Solver();

    // methods
    vector<int> generate_chromosome();

    Solution generate_solution();

    vector<int> shuffle_chromosome(vector<int> chromosome);

    Solution encode_solution(Solution solution);

    Solution decode_solution(Solution solution);

    void print_solution(Solution solution, int available_time);

    tuple<Solution, Solution> onepoint_crossover(Solution parent1, Solution parent2);

    tuple<Solution, Solution> order_crossover(Solution parent1, Solution parent2);

    Solution mutate(Solution solution);

    Solution calculate_fitness(Solution solution,
                               vector<int> node_valuations,
                               vector<vector<int>> edge_valuations, 
                               int available_time);

    void initialize_population();

    int spin_roulette_wheel(int total_fitness);

    Solution solve(vector<int> node_valuations,
                   vector<vector<int>> edge_valuations, 
                   int available_time, 
                   float crossover_rate,
                   float mutation_rate, 
                   int population_size, 
                   bool orderX=true);    
};