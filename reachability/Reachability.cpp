#include "Reachability.h"
#include <stdexcept>
#include <queue>
#include <set>

namespace ClassProject {

// Constructor
Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)
    : ReachabilityInterface(stateSize, inputSize), stateSize(stateSize), inputSize(inputSize) {
    if (stateSize == 0) {
        throw std::runtime_error("State size cannot be zero.");
    }

    // Create state bits
    for (unsigned int i = 0; i < stateSize; ++i) {
        stateBits.push_back(createVar("s" + std::to_string(i)));
    }

    // Create input bits, if any
    for (unsigned int i = 0; i < inputSize; ++i) {
        inputBits.push_back(createVar("i" + std::to_string(i)));
    }

    // Initialize the reachable states to the initial state (default all false)
    initialState = False();
    for (const auto &bit : stateBits) {
        initialState = and2(initialState, neg(bit));
    }

    reachableStates = initialState;
}

// Get state bits
const std::vector<BDD_ID> &Reachability::getStates() const {
    return stateBits;
}

// Get input bits
const std::vector<BDD_ID> &Reachability::getInputs() const {
    return inputBits;
}

// Set transition functions
void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) {
    if (transitionFunctions.size() != stateSize) {
        throw std::runtime_error("Transition function size mismatch with state size.");
    }

    this->transitionFunctions = transitionFunctions;
}

// Set initial state
void Reachability::setInitState(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("Initial state size mismatch with state size.");
    }

    initialState = True();
    for (size_t i = 0; i < stateSize; ++i) {
        initialState = and2(initialState, stateVector[i] ? stateBits[i] : neg(stateBits[i]));
    }

    reachableStates = initialState;
}

// Compute the image of the current state
BDD_ID Reachability::computeImage(const BDD_ID &currentStates, const BDD_ID &transitionRelation) {
    BDD_ID temp = and2(currentStates, transitionRelation);

    for (const auto &bit : stateBits) {
        temp = or2(coFactorTrue(temp, bit), coFactorFalse(temp, bit));
    }

    return temp;
}

// Check if a fixed point is reached
bool Reachability::isFixedPoint(const BDD_ID &current, const BDD_ID &next) {
    return current == next;
}

// Compute reachable states
void Reachability::computeReachableStates() {
    BDD_ID transitionRelation = True();
    for (size_t i = 0; i < stateSize; ++i) {
        transitionRelation = and2(transitionRelation, 
                                  xor2(stateBits[i], transitionFunctions[i]));
    }

    BDD_ID currentReachable = initialState;
    BDD_ID nextReachable;

    do {
        nextReachable = or2(currentReachable, computeImage(currentReachable, transitionRelation));
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

    BDD_ID stateBDD = True();
    for (size_t i = 0; i < stateSize; ++i) {
        stateBDD = and2(stateBDD, stateVector[i] ? stateBits[i] : neg(stateBits[i]));
    }

    return and2(reachableStates, stateBDD) != False();
}

// Compute state distance using BFS-like approach
int Reachability::stateDistance(const std::vector<bool> &stateVector) {
    if (stateVector.size() != stateSize) {
        throw std::runtime_error("State vector size mismatch with state size.");
    }

    // Target state as a BDD
    BDD_ID targetState = True();
    for (size_t i = 0; i < stateSize; ++i) {
        targetState = and2(targetState, stateVector[i] ? stateBits[i] : neg(stateBits[i]));
    }

    // BFS initialization
    std::queue<std::pair<BDD_ID, int>> bfsQueue;
    std::set<BDD_ID> visited;

    bfsQueue.push({initialState, 0});
    visited.insert(initialState);

    while (!bfsQueue.empty()) {
        BDD_ID currentState = bfsQueue.front().first;
        int currentDistance = bfsQueue.front().second;
        bfsQueue.pop();

        if (and2(currentState, targetState) != False()) {
            return currentDistance;
        }

        BDD_ID nextStates = computeImage(currentState, reachableStates);
        for (const auto &bit : stateBits) {
            BDD_ID trueState = coFactorTrue(nextStates, bit);
            BDD_ID falseState = coFactorFalse(nextStates, bit);

            if (visited.find(trueState) == visited.end()) {
                bfsQueue.push({trueState, currentDistance + 1});
                visited.insert(trueState);
            }
            if (visited.find(falseState) == visited.end()) {
                bfsQueue.push({falseState, currentDistance + 1});
                visited.insert(falseState);
            }
        }
    }

    return -1; // Unreachable
}

} // namespace ClassProject
