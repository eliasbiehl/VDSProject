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

    reachableStates = stateBits[0];

    for (int i = 1; i < stateSize; ++i) {
        if (stateVector.at(i))
        {
            reachableStates = and2(reachableStates, stateBits.at(i));
        } else
        {
            reachableStates = and2(reachableStates, neg(stateBits.at(i)));
        }
    }
}

// Set transition functions
void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions)
{
    if (transitionFunctions.size() != stateSize) {
        throw std::runtime_error("Transition function size mismatch with state size.");
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

    BDD_ID img = and2(temp, initialStates);

    for (const auto &state_bit : stateBits)
    {
        // TODO aktuell noch state_bit statt state_bit'
        img = or2(coFactorTrue(img, state_bit), coFactorFalse(img, state_bit));
    }

    return img;
}


//TODO **************************************************************

    // Check if a state is reachable
    bool Reachability::isReachable(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }


}
    // Compute state distance using BFS-like approach
    int Reachability::stateDistance(const std::vector<bool> &stateVector)
{
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }

    return -1;
}






// Check if a fixed point is reached
bool Reachability::isFixedPoint(const BDD_ID &current, const BDD_ID &next) {
    return current == next;
}

// Compute reachable states
void Reachability::computeReachableStates() {
    BDD_ID transitionRelation = True();
    for (size_t i = 0; i < stateSize; ++i) {
        transitionRelation = and2(transitionRelation, xnor2(stateBits.at(i), transitionFunctions.at(i)));
    }

    BDD_ID currentReachable = reachableStates;

    do {
        BDD_ID nextReachable = or2(currentReachable, computeImage(currentReachable, transitionRelation));
        if (isFixedPoint(currentReachable, nextReachable)) {
            break;
        }
        currentReachable = nextReachable;
    } while (true);

    reachableStates = currentReachable;
}

// Check if a state is reachable
bool Reachability::isReachable(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }

    // Compute reachable states before checking
    computeReachableStates();

    BDD_ID stateBDD = True();
    for (size_t i = 0; i < stateSize; ++i) {
        stateBDD = and2(stateBDD, stateVector.at(i) ? stateBits.at(i) : neg(stateBits.at(i)));
    }

    return and2(reachableStates, stateBDD) != False();
}


} // namespace ClassProject
