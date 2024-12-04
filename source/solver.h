#include <tuple>
#include <vector>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
#include <barrier>
#include <atomic>

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
        double exec_time;
        int iteration; // iteration that this solution was found
        int last_iteration; // last iteration that the Solver executed

        // Default constructor
        Solution() : size(0), fitness(0), tour_time(0), feasible(false), exec_time(0), iteration(0), last_iteration(0) {}

        // (chromosome, size, fitness, tour_time, feasible) constructor
        Solution(vector<int> chromosome, unsigned long size, int fitness, int tour_time, bool feasible) : 
        chromosome(chromosome), size(size), fitness(fitness), tour_time(tour_time), feasible(feasible), exec_time(0), iteration(0), last_iteration(0) {}

        // Constructor to initialize fitness
        Solution(int fitness) : size(0), fitness(fitness), tour_time(0), feasible(false), exec_time(0), iteration(0), last_iteration(0) {}

        // Comparison operator compare fitness
        bool operator>(const Solution& other) const{
            return this->fitness > other.fitness;
        }
        bool operator<(const Solution& other) const {
        return this->fitness < other.fitness;
        }
    };

    // initialization parameters
    unsigned long n;
    vector<int> node_dwell_times; // list of node dwell times
    vector<vector<int>> edge_travel_times; // matrix of edge dwell times
    int max_iterations;
    int patience;
    int migration_rate;
    unsigned int seed;

    // extra variables
    mt19937 gen; // random generator
    const int starting_node = 0;
    int population_size;
    vector<Solution> population;
    Solution best_solution;

    // Default constructor
    Solver();

    // constructor, destructor
    Solver(int n, 
           vector<int> node_dwell_times, 
           vector<vector<int>> edge_travel_times, 
           int max_iterations,
           int patience,
           int migration_rate,
           unsigned int seed);

    ~Solver();

    // Copy assignment operator
    Solver& operator=(const Solver& other);

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
                   barrier<>& pre_migration_barrier,
                   barrier<>& post_migration_barrier,
                   atomic<bool>& early_stopping_flag,
                   bool orderX=true);    

    void migrate_population(vector<Solution> new_population, 
                            int new_population_size);
};