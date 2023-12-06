#include <iostream>
#include <fstream>
#include <vector>
#include "Solver.cpp"

using namespace std;

struct parameters {
    int n;
    vector<int> node_dwell_times;
    vector<vector<int>> edge_travel_times;
} p ;

parameters get_parameters(string type_1_instance);

int main(int argc, char** argv){
    // check input parameters
    if (argc != 8) {
        cout << "Usage: " << argv[0] << " <type 1 instance> " << "<type 2 instance> " << "<max iterations> " <<"<population_size> "<<"<crossover rate> "<<"<mutation rate> "<<"<resets> "<< endl;
        return 1;
    }

    // get input parameters
    string type_1_instance = argv[1];
    string type_2_instance = argv[2];
    int max_iterations = stoi(argv[3]);
    int population_size = stoi(argv[4]);
    float crossover_rate = stof(argv[5]);
    float mutation_rate = stof(argv[6]);
    float resets = stoi(argv[7]);

    // get instance parameters
    p = get_parameters(type_1_instance);

    // initialize solver
    Solver s = Solver(p.n, p.node_dwell_times, p.edge_travel_times, max_iterations);

    // initialize random seed to generate random solutions
    srand(time(0));

    // start solving
    vector<vector<Solver::Solution>> all_solutions;
    ifstream file(type_2_instance);
    int user_count;
    vector<int> available_times;
    if (file.is_open()){
        file >> user_count;

        for(int i=0; i < user_count; i++){ // solve for each user
            // get parameters for this user
            int available_time;
            file >> available_time;
            available_times.push_back(available_time); // just for printing purposes

            vector<int> node_valuations(p.n);
            for(int j=0; j < p.n; j++){
                file >> node_valuations[j];
            }

            vector<vector<int>> edge_valuations(p.n, vector<int>(p.n));
            for(int j=0; j < p.n; j++){
                for(int k=0; k < p.n; k++){
                    file >> edge_valuations[j][k];
                }
            }

            // solve` "reset" times
            vector<Solver::Solution> user_solutions;
            for(int j=0; j < resets; j++){
                user_solutions.push_back(s.solve(node_valuations,
                                                 edge_valuations, 
                                                 available_time,
                                                 crossover_rate,
                                                 mutation_rate, 
                                                 population_size,
                                                 /*orderX*/true));
            }
            all_solutions.push_back(user_solutions);
            user_solutions.clear();

            node_valuations.clear();
            edge_valuations.clear();
        }
    }
    else {
        cout << "Unable to open file " << type_2_instance << endl;
    }
    file.close();

    // print results
    cout << s.exec_time.count() << "[ms]\n"<< "-----------------------------------"<<endl;
    for(int i=0; i < user_count; i++){
        cout << "User " << i + 1 << endl;
        for(int j=0; j < resets; j++){
            s.print_solution(all_solutions[i][j], available_times[i]);
            cout<<endl;
        }
        /*
        int infeasible_count = 0;
        for(int j=0; j < resets; j++){
            if(!all_solutions[i][j].feasible){
                infeasible_count++;
            }
        }
        cout << " Infeasible: " << infeasible_count << endl;
        for(int k=0; k < resets; k++){
            cout << all_solutions[i][k].fitness<<endl;
        }*/
        cout << "-----------------------------------"<<endl;
    }
}

// saves parameters of the type_1_instance in a struct
parameters get_parameters(string type_1_instance){
    ifstream file(type_1_instance);
    parameters p;
    if (file.is_open()){
        // first line: n
        int n;
        file >> n;
        p.n = n;

        // second line: dwell times
        p.node_dwell_times.resize(n);
        for (int i = 0; i < n; i++){
            file >> p.node_dwell_times[i];
        }

        // next n lines: travel times
        p.edge_travel_times.resize(n, vector<int>(n));
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                file >> p.edge_travel_times[i][j];
            }
        }

        file.close();
    }
    else {
        cout << "Unable to open file"<<endl;
    }
    
    return p;
}
