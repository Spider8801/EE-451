// C++ implementation of the above approach
#include <algorithm>
#include <bits/stdc++.h>
#include <limits.h>
#include <iterator> // for std::begin, std::end
#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <time.h>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

// Number of cities in TSP
#define V 48

// Initial population size for the algorithm
#define POP_SIZE 12

#define gen_threshold 300


int best_value = INT_MAX;

template <typename S>

// with_separator() function accepts
// two  arguments i.e., a vector and
// a separator string
void with_separator(const vector<S>& vec,
                    string sep = " ")
{
    // Iterating over all elements of vector
    for (auto elem : vec) {
        cout << elem << sep;
    }

    cout << endl;
}

// Template class to slice a vector
// from range X to Y
template <typename T>
vector<T> slicing(vector<T> const& v,
                  int X, int Y)
{

    // Begin and End iterator
    auto first = v.begin() + X;
    auto last = v.begin() + Y + 1;

    // Copy the element
    vector<T> vector(first, last);

    // Return the results
    return vector;
}

// Template class to print the element
// in vector v
template <typename T>
void printResult(vector<T> const& v)
{

    // Traverse the vector v
    for (auto i : v) {
        cout << i << ' ';
    }
    cout << '\n';
}

// Structure of a GNOME
// string defines the path traversed
// by the salesman while the fitness value
// of the path is stored in an integer
struct individual {
    vector<int> gnome;
    int fitness;
};

// Function to return a random number
// from start and end
int rand_num(int start, int end) {
    int r = end - start;
    int rnum = start + rand() % r;
    return rnum;
}

// Function to check if the character
// has already occurred in the string
bool repeat(string s, char ch) {
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == ch)
            return true;
    }
    return false;
}

// Function to return a mutated GNOME
// Mutated GNOME is a string
// with a random interchange
// of two genes to create variation in species
vector<int> mutatedGene(vector<int> gnome) {
    while (true) {
        int r = rand_num(1, V);
        int r1 = rand_num(1, V);
        if (r1 != r) {
            char temp = gnome[r];
            gnome[r] = gnome[r1];
            gnome[r1] = temp;
            break;
        }
    }
    return gnome;
}

// Function to return a valid GNOME string
// required to create the population
vector<int> create_gnome() {
    // TODO: Change characters to String Numbers (SHON)
    vector<int> old_gnome;

    for (int h = 0; h < V + 1; h++) {

        if (h == V) {
            old_gnome.push_back(0);
            break;
        }
        old_gnome.push_back(h);

    }

    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(old_gnome), std::end(old_gnome), rng);
    return old_gnome;
}

// Function to return the fitness value of a gnome.
// The fitness value is the path length
// of the path represented by the GNOME.
int cal_fitness(vector<int> gnome, int map[V][V]) {
    int f = 0;
    for (int i = 0; i < V; i++) {
        if (map[gnome[i]][gnome[i + 1]] == INT_MAX)
            return INT_MAX;
        f += map[gnome[i]][gnome[i + 1]];
    }


    if (f < best_value){
#pragma omp critical // Where did we declare best_value as private
        best_value = f;
    }
    return f;
}

// Function to return the updated value
// of the cooling element.
int cooldown(int temp) {
    // TODO: Evaluate need for Temp (SHON)
    return (99 * temp) / 100;
}

// Comparator for GNOME struct.
bool lessthan(struct individual t1,
              struct individual t2) {
    return t1.fitness < t2.fitness;
}

string get_string_vector(vector<int> arr) {
    stringstream ss;
    copy(arr.begin(), arr.end(), ostream_iterator<int>(ss, "->"));
    string s = ss.str();
    s = s.substr(0, s.length() - 1);

    return s;
}

