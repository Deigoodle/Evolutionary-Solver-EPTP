#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <barrier>
#include <functional> // hash
#include <algorithm> // merge
#include <atomic>

#include "solver.h"

using namespace std;

struct graph{
    int n; // Number of Nodes
    vector<int> node_dwell_times; // Node Dweel (wait) Times
    vector<vector<int>> edge_travel_times; // Edge Travel Times
} graph_info ;

struct user{
    int available_time; // Available Time to Complete a Tour
    vector<int> node_valuations; // Node Valuations
    vector<vector<int>> edge_valuations; // Edge Valuations
};

struct solver_parameters{
    int max_iterations; // Max Iterations
    int population_size; // Population Size
    float crossover_rate; // Crossover Rate
    float mutation_rate; // Mutation Rate
    int patience; // Patience (Generations without improvement to wait early stopping)
    int n_threads; // Number of Threads (Slaves)
    int migration_rate; // Number of generations (iterations) between migrations
} solver_pars;

graph get_parameters(string type_1_instance);
vector<user> get_users(string type_2_instance, int n);
Solver::Solution solve_for_user(user user_info, 
                                graph graph_info, 
                                solver_parameters solver_pars,
                                barrier<>& pre_migration_barrier, 
                                barrier<>& post_migration_barrier);
void start_slave(vector<Solver> &slaves, 
                 int thread_id, 
                 user user_info, 
                 graph graph_info, 
                 solver_parameters solver_pars, 
                 unsigned int seed,
                 barrier<>& pre_migration_barrier,
                 barrier<>& post_migration_barrier,
                 atomic<bool>& early_stopping_flag);
void print_solution(Solver::Solution solution, 
                    int available_time);

unsigned int SEED = 64;

int main(int argc, char** argv){
    // check input parameters
    if (argc != 10) {
        cout << "Usage: " << argv[0] << " <type 1 instance> " << "<type 2 instance> " << "<max iterations> "  << "<population_size> " 
             << "<crossover rate> " << "<mutation rate> " << "<patience> " << "<n_threads> " << "<migration_rate>" << endl;
        return 1;
    }

    // get input parameters
    string type_1_instance = argv[1];
    string type_2_instance = argv[2];
    solver_pars.max_iterations = stoi(argv[3]);
    solver_pars.population_size = stoi(argv[4]);
    solver_pars.crossover_rate = stof(argv[5]);
    solver_pars.mutation_rate = stof(argv[6]);
    solver_pars.patience = stoi(argv[7]);
    solver_pars.n_threads = stoi(argv[8]);
    solver_pars.migration_rate = stoi(argv[9]);

    // get graph parameters
    graph_info = get_parameters(type_1_instance);

    // get info of users
    vector<user> users;
    users = get_users(type_2_instance, graph_info.n);

    // barrier
    barrier pre_migration_barrier(solver_pars.n_threads + 1);
    barrier post_migration_barrier(solver_pars.n_threads + 1);

    // solve for each user
    int n_users = users.size();
    vector<Solver::Solution> all_solutions(n_users);
    auto start = chrono::high_resolution_clock::now();

    for(int i=0; i < n_users; i++){
        Solver::Solution user_solution;

        auto user_start = chrono::high_resolution_clock::now();
        user_solution = solve_for_user(users[i], graph_info, solver_pars, ref(pre_migration_barrier), ref(post_migration_barrier));
        auto user_end = chrono::high_resolution_clock::now();

        chrono::duration<double, milli> user_time = user_end - user_start;
        user_solution.exec_time =  user_time.count();

        all_solutions[i] = user_solution;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> total_time = end - start;

    // print results
    cout << "Total Time: " << total_time.count() << "[ms]\n"<< "-----------------------------------"<<endl;
    for(int i=0; i < n_users; i++){
        cout << "User " << i + 1 << endl;
        print_solution(all_solutions[i], users[i].available_time);
        cout << "-----------------------------------"<<endl;
    }
}

// saves parameters of the type_1_instance in a struct
graph get_parameters(string type_1_instance){
    ifstream file(type_1_instance);
    graph graph_info;
    if (file.is_open()){
        // first line: n
        int n;
        file >> n;
        graph_info.n = n;

        // second line: dwell times
        graph_info.node_dwell_times.resize(n);
        for (int i = 0; i < n; i++){
            file >> graph_info.node_dwell_times[i];
        }

        // next n lines: travel times
        graph_info.edge_travel_times.resize(n, vector<int>(n));
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                file >> graph_info.edge_travel_times[i][j];
            }
        }

        file.close();
    }
    else {
        cout << "Unable to open file"<<endl;
    }
    
    return graph_info;
}

vector<user> get_users(string type_2_instance, int n){
    ifstream file(type_2_instance);
    int user_count;
    vector<user> users;

    if (file.is_open()){
        // get number of users
        file >> user_count;
        users = vector<user>(user_count);

        // get info of users
        for(int i=0; i < user_count; i++){
            // get parameters for this user
            file >> users[i].available_time;

            users[i].node_valuations = vector<int>(n);
            for(int j=0; j < n; j++){
                file >> users[i].node_valuations[j];
            }

            users[i].edge_valuations = vector<vector<int>>(n, vector<int>(n));
            for(int j=0; j < n; j++){
                for(int k=0; k < n; k++){
                    file >> users[i].edge_valuations[j][k];
                }
            }
        }
        file.close();
    }
    else {
        cout << "Unable to open file " << type_2_instance << endl;
        file.close();
        exit(1);
    }
    
    return users;
}

