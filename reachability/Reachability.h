#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"

namespace ClassProject {

class Reachability : public ReachabilityInterface {
private:
    unsigned int stateSize;
    unsigned int inputSize;
    std::vector<BDD_ID> stateBits;
    std::vector<BDD_ID> inputBits;
    std::vector<BDD_ID> transitionFunctions;
    BDD_ID initialState;
    BDD_ID reachableStates;

    // Helper methods for symbolic traversal
    BDD_ID computeImage(const BDD_ID &currentStates, const BDD_ID &transitionRelation);
    bool isFixedPoint(const BDD_ID &current, const BDD_ID &next);

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