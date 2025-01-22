#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"

namespace ClassProject {

class Reachability : public ReachabilityInterface {
private:
    unsigned int stateSize;
    unsigned int inputSize;
    
    std::vector<BDD_ID> stateBits;
    std::vector<BDD_ID> nextStateBits;
    std::vector<BDD_ID> inputBits;
    std::vector<BDD_ID> transitionFunctions;

    BDD_ID initialStates;
    BDD_ID reachableStates; // <--- Needs to be an vector holding boolean values

    BDD_ID Cr;

    // Helper methods for symbolic traversal
    BDD_ID computeImage(const BDD_ID &currentStates, const BDD_ID &transitionRelation);
    static bool isFixedPoint(const BDD_ID &current, const BDD_ID &next);
    const BDD_ID &getCR() const;
    bool isReachableInSet(const std::vector<bool> &stateVector, const BDD_ID &stateSet);
    BDD_ID computeTransitionRelation();

public:
    // Constructor
    Reachability(unsigned int stateSize, unsigned int inputSize = 0);

    // Destructor
    ~Reachability() override = default;

    // Implementations of ReachabilityInterface methods
    const std::vector<BDD_ID> &getStates() const override;
    const std::vector<BDD_ID> &getInputs() const override;
    bool isReachable(const std::vector<bool> &stateVector) override;
    int stateDistance(const std::vector<bool> &stateVector) override;
    void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override;
    void setInitState(const std::vector<bool> &stateVector) override;



    // Additional utility methods if needed
    void computeReachableStates();
};

}

#endif