//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <gtest/gtest.h>
#include "../Manager.h"
#include <memory>

using namespace ClassProject;

class ManagerTest : public testing::Test {
public:
        std::unique_ptr<Manager> m;
        BDD_ID a;
        BDD_ID b;
        BDD_ID c;
        BDD_ID d;
        BDD_ID a_and_b_id;
        BDD_ID b_and_a_id;
        BDD_ID a_or_b_id;
        BDD_ID neg_a_id;
        BDD_ID neg_b_id;
        BDD_ID neg_c_id;
        BDD_ID neg_d_id;

        BDD_ID a_and_neg_b_id;
        BDD_ID neg_a_and_b_id;
        BDD_ID a_xor_b;
        BDD_ID nor_a_b_id;
        BDD_ID xnor_a_b_id;
        BDD_ID a_greater_equal_b;
        BDD_ID a_less_equal_b;
        BDD_ID nand_a_b_id;
        BDD_ID c_and_neg_d_id;
        BDD_ID complexBDD;

        BDD_ID complexBDD_pos_cofactor;
        BDD_ID complexBDD_neg_cofactor;



        void SetUp() override {
            m = std::make_unique<Manager>();
            a = m->createVar("a");  // ID 2
            b = m->createVar("b");  // ID 3
            c = m->createVar("c");  // ID 4
            d = m->createVar("d");  // ID 5
            a_and_b_id = m->and2(a, b);
            b_and_a_id = m->and2(b, a);
            a_or_b_id = m->or2(a, b);
            neg_a_id = m->neg(a);
            neg_b_id = m->neg(b);
            neg_c_id = m->neg(c);
            neg_d_id = m->neg(d);

            a_xor_b = m->xor2(a, b);

            nor_a_b_id = m->nor2(a, b);
            xnor_a_b_id = m->xnor2(a, b);
            a_greater_equal_b = m->or2(a, neg_b_id);
            a_less_equal_b = m->or2(neg_a_id, b);
            nand_a_b_id = m->nand2(a, b);

            a_and_neg_b_id = m->and2(a, neg_b_id);
            neg_a_and_b_id = m->and2(neg_a_id, b);

            c_and_neg_d_id = m->and2(c, neg_d_id);
            complexBDD = m->or2(a_and_b_id, c_and_neg_d_id);
            complexBDD_pos_cofactor = m->or2(b, m->and2(c, neg_d_id));
            complexBDD_neg_cofactor = m->and2(c, neg_d_id);
        }

