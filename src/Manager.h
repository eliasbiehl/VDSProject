// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014

#ifndef VDSPROJECT_MANAGER_H
#define VDSPROJECT_MANAGER_H

#include "ManagerInterface.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_map>


namespace ClassProject {

    // Constants for False and True BDD nodes
    static constexpr BDD_ID FalseId = 0;
    static constexpr BDD_ID TrueId = 1;

    // Structure representing a unique table row
    struct uTableRow {
        BDD_ID high;
        BDD_ID low;
        BDD_ID topVar;

        // Constructor
        uTableRow(BDD_ID high, BDD_ID low, BDD_ID top_var)
            : high(high), low(low), topVar(top_var) {}

        // Equality operator
        bool operator==(const uTableRow& rhs) const
        {
            return high == rhs.high && low == rhs.low && topVar == rhs.topVar;
        }
    };

    // Hash function for uTableRow
    struct uTableRowHash
    {
        size_t operator()(const uTableRow& row) const
        {
            size_t seed = 0x9e3779b9; // Prime number as seed
            seed ^= std::hash<BDD_ID>()(row.high) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<BDD_ID>()(row.low) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<BDD_ID>()(row.topVar) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

    // Manager class for BDD operations
    class Manager : public ManagerInterface {
    private:
        std::unordered_map<BDD_ID, uTableRow> unique_tb; // Unique table
        std::unordered_map<uTableRow, BDD_ID, uTableRowHash> rev_uniq_tb; // Reverse unique table
        std::unordered_map<uTableRow, BDD_ID, uTableRowHash> computed_tb; // Computed table

        // Print the unique table
        void print_unique_tb();

        // Initialize the unique table
        void init_unique_tb();

        // Get the next available BDD ID
        BDD_ID get_nextID()
        {
            return uniqueTableSize();
        }

    public:

        // Constructor
        Manager();

        // Destructor
        ~Manager() = default;

        // Create a new variable
        BDD_ID createVar(const std::string &label) override;

        // Return the BDD ID for True
        const BDD_ID &True() override;

        // Return the BDD ID for False
        const BDD_ID &False() override;

        // Get the unique table
        std::unordered_map<BDD_ID, uTableRow> getUniqueTable()
        {
            return unique_tb;
        }

        // Check if the computed table contains a specific row
        bool computedTableContains(const uTableRow& row) const {
            return computed_tb.find(row) != computed_tb.end();
        }

        /**
        * isConstant determines if a node is a leafnode
        * @param f ID of the Node under test
        * @return true if f is a constant 0 or 1
        */
        bool isConstant(BDD_ID f) override;

        /**
        * isVariable determines if a node is a variable
        * @param x ID of the Node under test
        * @return returns true if id of x is equal to topVar of x and x is not a leafnode
        * see /doc/VDSCP_Part_1.pdf Tab. 2 for a better understanding
        */
        bool isVariable(BDD_ID x) override;

        /**
        * topVar returns the top Variable of the node
        * @param f ID of the Node under test
        * @return topVar of f
        */
        BDD_ID topVar(BDD_ID f) override;

        // ITE (if-then-else) operation
        BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;

        // Compute the cofactor of a node with respect to a variable (true branch)
        BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;

        // Compute the cofactor of a node with respect to a variable (false branch)
        BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;

        // Compute the cofactor of a node (true branch)
        BDD_ID coFactorTrue(BDD_ID f) override;

        // Compute the cofactor of a node (false branch)
        BDD_ID coFactorFalse(BDD_ID f) override;

        // Swap two BDD IDs
        static void swapID(BDD_ID& a, BDD_ID& b);

        // Apply standard triples simplifications
        void standard_triples(BDD_ID& i, BDD_ID& t, BDD_ID& e);

        // AND operation
        BDD_ID and2(BDD_ID a, BDD_ID b) override;

        // OR operation
        BDD_ID or2(BDD_ID a, BDD_ID b) override;

        // XOR operation
        BDD_ID xor2(BDD_ID a, BDD_ID b) override;

        // Negation operation
        BDD_ID neg(BDD_ID a) override;

        // NAND operation
        BDD_ID nand2(BDD_ID a, BDD_ID b) override;

        // NOR operation
        BDD_ID nor2(BDD_ID a, BDD_ID b) override;

        // XNOR operation
        BDD_ID xnor2(BDD_ID a, BDD_ID b) override;

        // Get the name of the top variable of a node
        std::string getTopVarName(const BDD_ID &root) override;

        // Find all nodes reachable from a root node
        void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) override;

        // Find all variables in the BDD rooted at a node
        void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) override;

        // Get the size of the unique table
        size_t uniqueTableSize() override;

        // Visualize the BDD
        void visualizeBDD(std::string filepath, BDD_ID &root) override;
    };
}



#endif
