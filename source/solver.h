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
        vector<int> chromosome; // numerical representation of the solution
        unsigned long size; // size of chromosome
        int fitness; // score
        int tour_time; // tour time
        bool feasible;
        chrono::duration<double, milli> exec_time;
        int iteration; // iteration that this solution was found
        int last_iteration; // last iteration that the Solver executed
    };

    // initialization parameters
    unsigned long n;
    vector<int> node_dwell_times; // list of node dwell times
    vector<vector<int>> edge_travel_times; // matrix of edge dwell times
    int max_iterations;
    unsigned int seed;

    // extra variables
    mt19937 gen; // random generator
    const int starting_node = 0;
    int population_size;
    vector<Solution> population;
    Solution best_solution;

    // constructor, destructor
    Solver(int n, 
           vector<int> node_dwell_times, 
           vector<vector<int>> edge_travel_times, 
           int max_iterations,
           unsigned int seed);

    ~Solver();

    // methods
    void reset_seed();

    vector<int> generate_chromosome();

    Solution generate_solution();

    vector<int> shuffle_chromosome(vector<int> chromosome);

    Solution encode_solution(Solution solution);

    Solution decode_solution(Solution solution);

    //void print_solution(Solution solution, int available_time);

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