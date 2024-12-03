#include "solver.h"

Solver::Solver(int n, vector<int> node_dwell_times, vector<vector<int>> edge_travel_times, int max_iterations, unsigned int seed) {
    // initialize variables
    this->max_iterations = max_iterations;
    this->n = n;
    this->node_dwell_times = node_dwell_times;
    this->edge_travel_times = edge_travel_times;
    this->seed = seed;

    // initialize random number generator with the fixed seed
    this->gen = mt19937(seed);

    // best solution
    this->best_solution = Solution{vector<int>(n-1), static_cast<unsigned long>(n-1), 0,0,true};
} 
    
Solver::~Solver() {
}

void Solver::reset_seed()
{
    this->gen.seed(this->seed);
}

// chromosome [1,2,3,...,n-1]
vector<int> Solver::generate_chromosome()
{ 
    vector<int> chromosome;
    for (unsigned long i = 1; i < this->n; i++){
        chromosome.push_back(i);
    }
    return chromosome;
}

// solution of random size (between 1 and n-1) 
Solver::Solution Solver::generate_solution()
{
    unsigned long chromosome_size;
    vector<int> chromosome = generate_chromosome();
    
    chromosome_size = (this->gen() % (this->n - 1)) + 1;
    chromosome = shuffle_chromosome(chromosome);
    chromosome.resize(chromosome_size);

    return Solution{chromosome, chromosome_size, 0, 0, true};
}

vector<int> Solver::shuffle_chromosome(vector<int> chromosome)
{
    shuffle(chromosome.begin(), chromosome.end(), this->gen);

    return chromosome;
}

// encode with reference list
Solver::Solution Solver::encode_solution(Solution solution)
{
    vector<int> reference_list(this->n);
    vector<int> encoded_chromosome(solution.size);

    // the true reference_list is [1,2,3,...,n-1]
    // this reference_list [-1,0,1,2,...,n-2]
    // so if we do reference_list[1] we get 0, which is the index of 1 in the true reference_list
    for(unsigned long i = 0; i < this->n; i++){
        reference_list[i] = i-1;
    }

    for(unsigned long i = 0; i < solution.size; i++){
        encoded_chromosome[i] = reference_list[solution.chromosome[i]];
        // update the indexes, subtract 1 to all the elements that after the one we used
        for(unsigned long j = solution.chromosome[i]; j < this->n; j++){
            if(reference_list[j] != -1){
                reference_list[j]--;
            }
        }
    }
    return Solution{encoded_chromosome, solution.size, solution.fitness,solution.tour_time,true};
}

/* this function was very slower than the previous one
Solver::Solution Solver::encode_solution(Solution solution){
    vector<int> reference_list = generate_chromosome();
    vector<int> encoded_chromosome(solution.size);

    for(int i = 0; i < solution.size; i++){
        auto it = find(reference_list.begin(), reference_list.end(), solution.chromosome[i]);
        if (it != reference_list.end()) {
            int index = distance(reference_list.begin(), it);
            encoded_chromosome[i] = index;
            reference_list.erase(it);
        }
    }
    return Solution{encoded_chromosome, solution.size, solution.fitness};
}*/

Solver::Solution Solver::decode_solution(Solution solution)
{
    vector<int> reference_list = generate_chromosome();
    vector<int> decoded_chromosome(solution.size);

    for(unsigned long i = 0; i < solution.size; i++){
        decoded_chromosome[i] = reference_list[solution.chromosome[i]];
        reference_list.erase(reference_list.begin() + solution.chromosome[i]);
    }

    return Solution{decoded_chromosome, solution.size, solution.fitness,solution.tour_time,true};
}

// 1 point crossover, the crossover point is randomly selected in the shortest chromosome
tuple<Solver::Solution, Solver::Solution> Solver::onepoint_crossover(Solution parent1, 
                                                                     Solution parent2)
{
    // check shortest chromosome
    int shortest_chromosome_size = min(parent1.size, parent2.size);
    // select crossover point
    int crossover_point;
    if(shortest_chromosome_size == 1){
        crossover_point = 1;
    }
    else{
        crossover_point = this->gen() % (shortest_chromosome_size - 1) + 1;// the -1 and +1 are to avoid the starting node
    }
    // create children
    vector<int> child1;
    vector<int> child2;
    // fill children till crossover point
    for (int i = 0; i < crossover_point; i++){
        child1.push_back(parent1.chromosome[i]);
        child2.push_back(parent2.chromosome[i]);
    }
    // fill children after crossover point
    for (unsigned long i = crossover_point; i < this->n-1; i++){
        if(i < parent2.size){
            child1.push_back(parent2.chromosome[i]);
        }
        if(i < parent1.size){
            child2.push_back(parent1.chromosome[i]);
        }
    }
    return make_tuple(Solution{child1, child1.size(),0,0,true}, Solution{child2, child2.size(),0,0,true});
}

