#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <gtest/gtest.h>
#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : testing::Test {

    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(2);

    std::vector<BDD_ID> stateVars2 = fsm->getStates();
    std::vector<BDD_ID> transitionFunctions;

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);
};

TEST_F(ReachabilityTest, HowTo_Example) { /* NOLINT */

    transitionFunctions.push_back(fsm->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm->neg(s1)); // s1' = not(s1)
    fsm->setTransitionFunctions(transitionFunctions);

    fsm->setInitState({false,false});

    ASSERT_TRUE(fsm->isReachable({false, false}));
    ASSERT_FALSE(fsm->isReachable({false, true}));
    ASSERT_FALSE(fsm->isReachable({true, false}));
    ASSERT_TRUE(fsm->isReachable({true, true}));
}

TEST_F(ReachabilityTest, ConstructorRuntimeErrorTest) {
    // throws std::runtime_error if stateSize is zero
    EXPECT_THROW(std::make_unique<ClassProject::Reachability>(0, 0), std::runtime_error);
    EXPECT_THROW(std::make_unique<ClassProject::Reachability>(0, 1), std::runtime_error);
}

TEST_F(ReachabilityTest, isReachableRuntimeErrorTest) {
    // throws std::runtime_error if size does not match with number of state bits
    transitionFunctions.push_back(fsm->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm->neg(s1)); // s1' = not(s1)
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});

    // test for one missing state
    EXPECT_THROW(fsm->isReachable({false}), std::runtime_error);
    // test for one additional state
    EXPECT_THROW(fsm->isReachable({false, false, false}), std::runtime_error);
}

TEST_F(ReachabilityTest, stateDistanceRuntimeErrorTest) {
    // throws std::runtime_error if size does not match with number of state bits
    transitionFunctions.push_back(fsm->neg(s0));
    transitionFunctions.push_back(fsm->neg(s1));
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});
}

TEST_F(ReachabilityTest, TransistionRuntimeErrorTest) {
    // throws std::runtime_error if size does not match the number of state bits
    transitionFunctions.push_back(fsm->neg(s0));
    EXPECT_THROW(fsm->setTransitionFunctions(transitionFunctions), std::runtime_error);
    // test invalid BDD_ID
    transitionFunctions.push_back((BDD_ID) 999);
    EXPECT_THROW(fsm->setTransitionFunctions(transitionFunctions), std::runtime_error);
    transitionFunctions.pop_back();

    transitionFunctions.push_back(fsm->neg(s1));

    // test for one additional transition function
    transitionFunctions.push_back(fsm->neg(s1));
    EXPECT_THROW(fsm->setTransitionFunctions(transitionFunctions), std::runtime_error);
}

TEST_F(ReachabilityTest, setInitialStateRuntimeErrorTest) {
    // throws std::runtime_error if size does not match with number of state bits
    transitionFunctions.push_back(fsm->neg(s0));
    transitionFunctions.push_back(fsm->neg(s1));
    fsm->setTransitionFunctions(transitionFunctions);

    // test for one missing initial state
    EXPECT_THROW(fsm->setInitState({false}), std::runtime_error);
    // test for one additional initial state
    EXPECT_THROW(fsm->setInitState({false, false, false}), std::runtime_error);
}

TEST_F(ReachabilityTest, ConstructorIdentityTest) {
    // test if identity function is used as default transition function
    ASSERT_TRUE(fsm->isReachable({false, false}));
    ASSERT_FALSE(fsm->isReachable({false, true}));
    ASSERT_FALSE(fsm->isReachable({true, false}));
    ASSERT_FALSE(fsm->isReachable({true, true}));
}

TEST_F(ReachabilityTest, ConstructorDefaultParameterTest) {
    // test if correct number of state and input variables are created
    int stateSize = 3;
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(stateSize);
    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    EXPECT_EQ(stateVars.size(), stateSize);
    EXPECT_EQ(inputVars.size(), 0);
}

TEST_F(ReachabilityTest, ConstructorVarTest) {
    // test if correct number of state and input variables are created
    int stateSize = 13;
    int inputSize = 7;
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(stateSize,
                                                                                                            inputSize);
    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    EXPECT_EQ(stateVars.size(), stateSize);
    EXPECT_EQ(inputVars.size(), inputSize);
}

TEST_F(ReachabilityTest, isReachableTest) {
    // test if correct current_states are reachable
    transitionFunctions.push_back(fsm->neg(s0));
    transitionFunctions.push_back(fsm->neg(s1));
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});

    ASSERT_TRUE(fsm->isReachable({false, false}));
    ASSERT_FALSE(fsm->isReachable({false, true}));
    ASSERT_FALSE(fsm->isReachable({true, false}));
    ASSERT_TRUE(fsm->isReachable({true, true}));
}

TEST_F(ReachabilityTest, StateDistanceTest) {
    // test if computed distance between current_states matches
    transitionFunctions.push_back(fsm->neg(s1));
    transitionFunctions.push_back(s0);
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});

    EXPECT_EQ(fsm->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm->stateDistance({true, false}), 1);
    EXPECT_EQ(fsm->stateDistance({true, true}), 2);
    EXPECT_EQ(fsm->stateDistance({false, true}), 3);
}