        void TearDown() override {
            m.reset();
        }


};
    // Tests Lukas Philipp begin
    TEST_F(ManagerTest, Test3)
    {
        EXPECT_EQ(m->ite(a, b, neg_a_id), m->or2(a_and_b_id, neg_a_id));
        BDD_ID test = m->ite(b, m->True(),a);
        EXPECT_EQ(test, a_or_b_id);
    }

    TEST_F(ManagerTest, test)
    {
        EXPECT_EQ(a_and_b_id, b_and_a_id);
        m->visualizeBDD("a_and_b.dot", a_and_b_id);
        m->visualizeBDD("b_and_a.dot", b_and_a_id);
    }

    TEST_F(ManagerTest, IsVariableTest)
    {
        EXPECT_FALSE(m->isVariable(a_and_b_id));
    }

    TEST_F(ManagerTest, GetTopVarNameTest)
    {
        EXPECT_EQ(m->getTopVarName(m->False()), "False");
        EXPECT_EQ(m->getTopVarName(m->True()), "True");
        EXPECT_EQ(m->getTopVarName(a_and_b_id), "a");
        EXPECT_EQ(m->topVar(a_and_b_id), m->topVar(a));
    }

    TEST_F(ManagerTest, XNOR2Test)
    {
        BDD_ID neg_a_id = m->neg(a);
        BDD_ID neg_b_id = m->neg(b);
        BDD_ID xnor2_neg_b_neg_a = m->xnor2(neg_b_id, neg_a_id);
        BDD_ID xnor2_b_a =  m->xnor2(a, b);

        EXPECT_EQ(xnor2_neg_b_neg_a, xnor2_b_a);
    }

    TEST_F(ManagerTest, Test2)
    {
        BDD_ID neg_a_id = m->neg(a);

        BDD_ID a_and_b_id = m->and2(a, b);

        BDD_ID a_or_b_id = m->or2(a, b);

        EXPECT_EQ(m->ite(a, b, neg_a_id), m->or2(a_and_b_id, neg_a_id));
        EXPECT_EQ(m->ite(b, TrueId, a), a_or_b_id);
    }
    //end Tests Lukas Philipp

    TEST_F(ManagerTest, createVar) {
        EXPECT_EQ(a, 2);
        EXPECT_EQ(b, 3);
        EXPECT_EQ(c, 4);
        EXPECT_EQ(d, 5);
    }

    TEST_F(ManagerTest, True) {
        EXPECT_EQ(m->True(), TrueId);
    }


    TEST_F(ManagerTest, False) {
        EXPECT_EQ(m->False(), FalseId);
    }


    TEST_F(ManagerTest, isConstant) {

        EXPECT_EQ(m->isConstant(m->False()), TrueId);
        EXPECT_EQ(m->isConstant(m->True()), TrueId);
        EXPECT_EQ(m->isConstant(a), FalseId);
        EXPECT_EQ(m->isConstant(b), FalseId);
        EXPECT_EQ(m->isConstant(c), FalseId);
        EXPECT_EQ(m->isConstant(d), FalseId);
    }

    TEST_F(ManagerTest, variable_recognition) {

        EXPECT_EQ(m->isVariable(a), TrueId);
        EXPECT_EQ(m->isVariable(b), TrueId);
        EXPECT_EQ(m->isVariable(c), TrueId);
        EXPECT_EQ(m->isVariable(d), TrueId);

        EXPECT_EQ(m->isVariable(m->False()), FalseId);
        EXPECT_EQ(m->isVariable(m->True()), FalseId);
    }

    TEST_F(ManagerTest, ite) {

        EXPECT_EQ(m->ite(m->True(), m->False(), m->True()), FalseId);
        EXPECT_EQ(m->ite(m->False(), m->False(), m->True()), TrueId);

        EXPECT_EQ(m->ite(m->False(), a, b), b);
        EXPECT_EQ(m->ite(m->True(), a, b), a);
        EXPECT_EQ(m->ite(a, m->True(), m->False()), a);
        EXPECT_EQ(m->ite(c, d, d), d);
        EXPECT_EQ(m->ite(a, m->False(), m->True()), m->neg(a)); //Verbessert

        EXPECT_EQ(m->ite(m->False(), m->False(), m->False()), m->False());
        EXPECT_EQ(m->ite(a, b, m->False()), a_and_b_id);

        EXPECT_EQ(m->ite(a, neg_b_id, m->False()), a_and_neg_b_id);



        EXPECT_EQ(m->ite(a, m->False(), b), neg_a_and_b_id);

        EXPECT_EQ(m->ite(a, neg_b_id, b), a_xor_b);
        EXPECT_EQ(m->ite(a, m->True(), b), a_or_b_id);
        EXPECT_EQ(m->ite(a, m->False(), neg_b_id), nor_a_b_id);
        EXPECT_EQ(m->ite(a, b, neg_b_id), xnor_a_b_id);
        EXPECT_EQ(m->ite(b, m->False(), m->True()), neg_b_id);
        EXPECT_EQ(m->ite(a, m->True(), neg_b_id), a_greater_equal_b);
        EXPECT_EQ(m->ite(a, b, m->True()), a_less_equal_b);
        EXPECT_EQ(m->ite(a, neg_b_id, m->True()), nand_a_b_id);
    }

    TEST_F(ManagerTest, coFactorTrue) {

        // Basic cases
        EXPECT_EQ(m->coFactorTrue(m->True()), TrueId);
        EXPECT_EQ(m->coFactorTrue(m->False()), FalseId);
        EXPECT_EQ(m->coFactorTrue(a), TrueId); // Assuming 'a' evaluates to True

        // Cofactor with a variable that is not in the BDD
        EXPECT_EQ(m->coFactorTrue(b), m->True()); // Assuming 'b' is a BDD variable

        // Cofactor with a variable that is in the BDD
        EXPECT_EQ(m->coFactorTrue(c), m->True()); // Assuming 'c' evaluates to True

        // Cofactor with a complex BDD
        EXPECT_EQ(m->coFactorTrue(complexBDD, a), complexBDD_pos_cofactor);

        // Cofactor with a negated variable
        EXPECT_EQ(m->coFactorTrue(neg_a_id), m->False());
    }

    TEST_F(ManagerTest, coFactorFalse) {

        // Basic cases
        EXPECT_EQ(m->coFactorFalse(m->False()), FalseId);
        EXPECT_EQ(m->coFactorFalse(m->True()), TrueId);
        EXPECT_EQ(m->coFactorFalse(a), FalseId); // Assuming 'a' evaluates to False

        // Cofactor with a variable that is not in the BDD
        EXPECT_EQ(m->coFactorFalse(b), m->False()); // Assuming 'b' is a BDD variable

        // Cofactor with a variable that is in the BDD
        EXPECT_EQ(m->coFactorFalse(c), m->False()); // Assuming 'c' evaluates to False

        // Cofactor with a complex BDD
        EXPECT_EQ(m->coFactorFalse(complexBDD, a), complexBDD_neg_cofactor);

        // Cofactor with a negated variable
        EXPECT_EQ(m->coFactorFalse(neg_a_id), m->True());
    }

    TEST_F(ManagerTest, and2_function) {

        // truth table
        EXPECT_EQ(m->and2(m->False(), m->False()), FalseId);
        EXPECT_EQ(m->and2(m->False(), m->True()), FalseId);
        EXPECT_EQ(m->and2(m->True(), m->False()), FalseId);
        EXPECT_EQ(m->and2(m->True(), m->True()), TrueId);

        EXPECT_EQ(m->and2(a,b),m->ite(a,b,m->False()));
    }

    TEST_F(ManagerTest, or2_function) {

        // truth table
        EXPECT_EQ(m->or2(m->False(), m->False()), FalseId);
        EXPECT_EQ(m->or2(m->False(), m->True()), TrueId);
        EXPECT_EQ(m->or2(m->True(), m->False()), TrueId);
        EXPECT_EQ(m->or2(m->True(), m->True()), TrueId);

        EXPECT_EQ(m->or2(a, b), m->ite(a, m->True(), b));
    }

    TEST_F(ManagerTest, xor2_function) {

        // truth table
        EXPECT_EQ(m->xor2(m->False(), m->False()), FalseId);
        EXPECT_EQ(m->xor2(m->False(), m->True()), TrueId);
        EXPECT_EQ(m->xor2(m->True(), m->False()), TrueId);
        EXPECT_EQ(m->xor2(m->True(), m->True()), FalseId);
    }

    TEST_F(ManagerTest, neg_function) {

        // truth table
        // returns the ID representing the negation of the given function.
        EXPECT_EQ(m->neg(m->True()), FalseId);
        EXPECT_EQ(m->neg(m->False()), TrueId);
    }

    TEST_F(ManagerTest, nand2_function) {

        // truth table
        EXPECT_EQ(m->nand2(m->False(), m->False()), TrueId);
        EXPECT_EQ(m->nand2(m->False(), m->True()), TrueId);
        EXPECT_EQ(m->nand2(m->True(), m->False()), TrueId);
        EXPECT_EQ(m->nand2(m->True(), m->True()), FalseId);
    }

    TEST_F(ManagerTest, nor2_function) {

        // truth table
        EXPECT_EQ(m->nor2(m->False(), m->False()), TrueId);
        EXPECT_EQ(m->nor2(m->False(), m->True()), FalseId);
        EXPECT_EQ(m->nor2(m->True(), m->False()), FalseId);
        EXPECT_EQ(m->nor2(m->True(), m->True()), FalseId);
    }

    TEST_F(ManagerTest, xnor2_function) {

        // truth table
        EXPECT_EQ(m->xnor2(m->False(), m->False()), TrueId);
        EXPECT_EQ(m->xnor2(m->False(), m->True()), FalseId);
        EXPECT_EQ(m->xnor2(m->True(), m->False()), FalseId);
        EXPECT_EQ(m->xnor2(m->True(), m->True()), TrueId);
    }


    TEST_F(ManagerTest, getTopVarName) {

        // returns the label of the given BDD_ID
        EXPECT_EQ(m->getTopVarName(m->False()), "False");
        EXPECT_EQ(m->getTopVarName(m->True()), "True");
        EXPECT_EQ(m->getTopVarName(a), "a");
    }

    TEST_F(ManagerTest, findNodes) {

        BDD_ID a_or_b = m->or2(a, b);
        BDD_ID c_and_d = m->and2(c, d);
        BDD_ID f = m->and2(a_or_b, c_and_d);

        std::set<BDD_ID> expected = {m->and2(b, c_and_d), c_and_d, d, m->True(), m->False()};
        std::set<BDD_ID> nodes;
        m->findNodes(m->and2(b, c_and_d), nodes);

        // Assert that the nodes found match the expected nodes
        EXPECT_EQ(nodes, expected);

    }

    TEST_F(ManagerTest, findVars) {

        BDD_ID a_or_b = m->or2(a, b);
        BDD_ID c_and_d = m->and2(c, d);

        BDD_ID f = m->and2(a_or_b, c_and_d);

        std::set<BDD_ID> expected = {m->topVar(b), m->topVar(c), m->topVar(d)};
        std::set<BDD_ID> vars;

        m->findVars(m->and2(b, c_and_d), vars);

        EXPECT_EQ(vars, expected);
    }

    TEST_F(ManagerTest, uniqueTableSize) {
        BDD_ID size = m->uniqueTableSize();

        m->createVar("e");

        EXPECT_EQ(m->uniqueTableSize(), size + 1);
    }

    TEST_F(ManagerTest, Example_ROBDD){
        BDD_ID f = m->and2(m->or2(a, b), m->and2(c, d));

        m->visualizeBDD("ROBDD.txt",f);
    }

#endif
