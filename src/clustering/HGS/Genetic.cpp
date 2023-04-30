#include "../../../include/clustering/HGS/Genetic.h"
/**
 * @brief initializes random population of individuals; implements parent selection, crossover, neighbourhood search.
 * Evaluates penalties and prints the updated current best solution regularly.
 *
 */

using namespace std;
#include <thread>

void GeneticParallel::run() {
    /* INITIAL POPULATION */
    population->generatePopulation();

    int nbIter;
    int nbIterNonProd = 1;
    if (params.verbose) std::cout << "----- STARTING GENETIC ALGORITHM" << std::endl;
    for (nbIter = 0; nbIterNonProd <= params.ap.nbIter && (params.ap.timeLimit == 0 || (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC < params.ap.timeLimit); nbIter++) {
        /* SELECTION AND CROSSOVER */

        //------------Select Parents ----------------------//////
        vector<const Individual*> parents;
        for (int i = 0; i < this->nThreads; i++) {
            parents.push_back(&population->getBinaryTournament());
            parents.push_back(&population->getBinaryTournament());
        }

        //------------Crossover and Split ----------------------//
        vector<thread> threads;
        for (int i = 0; i < this->nThreads; i++) {
            threads.push_back(thread(&GeneticParallel::crossoverOX, this,
                                     ref(*offsprings[i]), ref(*parents[2 * i]),
                                     ref(*parents[2 * i + 1]),i));
        }
        for (int i = 0; i < this->nThreads; i++) {
            threads[i].join();
        }
        threads.clear();


        // --------------Local Search ----------------------//
        for (int i = 0; i < this->nThreads; i++) {
            auto fn = [](LocalSearch& ls, Individual& ind, double penaltyCapacity, double penaltyDuration) {
                ls.run(ind, penaltyCapacity, penaltyDuration);
            };
            threads.push_back(thread(fn, ref(*localSearches[i]), ref(*offsprings[i]),
                                     params.penaltyCapacity, params.penaltyDuration));
        }
        for (int i = 0; i < this->nThreads; i++) {
            threads[i].join();
        }
        threads.clear();

        // --------------Add to Population ----------------------//
        vector<bool> isNewBests;
        for (int i = 0; i < this->nThreads; i++) {
            isNewBests.push_back(population->addIndividual(*offsprings[i], true));
        }

        // --------------Repair Infeasible ----------------------//
        vector<int> infeasible;
        for (int i = 0; i < this->nThreads; i++) {
            if (!offsprings[i]->eval.isFeasible && params.ran() % 2 == 0) infeasible.push_back(i);
        }
        for (int i = 0; i < infeasible.size(); i++) {
            auto fn = [](LocalSearch& ls, Individual& ind, double penaltyCapacity, double penaltyDuration) {
                ls.run(ind, penaltyCapacity, penaltyDuration);
            };
            threads.push_back(thread(fn,ref(*localSearches[i]), ref(*offsprings[i]),params.penaltyCapacity * 10., params.penaltyDuration * 10.));
        }
        for (int i = 0; i < infeasible.size(); i++) {
            threads[i].join();
        }
        threads.clear();
        
        // --------------Add to Population ----------------------//
        for (int i = 0; i < infeasible.size(); i++) {
            isNewBests[infeasible[i]] = (population->addIndividual(*offsprings[infeasible[i]], false) ||
                                         isNewBests[infeasible[i]]);
        }
        bool isNewBest = false;
        for (int i = 0; i < this->nThreads; i++) {
            isNewBest = isNewBest || isNewBests[i];
        }

        // bool isNewBest = population.addIndividual(offspring, true);
        // if (!offspring.eval.isFeasible && params.ran() % 2 == 0)  // Repair half of the solutions in case of infeasibility
        // {
        //     localSearch.run(offspring, params.penaltyCapacity * 10., params.penaltyDuration * 10.);
        //     if (offspring.eval.isFeasible) isNewBest = (population.addIndividual(offspring, false) || isNewBest);
        // }

        /* TRACKING THE NUMBER OF ITERATIONS SINCE LAST SOLUTION IMPROVEMENT */
        if (isNewBest)
            nbIterNonProd = 1;
        else
            nbIterNonProd++;

        /* DIVERSIFICATION, PENALTY MANAGEMENT AND TRACES */
        if (nbIter % params.ap.nbIterPenaltyManagement == 0) population->managePenalties();
        if (nbIter % params.ap.nbIterTraces == 0) population->printState(nbIter, nbIterNonProd);

        /* FOR TESTS INVOLVING SUCCESSIVE RUNS UNTIL A TIME LIMIT: WE RESET THE ALGORITHM/POPULATION EACH TIME maxIterNonProd IS ATTAINED*/
        if (params.ap.timeLimit != 0 && nbIterNonProd == params.ap.nbIter) {
            population->restart();
            nbIterNonProd = 1;
        }
    }
    if (params.verbose) std::cout << "----- GENETIC ALGORITHM FINISHED AFTER " << nbIter << " ITERATIONS. TIME SPENT: " << (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC << std::endl;
}

void Genetic::run() {
    /* INITIAL POPULATION */
    population.generatePopulation();

    int nbIter;
    int nbIterNonProd = 1;
    if (params.verbose) std::cout << "----- STARTING GENETIC ALGORITHM" << std::endl;
    for (nbIter = 0; nbIterNonProd <= params.ap.nbIter && (params.ap.timeLimit == 0 || (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC < params.ap.timeLimit); nbIter++) {
        /* SELECTION AND CROSSOVER */
        crossoverOX(offspring, population.getBinaryTournament(), population.getBinaryTournament());

        /* LOCAL SEARCH */
        localSearch.run(offspring, params.penaltyCapacity, params.penaltyDuration);
        bool isNewBest = population.addIndividual(offspring, true);
        if (!offspring.eval.isFeasible && params.ran() % 2 == 0)  // Repair half of the solutions in case of infeasibility
        {
            localSearch.run(offspring, params.penaltyCapacity * 10., params.penaltyDuration * 10.);
            if (offspring.eval.isFeasible) isNewBest = (population.addIndividual(offspring, false) || isNewBest);
        }

        /* TRACKING THE NUMBER OF ITERATIONS SINCE LAST SOLUTION IMPROVEMENT */
        if (isNewBest)
            nbIterNonProd = 1;
        else
            nbIterNonProd++;

        /* DIVERSIFICATION, PENALTY MANAGEMENT AND TRACES */
        if (nbIter % params.ap.nbIterPenaltyManagement == 0) population.managePenalties();
        if (nbIter % params.ap.nbIterTraces == 0) population.printState(nbIter, nbIterNonProd);

        /* FOR TESTS INVOLVING SUCCESSIVE RUNS UNTIL A TIME LIMIT: WE RESET THE ALGORITHM/POPULATION EACH TIME maxIterNonProd IS ATTAINED*/
        if (params.ap.timeLimit != 0 && nbIterNonProd == params.ap.nbIter) {
            population.restart();
            nbIterNonProd = 1;
        }
    }
    if (params.verbose) std::cout << "----- GENETIC ALGORITHM FINISHED AFTER " << nbIter << " ITERATIONS. TIME SPENT: " << (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC << std::endl;
}

/**
 * @brief implements ordered crossover of the two input parents
 *
 * @param result chromosome to receive crossover output
 * @param parent1 parent 1 that undergoes crossover
 * @param parent2 parent 2 that undergoes crossover
 */
void Genetic::crossoverOX(Individual& result, const Individual& parent1, const Individual& parent2) {
    // Frequency table to track the customers which have been already inserted
    if (params.nbClients == 1) {
        result = parent1;
        split.generalSplit(result, parent1.eval.nbRoutes);
        return;
    }

    std::vector<bool> freqClient = std::vector<bool>(params.nbClients + 1, false);

    // Picking the beginning and end of the crossover zone
    std::uniform_int_distribution<> distr(0, params.nbClients - 1);
    int start = distr(params.ran);
    int end = distr(params.ran);

    // Avoid that start and end coincide by accident
    while (end == start) end = distr(params.ran);
    // Copy from start to end
    int j = start;
    while (j % params.nbClients != (end + 1) % params.nbClients) {
        result.chromT[j % params.nbClients] = parent1.chromT[j % params.nbClients];
        freqClient[result.chromT[j % params.nbClients]] = true;
        j++;
    }

    // Fill the remaining elements in the order given by the second parent
    for (int i = 1; i <= params.nbClients; i++) {
        int temp = parent2.chromT[(end + i) % params.nbClients];
        if (freqClient[temp] == false) {
            result.chromT[j % params.nbClients] = temp;
            j++;
        }
    }

    // Complete the individual with the Split algorithm
    split.generalSplit(result, parent1.eval.nbRoutes);
}

void GeneticParallel::crossoverOX(Individual& result, const Individual& parent1, const Individual& parent2, int idx) {
    // Frequency table to track the customers which have been already inserted
    if (params.nbClients == 1) {
        result = parent1;
        splits[idx]->generalSplit(result, parent1.eval.nbRoutes);
        return;
    }

    std::vector<bool> freqClient = std::vector<bool>(params.nbClients + 1, false);

    // Picking the beginning and end of the crossover zone
    std::uniform_int_distribution<> distr(0, params.nbClients - 1);
    int start = distr(params.ran);
    int end = distr(params.ran);

    // Avoid that start and end coincide by accident
    while (end == start) end = distr(params.ran);
    // Copy from start to end
    int j = start;
    while (j % params.nbClients != (end + 1) % params.nbClients) {
        result.chromT[j % params.nbClients] = parent1.chromT[j % params.nbClients];
        freqClient[result.chromT[j % params.nbClients]] = true;
        j++;
    }

    // Fill the remaining elements in the order given by the second parent
    for (int i = 1; i <= params.nbClients; i++) {
        int temp = parent2.chromT[(end + i) % params.nbClients];
        if (freqClient[temp] == false) {
            result.chromT[j % params.nbClients] = temp;
            j++;
        }
    }

    // Complete the individual with the Split algorithm
    splits[idx]->generalSplit(result, parent1.eval.nbRoutes);
}

GeneticParallel::GeneticParallel(Params& params, int _nThreads) : nThreads(_nThreads), params(params){
    this->splits = vector<Split*>(nThreads);
    this->localSearches = vector<LocalSearch*>(nThreads);
    this->offsprings = vector<Individual*>(nThreads);
    for (int i = 0; i < nThreads; i++){
        this->splits[i] = new Split(params);
        this->localSearches[i] = new LocalSearch(params);
        this->offsprings[i] = new Individual(params);
    }
    this->population = new Population(params,*splits[0],*localSearches[0]);
    
}

/**
 * @brief Construct a new Genetic:: Genetic object
 *
 * @param params problem-specific data including location coordinates and penalty weights
 */
Genetic::Genetic(Params& params) : params(params),
                                   split(params),
                                   localSearch(params),
                                   population(params, this->split, this->localSearch),
                                   offspring(params) {}