// Utility function for TSP problem.
void TSPUtil(int map[V][V]) {

//#pragma omp parallel num_threads(3)
//    {
//        int id = omp_get_thread_num();
//        int data = id;
//        int total = omp_get_num_threads();
//        //printf("Greetings from process %d out of %d with Data %d\n", id, total, data);
//    }


    // Generation Number
    int gen = 1;

    vector<struct individual> population;
    struct individual temp;



    // Populating the GNOME pool.
    for (int i = 0; i < POP_SIZE; i++) {
        temp.gnome = create_gnome();
        temp.fitness = cal_fitness(temp.gnome, map);
        population.push_back(temp);
    }


//    cout << "\nInitial population: " << endl
//         << "GNOME    FITNESS VALUE\n";
//    for (int i = 0; i < POP_SIZE; i++)
//        cout << get_string_vector(population[i].gnome) << " "
//             << population[i].fitness << endl;
//    cout << "\n";

    int temperature = 10000;

    // Iteration to perform
    // population crossing and gene mutation.
    while (temperature > 10 && gen <= gen_threshold) {
        sort(population.begin(), population.end(), lessthan);
        vector<struct individual> new_population;




        // CROSSOVER
        int f=0;
        for(int i=0; i<POP_SIZE-1; i++)
        {

            int crossover_point = 1 + (rand() % (V - 1));
            //cout<<crossover_point<<" a";
            //cout<< population[2].gnome.size()<<" "<<f++<<" ";
            //with_separator(population[0].gnome, ", ");
            vector<int> first_half, newgenome, second_half;

            // Function Call
            first_half = slicing(population[i].gnome, 0, crossover_point);
            //cout<<first_half.size()<<" b";
            //string first_half = population[0].gnome.substr(0, crossover_point);
            for (int j = crossover_point+1; j < population[i+1].gnome.size()-1; j++) {
                    second_half.push_back(population[i+1].gnome[j]);
                            }
            //cout<<second_half.size()<<" c";
            newgenome.insert(newgenome.begin(), first_half.begin(), first_half.end());
            //cout<<newgenome.size();
            newgenome.insert(newgenome.end(), second_half.begin(), second_half.end());
            //cout<<newgenome.size();
            newgenome.push_back(0);
            //newgenome.push_back(0);
            //cout<<newgenome.size();
            population[i].gnome = newgenome;
            population[i].fitness = cal_fitness(population[i].gnome, map);

            //        //Add fittest offspring to population
        }


        // MUTATION
    #pragma omp parallel num_threads(4) default(none) shared(population, map, temperature, gen, new_population)
        {
            int id = omp_get_thread_num();
            //printf("%d\n", gen);
    #pragma omp for
            for (int i = 0; i < POP_SIZE; i++) {
                struct individual p1 = population[i];

                int tempp = 0;

                while (tempp < POP_SIZE * 50) {
                    tempp += 1;

                    //Do mutation under a random probability
                    //                if (rn.nextInt()%7 < 5) {
                    //                    demo.mutation();
                    //                }

                    vector<int> new_g = mutatedGene(p1.gnome);
                    struct individual new_gnome;
                    new_gnome.gnome = new_g;
                    new_gnome.fitness = cal_fitness(new_gnome.gnome, map);

                    if (new_gnome.fitness <= population[i].fitness) {
                        #pragma omp critical
                        new_population.push_back(new_gnome);
                        break;
                    } else {
                        // Accepting the rejected children at
                        // a possible probability above threshold.
                        float prob = pow(2.7,
                                         -1 * ((float) (new_gnome.fitness
                                                        - population[i].fitness)
                                               / temperature));
                        if (prob > 0.5) {
                        #pragma omp critical
                            new_population.push_back(new_gnome);
                            break;
                        }
                    }
                }
            }

        }


        temperature = cooldown(temperature);
        population = new_population;
//        cout << "\nGeneration " << gen << " \n";
//        cout << "\nCurrent temp: " << temperature << "\n";
//        cout << "GNOME    FITNESS VALUE\n";
//
//        for (int i = 0; i < POP_SIZE; i++)
//            cout << get_string_vector(population[i].gnome) << " "
//                 << population[i].fitness << endl;
        gen++;
    }
    cout << "\nBest Value " << best_value << " \n";
}

int main() {

    int distances[V][V];
    int x, y;
    ifstream in("att48_d.txt");

    if (!in) {
        cout << "Cannot open file.\n";
        return 0;
    }

    for (y = 0; y < V; y++) {
        for (x = 0; x < V; x++) {
            in >> distances[x][y];
        }
    }
    in.close();

    auto start = high_resolution_clock::now();


    TSPUtil(distances);

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<seconds>(stop - start);

    cout <<"Time of execution is: "<< duration.count() <<" sec"<< endl;
}