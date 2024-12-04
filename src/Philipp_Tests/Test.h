//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <../gtest/googletest-src/googletest/include/gtest/gtest.h>
#include "../Manager.h"
#include <memory>

struct ManagerTest : testing::Test {
    std::unique_ptr<ClassProject::Manager> manager = std::make_unique<ClassProject::Manager>();

    ClassProject::BDD_ID false_id = manager->False();
    ClassProject::BDD_ID true_id = manager->True();

    ClassProject::BDD_ID a_id = manager->createVar("a");
    ClassProject::BDD_ID b_id = manager->createVar("b");

    ClassProject::BDD_ID a_and_b_id = manager->and2(a_id, b_id);
    ClassProject::BDD_ID b_and_a_id = manager->and2(b_id, a_id);

};

TEST_F(ManagerTest, test) /* NOLINT */
{
    EXPECT_EQ(a_and_b_id, b_and_a_id);
}

TEST_F(ManagerTest, IsVariableTest) /* NOLINT */
{
    EXPECT_FALSE(manager->isVariable(a_and_b_id));
}

TEST_F(ManagerTest, GetTopVarNameTest) /* NOLINT */
{
    EXPECT_EQ(manager->getTopVarName(false_id), "False");
    EXPECT_EQ(manager->getTopVarName(true_id), "True");
    EXPECT_EQ(manager->getTopVarName(a_and_b_id), "a");
}

#endif