Solver::Solution solve_for_user(user user_info, 
                                graph graph_info, 
                                solver_parameters solver_pars, 
                                barrier<>& pre_migration_barrier, 
                                barrier<>& post_migration_barrier){
    // create threads
    vector<thread> threads;

    // create solvers
    vector<Solver> slaves(solver_pars.n_threads);

    // Early Stopping Flag
    atomic<bool> early_stopping_flag(false);

    // start threads
    unsigned int thread_seed = SEED;
    for(int t=0; t < solver_pars.n_threads; t++){
        // calculate seed iteratively
        thread_seed += t * 1000;

        // start
        threads.emplace_back(start_slave, // function to execute by the thread
                             ref(slaves), // vector of slaves (Solvers)
                             t,           // thread_id
                             user_info,   // user_info
                             graph_info,  // graph_info 
                             solver_pars, // solver_parameters
                             thread_seed, // hash of the previous seed
                             ref(pre_migration_barrier),   // reference to the pre migration barrier
                             ref(post_migration_barrier),  // reference to the post migration barrier
                             ref(early_stopping_flag));    // early stopping flag
    }

    // user best solution
    Solver::Solution best_solution(/*fitness=*/0);

    // patience counter
    int iterations_without_improvement = 0;

    // handle migrations
    int g = solver_pars.migration_rate - 1;
    for(g; g < solver_pars.max_iterations; g += solver_pars.migration_rate){
        bool improvement_found = false;
    
        // Arrive premigration barrier
        pre_migration_barrier.arrive_and_wait();
    
        // ~~~~~~~~~~~~~~~~~~~~~
        // ~ perform migration ~
        // ~~~~~~~~~~~~~~~~~~~~~
    
        // combine populations (sorted)
        vector<Solver::Solution> entire_population;
        int entire_population_size = 0;
        for(int t=0; t < solver_pars.n_threads; t++){
            // temporal vector
            vector<Solver::Solution> temp;
            temp.reserve(entire_population_size + slaves[t].population.size());
    
            // add thread population and sort it
            merge(entire_population.begin(), entire_population.end(), 
                  slaves[t].population.begin(), slaves[t].population.end(),
                  back_inserter(temp));
    
            // update entire population
            entire_population = move(temp);
    
            // calculate new size
            entire_population_size = entire_population.size();
        }
    
        // Check if entire_population is empty
        if (entire_population.empty()) {
            cout << "Error: entire_population is empty" << endl;
            break;
        }
    
        // search for solution improvement
        if(entire_population[0] > best_solution){
            // reset patience controllers
            improvement_found = true;
            iterations_without_improvement = 0;
    
            // update best_solution
            best_solution = entire_population[0];
            best_solution.iteration = g;
        }
    
        // if no improvement in this iteration add 1 
        if (!improvement_found){
            iterations_without_improvement += solver_pars.migration_rate;
        }
    
        // early stopping
        if ((iterations_without_improvement >= solver_pars.patience) || entire_population_size < solver_pars.n_threads * 2){
            early_stopping_flag.store(true);
            post_migration_barrier.arrive_and_wait();
            break;
        }
    
        // calculate interval size
        int interval_size = entire_population_size / solver_pars.n_threads;
        int remainder = entire_population_size % solver_pars.n_threads;
        
        // distribute population into intervals
        int start_idx = 0;
        for (int t = 0; t < solver_pars.n_threads; t++) {
            int end_idx = start_idx + interval_size + (t < remainder ? 1 : 0); // use remainder for the first threads
        
            // Ensure indices are within bounds
            if (start_idx < 0 || start_idx >= entire_population_size || end_idx < 0 || end_idx > entire_population_size) {
                cout << "Error: start_idx or end_idx out of bounds" << endl;
                break;
            }
        
            vector<Solver::Solution> interval_population(entire_population.begin() + start_idx, entire_population.begin() + end_idx);
        
            // Set population size before moving
            slaves[t].migrate_population(interval_population, interval_population.size());
        
            // update idx to next interval
            start_idx = end_idx;
        }
        
        // Arrive at post migration barrier
        post_migration_barrier.arrive_and_wait();
    }

    // join threads
    for (auto& th : threads) {
        th.join();
    }

    // save last iteration
    best_solution.last_iteration = g;

    return best_solution;
}

void start_slave(vector<Solver> &slaves, 
                 int thread_id, 
                 user user_info, 
                 graph graph_info, 
                 solver_parameters solver_pars, 
                 unsigned int seed,
                 barrier<>& pre_migration_barrier,
                 barrier<>& post_migration_barrier,
                 atomic<bool>& early_stopping_flag)
{
    slaves[thread_id] = Solver(graph_info.n, 
                               graph_info.node_dwell_times, 
                               graph_info.edge_travel_times, 
                               solver_pars.max_iterations, 
                               solver_pars.patience,
                               solver_pars.migration_rate,
                               seed);

    Solver::Solution solution;
    solution = slaves[thread_id].solve(user_info.node_valuations,
                                       user_info.edge_valuations, 
                                       user_info.available_time,
                                       solver_pars.crossover_rate,
                                       solver_pars.mutation_rate, 
                                       solver_pars.population_size,
                                       ref(pre_migration_barrier),
                                       ref(post_migration_barrier),
                                       ref(early_stopping_flag),
                                       /*orderX*/true);
}

void print_solution(Solver::Solution solution, int available_time)
{
    cout<< "Score: " << solution.fitness << endl;
    cout<< "Tour Time/Available Time: " << solution.tour_time << "/" << available_time << endl;
    cout<< "Solution: ";
    cout<< 1 << " ";
    for (unsigned long i = 0; i < solution.size; i++){
        cout << solution.chromosome[i] + 1 << " ";
    }
    if(!solution.feasible){
        cout << "(Infeasible)";
    }
    cout << endl;
    cout << "Execution Time: " << solution.exec_time << "[ms]"<<endl;
    cout << "Iteration: " << solution.iteration << "/" << solution.last_iteration << endl;
}