tuple<Solver::Solution, Solver::Solution> Solver::order_crossover(Solution parent1, 
                                                                  Solution parent2)
{
    if(parent1.size == 1 && parent2.size == 1){
        return make_tuple(parent1, parent2);
    }
    // check shortest chromosome
    int shortest_chromosome_size = min(parent1.size, parent2.size);

    // select two crossover point from 0 to shortest_chromosome_size-1
    unsigned long a,b;
    if(shortest_chromosome_size == 1){
        a = 0;
        b = 0;
    }
    else{
        a = this->gen() % (shortest_chromosome_size - 1);
        b = this->gen() % (shortest_chromosome_size - 1);
    }
    // make sure a is equal or smaller than b
    if(a > b){
        int temp = a;
        a = b;
        b = temp;
    }
    // create children
    vector<int> child1(parent2.size);
    vector<int> child2(parent1.size);

    // Copy subsegment from parent1 to child1 and from parent2 to child2
    for (unsigned long i = a; i <= b; i++){
        if(i < parent1.size && i < child1.size()){
            child1[i] = parent1.chromosome[i];
        }
        if(i < parent2.size && i < child2.size()){
            child2[i] = parent2.chromosome[i];
        }
    }

    // Fill the remaining positions in child1 from parent2
    unsigned long count1,count2;// count1 iterates over the child and count2 over the parent
    // remember that the size of child1 is parent2.size and viceversa
    count1 = count2 = (b + 1) % parent2.size;
    while(count1 !=a){
        if(find(child1.begin(), child1.end(), parent2.chromosome[count2]) == child1.end()){
            child1[count1] = parent2.chromosome[count2];
            count1 = (count1 + 1) % parent2.size;
        }
        count2 = (count2 + 1 )% parent2.size;
    }

    // Fill the remaining positions in child2 from parent1
    count1 = count2 = (b + 1) % parent1.size;
    while(count1 != a){
        if(find(child2.begin(), child2.end(), parent1.chromosome[count2]) == child2.end()){
            child2[count1] = parent1.chromosome[count2];
            count1 = (count1 + 1) % parent1.size;
        }
        count2 = (count2 + 1 )% parent1.size;
    }

    return make_tuple(Solution{child1, parent2.size,0,0,true}, Solution{child2, parent1.size,0,0,true});
}

Solver::Solution Solver::mutate(Solution solution)
{
    int random_node = this->gen() % (this->n-1) + 1; // random number between 1 and n-1
    int random_position;
    if (solution.size > 1) {
        random_position = this->gen() % solution.size; // random number between 0 and solution.size-1
    } else {
        random_position = 0; // if solution.size is 1, random_position should be 0
    }
    // if random_node is in solution swap it with the node in random_position
    if(find(solution.chromosome.begin(), solution.chromosome.end(), random_node) != solution.chromosome.end()){
        int index = find(solution.chromosome.begin(), solution.chromosome.end(), random_node) - solution.chromosome.begin();
        int temp = solution.chromosome[index];
        solution.chromosome[index] = solution.chromosome[random_position];
        solution.chromosome[random_position] = temp;
    }
    else{ // if random_node is not in solution, add it in random_position
        solution.chromosome[random_position] = random_node;
    }
    for (unsigned long i=0;i < solution.size;i++){
        if(solution.chromosome[i]==0){
            cout<<"solution has 0"<<endl;
        }
    }
    return solution;
}


Solver::Solution Solver::calculate_fitness(Solution solution, 
                                           vector<int> node_valuations, 
                                           vector<vector<int>> edge_valuations, 
                                           int available_time)
{
    int fitness = node_valuations[this->starting_node];
    int current_time = this->node_dwell_times[this->starting_node];
    bool feasible = true;
    float distance_penalty_rate = 0.9;

    // add edges that connect with the starting node
    int first_edge_time = this->edge_travel_times[this->starting_node][solution.chromosome[0]];
    if(first_edge_time>0){ // there is a route between starting_node and first node in the chromosome
        current_time += first_edge_time;
        fitness += edge_valuations[this->starting_node][solution.chromosome[0]];
    }
    else{ // penalty
        fitness *= distance_penalty_rate;
        feasible = false;
    }
    current_time += this->node_dwell_times[solution.chromosome[0]];
    fitness += node_valuations[solution.chromosome[0]];

    int last_edge_time = this->edge_travel_times[solution.chromosome[solution.size-1]][this->starting_node];
    if(last_edge_time>0){ // there is a route between last node in the chromosome and starting_node
        current_time += last_edge_time;
        fitness += edge_valuations[solution.chromosome[solution.size-1]][this->starting_node];
    }
    else{ // penalty
        fitness *= distance_penalty_rate;
        feasible = false;
    }

    // add the rest of the edges and nodes
    if(solution.size > 1){
        for(unsigned long i=1; i < solution.size; i++){
            int edge_time = this->edge_travel_times[solution.chromosome[i-1]][solution.chromosome[i]];
            if(edge_time>0){ // there is a route between current node and previous node
                current_time += edge_time;
                fitness += edge_valuations[solution.chromosome[i-1]][solution.chromosome[i]];
            }
            else{ // penalty
                fitness *= distance_penalty_rate;
                feasible = false;
            }
            fitness += node_valuations[solution.chromosome[i]];
            current_time += this->node_dwell_times[solution.chromosome[i]];
        }
    }

    if(current_time > available_time){ // penalty
        float time_penalty_rate = static_cast<float>(available_time) / pow(current_time,2) ;
        fitness *= time_penalty_rate;
        feasible = false;
    }
    
    return Solution{solution.chromosome, solution.size, fitness, current_time, feasible};
}

