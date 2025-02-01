#include "Reachability.h"
#include <stdexcept>
#include <queue>
#include <set>
#include <sys/stat.h>

namespace ClassProject {

// Constructor: initializes BDD variables for states and inputs, and sets default transition and initial state.
Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)
    : ReachabilityInterface(stateSize, inputSize), stateSize(stateSize), inputSize(inputSize) {
    if (stateSize == 0) {
        throw std::runtime_error("State size cannot be zero.");
    }

    // Create BDD variables for current state bits and their next state counterparts.
    for (unsigned int i = 0; i < stateSize; ++i) {
        stateBits.push_back(Manager::createVar("s" + std::to_string(i)));
        nextStateBits.push_back(Manager::createVar("s'" + std::to_string(i)));
    }

    // Create BDD variables for input bits if any.
    for (unsigned int i = 0; i < inputSize; ++i) {
        inputBits.push_back(Manager::createVar("x" + std::to_string(i)));
    }

    // Set default transition functions to the identity (stateBits).
    Reachability::setTransitionFunctions(stateBits);

    // Initialize the reachable states with the default initial state (all false).
    const std::vector<bool> temp(stateSize, false);
    Reachability::setInitState(temp);
}

// Returns the vector of BDD state variables.
const std::vector<BDD_ID> &Reachability::getStates() const {
    return stateBits;
}

// Returns the vector of BDD input variables.
const std::vector<BDD_ID> &Reachability::getInputs() const {
    return inputBits;
}

// Sets the initial state based on the provided boolean vector.
void Reachability::setInitState(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("Initial state size mismatch with state size.");
    }

    // Start with the universal true BDD.
    initialStates = Manager::True();

    // Constrain the initialStates BDD for each state bit.
    for (int i = 0; i < stateSize; ++i) { // TODO: change to iterator based loops
        if (stateVector.at(i)) {
            initialStates = and2(initialStates, stateBits.at(i));
        } else {
            initialStates = and2(initialStates, neg(stateBits.at(i)));
        }
    }
}

// Sets transition functions ensuring there is exactly one function per state bit and that they exist.
void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions)
{
    if (transitionFunctions.size() != stateSize) {
        throw std::runtime_error("Transition function size mismatch with state size.");
    }

    // Validate each transition function using Manager's unique table size.
    for (const auto transition_id : transitionFunctions) {
        if (Manager::uniqueTableSize() < transition_id) {
            throw std::runtime_error("Transition function does not exist.");
        }
    }

    // Check that each transition function interacts with BDD nodes.
    for (unsigned int i = 0; i < stateSize; ++i) {
        std::set<BDD_ID> tempSet = {};
        Manager::findNodes(transitionFunctions.at(i), tempSet);
        if (tempSet.empty()) {
            throw std::runtime_error("Transition function does not exist.");
        }
    }

    this->transitionFunctions = transitionFunctions;
}

// Computes the image (next state set) from the current state set using the transition relation.
BDD_ID Reachability::computeImage(const BDD_ID &currentStates, const BDD_ID &transitionRelation) {
    // Combine current states with transition relation.
    BDD_ID temp = and2(currentStates, transitionRelation);

    // Perform existential quantification over state and input bits.
    for (const auto &state_bit : stateBits) {
        temp = or2(coFactorTrue(temp, state_bit), coFactorFalse(temp, state_bit));
    }
    for (const auto &input_bit : inputBits) {
        temp = or2(coFactorTrue(temp, input_bit), coFactorFalse(temp, input_bit));
    }

    // Compute image using xnor for next state bits (part 3 image computation: document section 8.1)
    BDD_ID img = xnor2(stateBits.at(0), nextStateBits.at(0));
    img = and2(img, temp);

    // Refine image for remaining state bits.
    for (int i = 1; i < stateSize; ++i) {
        temp = and2(temp, xnor2(stateBits.at(i), nextStateBits.at(i)));
        img = and2(img, temp);
    }

    // Finalize image computation by abstracting next state bits (document section 8.2)
    for (int i = stateSize - 1; i >= 0; --i) {
        img = or2(coFactorTrue(img, nextStateBits.at(i)), coFactorFalse(img, nextStateBits.at(i)));
    }

    return img;
}

// Constructs the overall transition relation (tau) from the individual transition functions.
BDD_ID Reachability::computeTransitionRelation() {
    if (nextStateBits.size() != transitionFunctions.size()) {
        throw std::runtime_error("Transition function size mismatch with state size.");
    }

    // Start with the relation for the first state bit.
    BDD_ID tau = xnor2(nextStateBits.at(0), transitionFunctions.at(0));
    BDD_ID tmp;

    // Combine the relations for remaining state bits.
    for(int i = 1; i < nextStateBits.size(); ++i) {
        tmp = xnor2(nextStateBits.at(i), transitionFunctions.at(i));
        tau = and2(tmp, tau);
    }

    return tau;
}

// Checks if a given state, described by a boolean vector, is reachable.
bool Reachability::isReachable(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }
    // Compute all reachable states.
    computeReachableStates();
    BDD_ID tmp = reachableStates;

    // Narrow down the BDD for each bit based on the provided stateVector.
    for (int i = 0; i < stateSize; ++i) {
        if (stateVector.at(i)) {
            tmp = coFactorTrue(tmp, stateBits.at(i));
        } else {
            tmp = coFactorFalse(tmp, stateBits.at(i));
        }
        // Early exit if tmp is a terminal node.
        if (tmp <= Manager::True()) {
            return (tmp == Manager::True());
        }
    }
    return false;
}

// Helper function: checks whether a given state is contained within a provided state set using BDD cofactors.
bool Reachability::isReachableInSet(const std::vector<bool> &stateVector, const BDD_ID &stateSet) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }
    BDD_ID tmp = stateSet;
    for (int i = 0; i < stateSize; ++i) {
        if (stateVector.at(i)) {
            tmp = coFactorTrue(tmp, stateBits.at(i));
        } else {
            tmp = coFactorFalse(tmp, stateBits.at(i));
        }
        if (tmp <= Manager::True()) {
            return tmp == Manager::True();
        }
    }
    return false;
}

// Iteratively computes the set of reachable states until a fixed point is reached.
void Reachability::computeReachableStates() {
    BDD_ID tau = computeTransitionRelation();
    BDD_ID Crit = initialStates;
    BDD_ID img, Cr;

    // Loop until no new reachable states are found.
    do {
        Cr = Crit;
        img = computeImage(Cr, tau);
        Crit = or2(img, Cr);
    } while (Cr != Crit);

    reachableStates = Cr;
}

int Reachability::stateDistance(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }
    int cnt = 0;
    BDD_ID tau = computeTransitionRelation();
    BDD_ID Crit = initialStates;
    BDD_ID img, Cr;
    
    // Loop until the target state is found or no new states are reached.
    do {
        Cr = Crit;
        img = computeImage(Cr, tau);
        Crit = or2(img, Cr);
        if (isReachableInSet(stateVector, Cr)) {
            return cnt;
        }
        cnt++;
    } while (Cr != Crit);
    return -1; // Return -1 if target state is unreachable.
}

} // namespace ClassProject
