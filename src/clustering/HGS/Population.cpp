#include "../../../include/clustering/HGS/Population.h"
/**
 * @brief generate a random initial population, where each solution is in the form of 1 giant tour 
 * starting and ending at the depot. split this solution into multiple smaller tours (corresponding to each
 * rider). perform local search, evaluate penalty and check feasibility of solutions
 * 
 */

using namespace std;
void Population::generatePopulation()
{
    cout << __FUNCTION__ << " " << __LINE__ << endl;
	if (params.verbose) std::cout << "----- BUILDING INITIAL POPULATION" << std::endl;
	for (int i = 0; i < 4*params.ap.mu && (i == 0 || params.ap.timeLimit == 0 || (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC < params.ap.timeLimit) ; i++)
	{
		Individual randomIndiv(params); // chromosome
		split.generalSplit(randomIndiv, params.nbVehicles); // make clusters inside the chromosome
		localSearch.run(randomIndiv, params.penaltyCapacity, params.penaltyDuration); /// 
		addIndividual(randomIndiv, true);
		if (!randomIndiv.eval.isFeasible && params.ran() % 2 == 0)  // Repair half of the solutions in case of infeasibility
		{
			localSearch.run(randomIndiv, params.penaltyCapacity*10., params.penaltyDuration*10.);
			if (randomIndiv.eval.isFeasible) addIndividual(randomIndiv, false);
		}
	}
    cout << __FUNCTION__ << " " << __LINE__ << endl;
}
/**
 * @brief add an individual (a complete assignment of routes to all riders) to the population
 * 
 * @param indiv individual to be added
 * @param updateFeasible whether the new individual is feasible
 * @return true if new individual is feasible and its cost is within an acceptable limit
 * @return false otherwise
 */
bool Population::addIndividual(const Individual & indiv, bool updateFeasible)
{

	if (updateFeasible)
	{
		listFeasibilityLoad.push_back(indiv.eval.capacityExcess < MY_EPSILON);
		listFeasibilityDuration.push_back(indiv.eval.durationExcess < MY_EPSILON);
		listFeasibilityLoad.pop_front();
		listFeasibilityDuration.pop_front();
	}

	// Find the adequate subpopulation in relation to the individual feasibility
	SubPopulation & subpop = (indiv.eval.isFeasible) ? feasibleSubpop : infeasibleSubpop;

	// Create a copy of the individual and updade the proximity structures calculating inter-individual distances
	Individual * myIndividual = new Individual(indiv);
	for (Individual * myIndividual2 : subpop)
	{
		double myDistance = brokenPairsDistance(*myIndividual,*myIndividual2);
		myIndividual2->indivsPerProximity.insert({ myDistance, myIndividual });
		myIndividual->indivsPerProximity.insert({ myDistance, myIndividual2 });
	}

	// Identify the correct location in the subpopulation and insert the individual
	int place = (int)subpop.size();
	while (place > 0 && subpop[place - 1]->eval.penalizedCost > indiv.eval.penalizedCost - MY_EPSILON) place--;
	subpop.emplace(subpop.begin() + place, myIndividual);

	// Trigger a survivor selection if the maximimum subpopulation size is exceeded
	if ((int)subpop.size() > params.ap.mu + params.ap.lambda)
		while ((int)subpop.size() > params.ap.mu)
			removeWorstBiasedFitness(subpop);


	// Track best solution
	if (indiv.eval.isFeasible && indiv.eval.penalizedCost < bestSolutionRestart.eval.penalizedCost - MY_EPSILON)
	{	
		bestSolutionRestart = indiv; // Copy
		if (indiv.eval.penalizedCost < bestSolutionOverall.eval.penalizedCost - MY_EPSILON)
		{	
			bestSolutionOverall = indiv;
			searchProgress.push_back({ clock() - params.startTime , bestSolutionOverall.eval.penalizedCost });
		}
		return true;
	}
	else
		return false;
}
/**
 * @brief update the fitness function
 * 
 * @param pop subpopulation whose individuals' fitness is to be updated
 */
void Population::updateBiasedFitnesses(SubPopulation & pop)
{
	// Ranking the individuals based on their diversity contribution (decreasing order of distance)
	std::vector <std::pair <double, int> > ranking;
	for (int i = 0 ; i < (int)pop.size(); i++) 
		ranking.push_back({-averageBrokenPairsDistanceClosest(*pop[i],params.ap.nbClose),i});
	std::sort(ranking.begin(), ranking.end());

	// Updating the biased fitness values
	if (pop.size() == 1) 
		pop[0]->biasedFitness = 0;
	else
	{
		for (int i = 0; i < (int)pop.size(); i++)
		{
			double divRank = (double)i / (double)(pop.size() - 1); // Ranking from 0 to 1
			double fitRank = (double)ranking[i].second / (double)(pop.size() - 1);
			if ((int)pop.size() <= params.ap.nbElite) // Elite individuals cannot be smaller than population size
				pop[ranking[i].second]->biasedFitness = fitRank;
			else 
				pop[ranking[i].second]->biasedFitness = fitRank + (1.0 - (double)params.ap.nbElite / (double)pop.size()) * divRank;
		}
	}
}
/**
 * @brief remove individual with the worst fitness value
 * 
 * @param pop subpopulation in which to remove the worst fitness value
 */
void Population::removeWorstBiasedFitness(SubPopulation & pop)
{
	updateBiasedFitnesses(pop);
	if (pop.size() <= 1) throw std::string("Eliminating the best individual: this should not occur in HGS");

	Individual * worstIndividual = NULL;
	int worstIndividualPosition = -1;
	bool isWorstIndividualClone = false;
	double worstIndividualBiasedFitness = -1.e30;
	for (int i = 1; i < (int)pop.size(); i++)
	{
		bool isClone = (averageBrokenPairsDistanceClosest(*pop[i],1) < MY_EPSILON); // A distance equal to 0 indicates that a clone exists
		if ((isClone && !isWorstIndividualClone) || (isClone == isWorstIndividualClone && pop[i]->biasedFitness > worstIndividualBiasedFitness))
		{
			worstIndividualBiasedFitness = pop[i]->biasedFitness;
			isWorstIndividualClone = isClone;
			worstIndividualPosition = i;
			worstIndividual = pop[i];
		}
	}

	// Removing the individual from the population and freeing memory
	pop.erase(pop.begin() + worstIndividualPosition); 

	// Cleaning its distances from the other individuals in the population
	for (Individual * indiv2 : pop)
	{
		auto it = indiv2->indivsPerProximity.begin();
		while (it->second != worstIndividual) ++it;
		indiv2->indivsPerProximity.erase(it);
	}

	// Freeing memory
	delete worstIndividual; 
}
/**
 * @brief restart HGS by generating a new population
 * 
 */
void Population::restart()
{
	if (params.verbose) std::cout << "----- RESET: CREATING A NEW POPULATION -----" << std::endl;
	for (Individual * indiv : feasibleSubpop) delete indiv ;
	for (Individual * indiv : infeasibleSubpop) delete indiv;
	feasibleSubpop.clear();
	infeasibleSubpop.clear();
	bestSolutionRestart = Individual(params);
	generatePopulation();
}

/**
 * @brief ensure that all penalty weights are within bounds
 * 
 */
void Population::managePenalties()
{
	// Setting some bounds [0.1,100000] to the penalty values for safety
	double fractionFeasibleLoad = (double)std::count(listFeasibilityLoad.begin(), listFeasibilityLoad.end(), true) / (double)listFeasibilityLoad.size();
	if (fractionFeasibleLoad < params.ap.targetFeasible - 0.05 && params.penaltyCapacity < 100000.)
		params.penaltyCapacity = std::min<double>(params.penaltyCapacity * params.ap.penaltyIncrease, 100000.);
	else if (fractionFeasibleLoad > params.ap.targetFeasible + 0.05 && params.penaltyCapacity > 0.1)
		params.penaltyCapacity = std::max<double>(params.penaltyCapacity * params.ap.penaltyDecrease, 0.1);

	// Setting some bounds [0.1,100000] to the penalty values for safety
	double fractionFeasibleDuration = (double)std::count(listFeasibilityDuration.begin(), listFeasibilityDuration.end(), true) / (double)listFeasibilityDuration.size();
	if (fractionFeasibleDuration < params.ap.targetFeasible - 0.05 && params.penaltyDuration < 100000.)
		params.penaltyDuration = std::min<double>(params.penaltyDuration * params.ap.penaltyIncrease, 100000.);
	else if (fractionFeasibleDuration > params.ap.targetFeasible + 0.05 && params.penaltyDuration > 0.1)
		params.penaltyDuration = std::max<double>(params.penaltyDuration * params.ap.penaltyDecrease, 0.1);

	// Update the evaluations
	for (int i = 0; i < (int)infeasibleSubpop.size(); i++)
		infeasibleSubpop[i]->eval.penalizedCost = infeasibleSubpop[i]->eval.distance
		+ params.penaltyCapacity * infeasibleSubpop[i]->eval.capacityExcess
		+ params.penaltyDuration * infeasibleSubpop[i]->eval.durationExcess;

	// If needed, reorder the individuals in the infeasible subpopulation since the penalty values have changed (simple bubble sort for the sake of simplicity)
	for (int i = 0; i < (int)infeasibleSubpop.size(); i++)
	{
		for (int j = 0; j < (int)infeasibleSubpop.size() - i - 1; j++)
		{
			if (infeasibleSubpop[j]->eval.penalizedCost > infeasibleSubpop[j + 1]->eval.penalizedCost + MY_EPSILON)
			{
				Individual * indiv = infeasibleSubpop[j];
				infeasibleSubpop[j] = infeasibleSubpop[j + 1];
				infeasibleSubpop[j + 1] = indiv;
			}
		}
	}
}
/**
 * @brief implement tournament selection for parent selection
 * 
 * @return const Individual& parent selected from the population
 */
const Individual & Population::getBinaryTournament ()
{
	// Picking two individuals with uniform distribution over the union of the feasible and infeasible subpopulations
	std::uniform_int_distribution<> distr(0, feasibleSubpop.size() + infeasibleSubpop.size() - 1);
	int place1 = distr(params.ran);
	int place2 = distr(params.ran);
	Individual * indiv1 = (place1 >= (int)feasibleSubpop.size()) ? infeasibleSubpop[place1 - feasibleSubpop.size()] : feasibleSubpop[place1];
	Individual * indiv2 = (place2 >= (int)feasibleSubpop.size()) ? infeasibleSubpop[place2 - feasibleSubpop.size()] : feasibleSubpop[place2];
	
	// Keeping the best of the two in terms of biased fitness
	updateBiasedFitnesses(feasibleSubpop);
	updateBiasedFitnesses(infeasibleSubpop);
	if (indiv1->biasedFitness < indiv2->biasedFitness) return *indiv1 ;
	else return *indiv2 ;		
}
/**
 * @brief get the best feasible solution (solution with maximum fitness value)
 * 
 * @return const Individual* the best feasible solution
 */
const Individual * Population::getBestFeasible ()
{
	if (!feasibleSubpop.empty()) return feasibleSubpop[0] ;
	else return NULL ;
}
/**
 * @brief get the best infeasible solution (solution with maximum fitness value)
 * 
 * @return const Individual* the best infeasible solution
 */
const Individual * Population::getBestInfeasible ()
{
	if (!infeasibleSubpop.empty()) return infeasibleSubpop[0] ;
	else return NULL ;
}
/**
 * @brief get the overall best solution (solution with maximum fitness value)
 * 
 * @return const Individual* the overall best solution
 */
const Individual * Population::getBestFound()
{
	if (bestSolutionOverall.eval.penalizedCost < 1.e29) return &bestSolutionOverall;
	else return NULL;
}
/**
 * @brief print the current state of HGS implementation
 * 
 * @param nbIter number of iterations completed
 * @param nbIterNoImprovement number of iterations without any improvement in fitness value
 */
void Population::printState(int nbIter, int nbIterNoImprovement)
{
	if (params.verbose)
	{
		std::printf("It %6d %6d | T(s) %.2f", nbIter, nbIterNoImprovement, (double)(clock()-params.startTime)/(double)CLOCKS_PER_SEC);

		if (getBestFeasible() != NULL) std::printf(" | Feas %zu %.2f %.2f", feasibleSubpop.size(), getBestFeasible()->eval.penalizedCost, getAverageCost(feasibleSubpop));
		else std::printf(" | NO-FEASIBLE");

		if (getBestInfeasible() != NULL) std::printf(" | Inf %zu %.2f %.2f", infeasibleSubpop.size(), getBestInfeasible()->eval.penalizedCost, getAverageCost(infeasibleSubpop));
		else std::printf(" | NO-INFEASIBLE");

		std::printf(" | Div %.2f %.2f", getDiversity(feasibleSubpop), getDiversity(infeasibleSubpop));
		std::printf(" | Feas %.2f %.2f", (double)std::count(listFeasibilityLoad.begin(), listFeasibilityLoad.end(), true) / (double)listFeasibilityLoad.size(), (double)std::count(listFeasibilityDuration.begin(), listFeasibilityDuration.end(), true) / (double)listFeasibilityDuration.size());
		std::printf(" | Pen %.2f %.2f", params.penaltyCapacity, params.penaltyDuration);
		std::cout << std::endl;
	}
}
/**
 * @brief measure the proportion of arc similarities between two solutions
 * 
 * @param indiv1 solution 1
 * @param indiv2 solution 2
 * @return double proportion of arc similarities between two solutions
 */
double Population::brokenPairsDistance(const Individual & indiv1, const Individual & indiv2)
{
	int differences = 0;
	for (int j = 1; j <= params.nbClients; j++)
	{
		if (indiv1.successors[j] != indiv2.successors[j] && indiv1.successors[j] != indiv2.predecessors[j]) differences++;
		if (indiv1.predecessors[j] == 0 && indiv2.predecessors[j] != 0 && indiv2.successors[j] != 0) differences++;
	}
	return (double)differences / (double)params.nbClients;
}
/**
 * @brief  average broken-pairs distance  of an individual to its nClosest most similar solutions 
 * in the population gives the diversity contribution of a solution
 * 
 * @param indiv solution whose diversity contribution is evaluated
 * @param nbClosest number of closest similar solutions in the population
 * @return double average broken-pairs distance to its nClosest most similar solutions in the population
 */
double Population::averageBrokenPairsDistanceClosest(const Individual & indiv, int nbClosest)
{
	double result = 0.;
	int maxSize = std::min<int>(nbClosest, indiv.indivsPerProximity.size());
	auto it = indiv.indivsPerProximity.begin();
	for (int i = 0; i < maxSize; i++)
	{
		result += it->first;
		++it;
	}
	return result / (double)maxSize;
}
/**
 * @brief evaluate diversity: average of the average broken-pairs distance of an individual to 
 * its nClosest most similar solutions in the population
 * 
 * @param pop subpopulation for which this is to be evaluated
 * @return double diversity
 */
double Population::getDiversity(const SubPopulation & pop)
{
	double average = 0.;
	int size = std::min<int>(params.ap.mu, pop.size()); // Only monitoring the "mu" better solutions to avoid too much noise in the measurements
	for (int i = 0; i < size; i++) average += averageBrokenPairsDistanceClosest(*pop[i],size);
	if (size > 0) return average / (double)size;
	else return -1.0;
}
/**
 * @brief get average of penalized costs
 * 
 * @param pop subpopulation for which average is to be calculated
 * @return double average cost
 */
double Population::getAverageCost(const SubPopulation & pop)
{
	double average = 0.;
	int size = std::min<int>(params.ap.mu, pop.size()); // Only monitoring the "mu" better solutions to avoid too much noise in the measurements
	for (int i = 0; i < size; i++) average += pop[i]->eval.penalizedCost;
	if (size > 0) return average / (double)size;
	else return -1.0;
}
/**
 * @brief export Search algorithm Progress
 * 
 * @param fileName file location to which progress is to be updated
 * @param instanceName name of the file
 */
void Population::exportSearchProgress(std::string fileName, std::string instanceName)
{
	std::ofstream myfile(fileName);
	for (std::pair<clock_t, double> state : searchProgress)
		myfile << instanceName << ";" << params.ap.seed << ";" << state.second << ";" << (double)state.first / (double)CLOCKS_PER_SEC << std::endl;
}
/**
 * @brief export final routing output in .sol format
 * 
 * @param indiv solution that is to be written to the file
 * @param fileName name of the file where final routing output is to be written
 */
void Population::exportCVRPLibFormat(const Individual & indiv, std::string fileName)
{
	std::ofstream myfile(fileName);
	if (myfile.is_open())
	{
		for (int k = 0; k < (int)indiv.chromR.size(); k++)
		{
			if (!indiv.chromR[k].empty())
			{
				myfile << "Route #" << k + 1 << ":"; // Route IDs start at 1 in the file format
				for (int i : indiv.chromR[k]) myfile << " " << i;
				myfile << std::endl;
			}
		}
		myfile << "Cost " << indiv.eval.penalizedCost << std::endl;
	}
	else std::cout << "----- IMPOSSIBLE TO OPEN: " << fileName << std::endl;
}
/**
 * @brief Construct a new Population:: Population object
 * 
 * @param params problem-specific data including location coordinates and penalty weights
 * @param split output split algorithm
 * @param localSearch improved solution after Route Improvement and SWAP*
 */
Population::Population(Params & params, Split & split, LocalSearch & localSearch) : params(params), split(split), localSearch(localSearch), bestSolutionRestart(params), bestSolutionOverall(params)
{
	listFeasibilityLoad = std::list<bool>(params.ap.nbIterPenaltyManagement, true);
	listFeasibilityDuration = std::list<bool>(params.ap.nbIterPenaltyManagement, true);
}
/**
 * @brief Destroy the Population:: Population object
 * 
 */
Population::~Population()
{
	for (int i = 0; i < (int)feasibleSubpop.size(); i++) delete feasibleSubpop[i];
	for (int i = 0; i < (int)infeasibleSubpop.size(); i++) delete infeasibleSubpop[i];
}