void Solver::initialize_population(){
    this->population.clear();
    for(int i = 0; i < this->population_size; i++){
        Solution solution = this->generate_solution();
        this->population.push_back(solution);
    }
}

int Solver::spin_roulette_wheel(int total_fitness){
    int random_fitness = this->gen() % total_fitness; // random number between 0 and total_fitness-1
    int selected_index = -1; 
    int sum_fitness = 0;
    for(int i = 0; i < this->population_size; i++){
        sum_fitness += this->population[i].fitness;
        if(sum_fitness >= random_fitness){
            selected_index = i;
            break;
        }
    }
    return selected_index;
}

Solver::Solution Solver::solve(vector<int>node_valuations, 
                               vector<vector<int>> edge_valuations, 
                               int available_time, 
                               float crossover_rate, 
                               float mutation_rate, 
                               int population_size, 
                               bool orderX)
{
    auto start = chrono::high_resolution_clock::now();

    this->population_size = population_size;
    initialize_population();
    this->best_solution = this->population[0];

    int i;
    for(i=0; i < this->max_iterations;i++){

        // calculate fitness for each solution
        int total_fitness = 0;
        for(int x = 0; x < this->population_size; x++){
            this->population[x] = calculate_fitness(this->population[x], node_valuations, edge_valuations, available_time);
            if(this->population[x].fitness > this->best_solution.fitness){
                this->best_solution = this->population[x];
                this->best_solution.iteration = i;
                //cout<<"new best solution: "<< this->best_solution.fitness <<endl;
            }
            total_fitness += this->population[x].fitness;
        }
        //cout << "Iteration: " << i << endl;
        //cout << "Initial population size: "<< this->population_size << endl;

        // check if the population is only 2 and they are the same
        if(this->population_size == 2 && this->population[0].chromosome == this->population[1].chromosome){
            break;
        }

        // selection phase (50% of the population)
        int selected_population_size = max(this->population_size / 2, 2); //there is at least 2 selected
        vector<Solution> selected_population;
        for(int j = 0; j < selected_population_size; j++){
            int selected_index = spin_roulette_wheel(total_fitness);
            selected_population.push_back(this->population[selected_index]);
        }
        //cout << "selected population size: "<<selected_population_size<< endl;

        // crossover phase
        int offspring_size = 0;
        vector<Solution> offspring;
        for (int k = 1; k < selected_population_size; k += 2) {
            // generate random number between 0 and 1
            double random_number = generate_canonical<double, 10>(this->gen);
            
            if (random_number < crossover_rate) {
                // get parents
                Solution parent1,parent2;
                parent1 = selected_population[k-1];
                parent2 = selected_population[k];

                // encode parents
                if(!orderX){
                    parent1 = encode_solution(parent1);
                    parent2 = encode_solution(parent2);
                }

                // perform crossover
                Solution child1,child2;
                if(orderX){ // order crossover
                    tie(child1, child2) = order_crossover(parent1, parent2);
                }
                else {// 1 point with reference list crossover
                    tie(child1, child2) = onepoint_crossover(parent1, parent2);
                    // decode children
                    child1 = decode_solution(child1);
                    child2 = decode_solution(child2);
                }

                // add children to population
                offspring.push_back(child1);
                offspring.push_back(child2);
                offspring_size += 2;
            }
        }
        //cout << "offspring generated: " << offspring_size << endl;
        // update population
        this->population.clear();
        this->population_size = offspring_size + selected_population_size;
        this->population.insert(this->population.end(), selected_population.begin(), selected_population.end());
        this->population.insert(this->population.end(), offspring.begin(), offspring.end());

        // mutation phase
        for(int l = 0; l < this->population_size; l++){
            // generate random number between 0 and 1
            double random_number = generate_canonical<double, 10>(this->gen);;
            if (random_number < mutation_rate) {
                this->population[l] = mutate(this->population[l]);
            }
        }
        //cout << "new population size: " << this->population_size << endl;
        //cout <<"---------------------------------------------"<<endl;
    }
    // calculate execution times
    auto end = chrono::high_resolution_clock::now();
    this->best_solution.exec_time = end - start;

    // save last iteration
    this->best_solution.last_iteration = i;

    return this->best_solution;
}