TEST_F(ReachabilityTest, SingleInputTest) {
    // test if state machine works wih input variables
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(1, 1);
    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    std::vector<BDD_ID> transitionFunctions;
    BDD_ID s0 = stateVars.at(0);
    BDD_ID x0 = inputVars.at(0);

    transitionFunctions.push_back(fsm->and2(fsm->neg(s0), x0));
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false});

    EXPECT_TRUE(fsm->isReachable({false}));
    EXPECT_TRUE(fsm->isReachable({true}));
    fsm->setInitState({true});
    EXPECT_TRUE(fsm->isReachable({false}));
    EXPECT_TRUE(fsm->isReachable({true}));
}

TEST(Distance_Test, distanceExample) { /* NOLINT */
    std::unique_ptr<ClassProject::Reachability> distanceFSM = std::make_unique<ClassProject::Reachability>(2,1);
    std::vector<BDD_ID> stateVars6 = distanceFSM->getStates();
    std::vector<BDD_ID> transitionFunctions;

    auto s0 = stateVars6.at(0);
    auto s1 = stateVars6.at(1);
    auto inputs = distanceFSM->getInputs();
    auto i = inputs.at(0);

    auto nots1 = distanceFSM->neg(s1);
    auto nots0 = distanceFSM->neg(s0);
    auto noti = distanceFSM->neg(i);

    //s0' = not(s1)*not(s0)*not(i) + !s1*s0*!i + s1*!s0*!i
    auto s0trans = distanceFSM->or2(distanceFSM->or2(distanceFSM->and2(distanceFSM->and2(nots1, nots0), noti), distanceFSM->and2(distanceFSM->and2(nots1, s0), noti)), distanceFSM->and2(distanceFSM->and2(s1, nots0), noti));
    //s1' = !s1*!s0 + s1*!s0*i + s1s0i
    auto s1trans = distanceFSM->or2(distanceFSM->or2(distanceFSM->and2(nots1, nots0), distanceFSM->and2(distanceFSM->and2(s1, nots0), i)), distanceFSM->and2(distanceFSM->and2(s1, s0), i));

    transitionFunctions.push_back(s0trans);
    transitionFunctions.push_back(s1trans);
    //s1' = not(s3) and (s3 or (s3 nand (s0 and s1)))
    distanceFSM->setTransitionFunctions(transitionFunctions);
    /*
     * Reset state A {false, false}
     * B {false, true}
     * C {true, false}
     * D {true, true}
     * input i
     *
     * A, i = 1 -> B
     * A, i = 0 -> D
     * B, i = 1 -> B
     * B, i = 0 -> C
     * C, i = 1 -> A
     * C, i = 0 -> C
     * D, i = 1 -> B
     * D, i = 0 -> A
     */

    distanceFSM->setInitState({false,false});
    ASSERT_TRUE (distanceFSM->isReachable({true, false}));
    ASSERT_TRUE (distanceFSM->isReachable({false,  true}));
    ASSERT_TRUE (distanceFSM->isReachable({true, true}));
    ASSERT_TRUE (distanceFSM->isReachable({false, false}));

    ASSERT_EQ (distanceFSM->stateDistance({false, false}), 0);
    ASSERT_EQ (distanceFSM->stateDistance({false, true}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({true, true}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({true, false}), 2);

    distanceFSM->setInitState({false,true});
    ASSERT_TRUE (distanceFSM->isReachable({true, false}));
    ASSERT_TRUE (distanceFSM->isReachable({false,  true}));
    ASSERT_TRUE (distanceFSM->isReachable({true, true}));
    ASSERT_TRUE (distanceFSM->isReachable({false, false}));

    ASSERT_EQ (distanceFSM->stateDistance({false, false}), 2);
    ASSERT_EQ (distanceFSM->stateDistance({false, true}), 0);
    ASSERT_EQ (distanceFSM->stateDistance({true, true}), 3);
    ASSERT_EQ (distanceFSM->stateDistance({true, false}), 1);


    distanceFSM->setInitState({true,false});
    ASSERT_TRUE (distanceFSM->isReachable({true, false}));
    ASSERT_TRUE (distanceFSM->isReachable({false,  true}));
    ASSERT_TRUE (distanceFSM->isReachable({true, true}));
    ASSERT_TRUE (distanceFSM->isReachable({false, false}));

    ASSERT_EQ (distanceFSM->stateDistance({false, false}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({false, true}), 2);
    ASSERT_EQ (distanceFSM->stateDistance({true, true}), 2);
    ASSERT_EQ (distanceFSM->stateDistance({true, false}), 0);

    distanceFSM->setInitState({true,true});
    ASSERT_TRUE (distanceFSM->isReachable({true, false}));
    ASSERT_TRUE (distanceFSM->isReachable({false,  true}));
    ASSERT_TRUE (distanceFSM->isReachable({true, true}));
    ASSERT_TRUE (distanceFSM->isReachable({false, false}));

    ASSERT_EQ (distanceFSM->stateDistance({false, false}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({false, true}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({true, true}), 0);
    ASSERT_EQ (distanceFSM->stateDistance({true, false}), 2);

}

#endif
