#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"

namespace ClassProject {

// The Reachability class implements state reachability analysis using Binary Decision Diagrams (BDD).
// It extends ReachabilityInterface and provides methods to compute reachable states and distances.
class Reachability : public ReachabilityInterface {
private:
    unsigned int stateSize;
    unsigned int inputSize;
    
    std::vector<BDD_ID> stateBits;
    std::vector<BDD_ID> nextStateBits;
    std::vector<BDD_ID> inputBits;
    std::vector<BDD_ID> transitionFunctions;

    BDD_ID initialStates;
    BDD_ID reachableStates;

    // Helper function to compute the next state image based on the current state and transition relation.
    BDD_ID computeImage(const BDD_ID &currentStates, const BDD_ID &transitionRelation);
    // Checks if the fixed point in state computation has been reached.
    static bool isFixedPoint(const BDD_ID &current, const BDD_ID &next);
    // Returns a reference to some internal representation (not used in current implementation).
    const BDD_ID &getCR() const;
    // Helper to check if a specific state (given as a vector) belongs to a provided state set.
    bool isReachableInSet(const std::vector<bool> &stateVector, const BDD_ID &stateSet);
    // Computes the overall transition relation based on individual transition functions.
    BDD_ID computeTransitionRelation();

public:
    // Constructor: creates state and input bits, sets up default transition functions and initial state.
    Reachability(unsigned int stateSize, unsigned int inputSize = 0);

    // Destructor
    ~Reachability() override = default;

    // Returns the BDD IDs representing the state bits.
    const std::vector<BDD_ID> &getStates() const override;
    // Returns the BDD IDs representing the input bits.
    const std::vector<BDD_ID> &getInputs() const override;
    // Checks if the provided state (as a vector of booleans) is reachable.
    bool isReachable(const std::vector<bool> &stateVector) override;
    // Calculates the distance (in transitions) from the initial state to the specified state.
    int stateDistance(const std::vector<bool> &stateVector) override;
    // Sets the transition functions; ensures one per state variable.
    void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override;
    // Defines the initial state using a boolean vector (false means low, true means high).
    void setInitState(const std::vector<bool> &stateVector) override;

    // Computes and stores the set of all reachable states.
    void computeReachableStates();
};

} // namespace ClassProject

#endif