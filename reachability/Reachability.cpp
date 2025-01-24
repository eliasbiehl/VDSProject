#include "Reachability.h"
#include <stdexcept>
#include <queue>
#include <set>
#include <sys/stat.h>

namespace ClassProject {

// Constructor
Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)
    : ReachabilityInterface(stateSize, inputSize), stateSize(stateSize), inputSize(inputSize) {
    if (stateSize == 0) {
        throw std::runtime_error("State size cannot be zero.");
    }

    // Create state bits
    for (unsigned int i = 0; i < stateSize; ++i) {
        stateBits.push_back(Manager::createVar("s" + std::to_string(i)));
        nextStateBits.push_back(Manager::createVar("s'" + std::to_string(i)));
    }

    // Create input bits, if any
    for (unsigned int i = 0; i < inputSize; ++i) {
        inputBits.push_back(Manager::createVar("x" + std::to_string(i)));
    }

    // Initialize the transition function
    Reachability::setTransitionFunctions(stateBits);

    // Initialize the reachable states to the initial state (default all false)
    const std::vector<bool> temp(stateSize, false);
    Reachability::setInitState(temp);


}

// Get state bits
const std::vector<BDD_ID> &Reachability::getStates() const {
    return stateBits;
}

// Get input bits
const std::vector<BDD_ID> &Reachability::getInputs() const {
    return inputBits;
}


// Set initial state
void Reachability::setInitState(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("Initial state size mismatch with state size.");
    }

    initialStates = Manager::True();


    for (int i = 0; i < stateSize; ++i) { //TODO change to iterator range based for loops
        if (stateVector.at(i))
        {
            initialStates = and2(initialStates, stateBits.at(i));
        } else
        {
            initialStates = and2(initialStates, neg(stateBits.at(i)));
        }
    }
}

// Set transition functions
void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions)
{
    if (transitionFunctions.size() != stateSize) {
        throw std::runtime_error("Transition function size mismatch with state size.");
    }

    //TODO check whether transition function is part of BDD -> see test
    for (const auto transition_id : transitionFunctions)
    {
        if (Manager::uniqueTableSize() < transition_id)
        {
            throw std::runtime_error("Transition function does not exist.");
        }
    }

    for (unsigned int i = 0; i < stateSize; ++i) {
        std::set<BDD_ID> tempSet = {};
        Manager::findNodes(transitionFunctions.at(i), tempSet);
        if (tempSet.empty())
        {
            throw std::runtime_error("Transition function does not exist.");
        }
    }

    this->transitionFunctions = transitionFunctions;
}

// Compute the image of the current state
BDD_ID Reachability::computeImage(const BDD_ID &currentStates, const BDD_ID &transitionRelation) {
    BDD_ID temp = and2(currentStates, transitionRelation);

    for (const auto &state_bit : stateBits) {
        temp = or2(coFactorTrue(temp, state_bit), coFactorFalse(temp, state_bit));
    }

    // Part 3 image computation (8.1 in document)
    BDD_ID img = xnor2(stateBits.at(0), nextStateBits.at(0));
    img = and2(img, temp);

    for (int i = 1; i < stateSize; ++i)
    {
        temp = and2(temp, xnor2(stateBits.at(i), nextStateBits.at(i)));
        img = and2(img, temp);
    }

    // Part 3 image computation (8.2 in document)
    for (int i = stateSize - 1; i >= 0; --i)
    {
        img = or2(coFactorTrue(img, nextStateBits.at(i)), coFactorFalse(img, nextStateBits.at(i)));
    }


    return img;
}


BDD_ID Reachability::computeTransitionRelation()
{
    if (nextStateBits.size() != transitionFunctions.size())
    {
        throw std::runtime_error("Transition function size mismatch with state size.");
    }

    BDD_ID tau =  xnor2(nextStateBits.at(0), transitionFunctions.at(0)); //,  and2(not(nextStateBits.at(0)), not(transitionFunction.at(0))));
    BDD_ID tmp;

    for(int i = 1; i < nextStateBits.size(); ++i)
    {
        tmp = xnor2(nextStateBits.at(i), transitionFunctions.at(i));//or2(and2(nextStateBits.at(i), transitionFunction.at(i)),  and2(not(nextStateBits.at(i)), not(transitionFunction.at(i))));
        tau = and2(tmp,tau);
    }

    return tau;

}

// Check if a state is reachable
bool Reachability::isReachable(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }
    computeReachableStates();
    BDD_ID tmp = reachableStates;
    for (int i = 0; i < stateSize; ++i) {
        if (stateVector.at(i))
        {
            tmp = coFactorTrue(tmp, stateBits.at(i));
        } else
        {
            tmp = coFactorFalse(tmp, stateBits.at(i));
        }
        if (tmp <= Manager::True())
        {
            return (tmp == Manager::True());
        }
    }
    return false;
}

// Check if a state is reachable in a given ROBDD
bool Reachability::isReachableInSet(const std::vector<bool> &stateVector, const BDD_ID &stateSet) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }
    BDD_ID tmp = stateSet;
    for (int i = 0; i < stateSize; ++i) {
        if (stateVector.at(i))
        {
            tmp = coFactorTrue(tmp, stateBits.at(i));
        } else
        {
            tmp = coFactorFalse(tmp, stateBits.at(i));
        }
        if (tmp <= Manager::True())
        {
            return tmp;
        }
    }
    return false;
}

// Compute reachable states
void Reachability::computeReachableStates() {
    BDD_ID tau = computeTransitionRelation();
    BDD_ID Crit = initialStates;
    BDD_ID img, Cr;

    do
    {
        Cr = Crit;
        img = computeImage(Cr, tau);
        Crit = or2(img, Cr);
    } while (Cr != Crit);

    reachableStates = Cr;
}

// Compute state distance using BFS-like approach
int Reachability::stateDistance(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }
    int cnt = 0;
    BDD_ID tau = computeTransitionRelation();
    BDD_ID Crit = initialStates;
    BDD_ID img, Cr;
    if(isReachableInSet(stateVector, Crit))
    {
        return 0;
    }
    do
    {
        Cr = Crit;
        img = computeImage(Cr, tau);
        Crit = or2(img, Cr);
        cnt++;
        if (isReachableInSet(stateVector, Cr))
        {
            return cnt;
        }
    } while (Cr != Crit);
    return -1;
}

} // namespace ClassProject
