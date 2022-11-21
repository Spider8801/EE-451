// C++ implementation of the above approach
#include <bits/stdc++.h>
#include <limits.h>
#include <time.h>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace std::chrono;


// Number of cities in TSP
#define V 48

// Initial population size for the algorithm
#define POP_SIZE 10

#define gen_threshold 500

// Structure of a GNOME
// string defines the path traversed
// by the salesman while the fitness value
// of the path is stored in an integer
struct individual {
    string gnome;
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
string mutatedGene(string gnome) {
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
string create_gnome() {
    // TODO: Change characters to String Numbers (SHON)
    string gnome = "0";
    while (true) {
        if (gnome.size() == V) {
            gnome += gnome[0];
            break;
        }
        int temp = rand_num(1, V);
        if (!repeat(gnome, (char) (temp + 48)))
            gnome += (char) (temp + 48);
    }
    return gnome;
}

// Function to return the fitness value of a gnome.
// The fitness value is the path length
// of the path represented by the GNOME.
int cal_fitness(string gnome, int map[V][V]) {
    int f = 0;
    for (int i = 0; i < gnome.size() - 1; i++) {
        if (map[gnome[i] - 48][gnome[i + 1] - 48] == INT_MAX)
            return INT_MAX;
        f += map[gnome[i] - 48][gnome[i + 1] - 48];
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

// Utility function for TSP problem.
void TSPUtil(int map[V][V]) {
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

    cout << "\nInitial population: " << endl
         << "GNOME	 FITNESS VALUE\n";
    for (int i = 0; i < POP_SIZE; i++)
        cout << population[i].gnome << " "
             << population[i].fitness << endl;
    cout << "\n";

    int temperature = 10000;

    // Iteration to perform
    // population crossing and gene mutation.
    while (temperature > 10 && gen <= gen_threshold) {
        sort(population.begin(), population.end(), lessthan);
        vector<struct individual> new_population;


        // CROSSOVER
        int crossover_point = 1 + (rand() % (V - 1));
        string first_half = population[0].gnome.substr(0, crossover_point);
        string second_half = "";
        for (int i = 0; i < population[1].gnome.size(); i++) {
            if (first_half.find(population[1].gnome[i]) == std::string::npos) {
                second_half += population[1].gnome[i];
            }
        }
        string newgenome = first_half + second_half + "0";
        population[POP_SIZE].gnome = newgenome;
        population[POP_SIZE].fitness = cal_fitness(population[POP_SIZE].gnome, map);

        //        //Add fittest offspring to population

        // MUTATION
        for (int i = 0; i < POP_SIZE; i++) {
            struct individual p1 = population[i];

            int tempp = 0;

            while (tempp < POP_SIZE * 100) {
                tempp += 1;

                //Do mutation under a random probability
                //                if (rn.nextInt()%7 < 5) {
                //                    demo.mutation();
                //                }

                string new_g = mutatedGene(p1.gnome);
                struct individual new_gnome;
                new_gnome.gnome = new_g;
                new_gnome.fitness = cal_fitness(new_gnome.gnome, map);

                if (new_gnome.fitness <= population[i].fitness) {
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
                        new_population.push_back(new_gnome);
                        break;
                    }
                }
            }
        }

        temperature = cooldown(temperature);
        population = new_population;
        cout << "\nGeneration " << gen << " \n";
        cout << "\nCurrent temp: " << temperature << "\n";
        cout << "GNOME	 FITNESS VALUE\n";

        for (int i = 0; i < POP_SIZE; i++)
            cout << population[i].gnome << " "
                 << population[i].fitness << endl;
        gen++;
    }
}
int main() {
    int distances[V][V];
    int x, y;
    ifstream in("C:\\Users\\ASUS\\CLionProjects\\TSP-GA\\att48_d.txt");

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

    auto duration = duration_cast<microseconds>(stop - start);

    cout <<"Time of execution is: "<< duration.count() << endl;
}
