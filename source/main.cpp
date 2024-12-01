#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

#include "solver.h"

using namespace std;

struct graph{
    int n;
    vector<int> node_dwell_times;
    vector<vector<int>> edge_travel_times;
} graph_info ;

struct user{
    int available_time;
    vector<int> node_valuations;
    vector<vector<int>> edge_valuations;
};

struct solver_parameters{
    int max_iterations;
    int population_size;
    float crossover_rate;
    float mutation_rate;
    int resets;
} solver_pars;

graph get_parameters(string type_1_instance);
vector<user> get_users(string type_2_instance, int n);
vector<Solver::Solution> solve_for_user(user user_info, graph graph_info, solver_parameters solver_pars, unsigned int seed);
void print_solution(Solver::Solution solution, int available_time);

int main(int argc, char** argv){
    // check input parameters
    if (argc != 8) {
        cout << "Usage: " << argv[0] << " <type 1 instance> " << "<type 2 instance> " << "<max iterations> " 
             << "<population_size> " << "<crossover rate> " << "<mutation rate> " << "<resets> " << endl;
        return 1;
    }

    // get input parameters
    string type_1_instance = argv[1];
    string type_2_instance = argv[2];
    solver_pars.max_iterations = stoi(argv[3]);
    solver_pars.population_size = stoi(argv[4]);
    solver_pars.crossover_rate = stof(argv[5]);
    solver_pars.mutation_rate = stof(argv[6]);
    solver_pars.resets = stoi(argv[7]);

    // get graph parameters
    graph_info = get_parameters(type_1_instance);

    // initialize seed
    unsigned seed = 64;

    // get info of users
    vector<user> users;
    users = get_users(type_2_instance, graph_info.n);

    // solve for each user
    int n_users = users.size();
    vector<vector<Solver::Solution>> all_solutions(n_users, vector<Solver::Solution>(solver_pars.resets));
    auto start = chrono::high_resolution_clock::now();
    for(int i=0; i < n_users; i++){
        vector<Solver::Solution> user_solutions(solver_pars.resets);
        user_solutions = solve_for_user(users[i], graph_info, solver_pars, seed);
        all_solutions[i] = user_solutions;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> total_time = end - start;

    // print results
    cout << "Total Time: " << total_time.count() << "[ms]\n"<< "-----------------------------------"<<endl;
    for(int i=0; i < n_users; i++){
        cout << "User " << i + 1 << endl;
        for(int j=0; j < solver_pars.resets; j++){
            if(solver_pars.resets > 1) cout<<endl;
            print_solution(all_solutions[i][j], users[i].available_time);
        }
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

vector<Solver::Solution> solve_for_user(user user_info, graph graph_info, solver_parameters solver_pars, unsigned int seed){
    // solution vector
    vector<Solver::Solution> user_solutions;

    // initialize solver
    Solver s = Solver(graph_info.n, 
                      graph_info.node_dwell_times, 
                      graph_info.edge_travel_times, 
                      solver_pars.max_iterations, 
                      seed);

    // solve "reset" times
    for(int i=0; i < solver_pars.resets; i++){
        Solver::Solution solution;
        solution = s.solve(user_info.node_valuations,
                                         user_info.edge_valuations, 
                                         user_info.available_time,
                                         solver_pars.crossover_rate,
                                         solver_pars.mutation_rate, 
                                         solver_pars.population_size,
                                         /*orderX*/true);
        user_solutions.push_back(solution);
    }

    return user_solutions;
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
    cout << "Execution Time: " << solution.exec_time.count() << "[ms]"<<endl;
}


