#include "Manager.h"

#include <algorithm>


namespace ClassProject {

    Manager::Manager() {
        init_unique_tb();
    }

    void Manager::init_unique_tb() {
        //computed_tb[uTableRow {False(), False(), False(), "False"}] = False();
        // unique_tb.emplace(uniqueTableSize(), uTableRow {False(), False(), False(), "False"});
        // rev_uniq_tb[uTableRow {False(), False(), False(), "False"}] = False();
        // //computed_tb[uTableRow{True(), True(), True(), "True"}] = True();
        // unique_tb.emplace(uniqueTableSize(), uTableRow{True(), True(), True(), "True"});
        // rev_uniq_tb[uTableRow{True(), True(), True(), "True"}] = True();
        unique_tb.emplace(uniqueTableSize(), uTableRow {False(), False(), False()});
        rev_uniq_tb[uTableRow {False(), False(), False()}] = False();
        unique_tb.emplace(uniqueTableSize(), uTableRow{True(), True(), True()});
        rev_uniq_tb[uTableRow{True(), True(), True()}] = True();
    }

    BDD_ID Manager::createVar(const std::string &label){
        const BDD_ID id = get_nextID();
        // unique_tb.emplace(uniqueTableSize(), uTableRow{True(), False(), id, label});
        // rev_uniq_tb[uTableRow{True(), False(), id, label}] = id;
        unique_tb.emplace(uniqueTableSize(), uTableRow{True(), False(), id});
        rev_uniq_tb[uTableRow{True(), False(), id}] = id;
        return id;
    }

    const BDD_ID &Manager::True(){
        return TrueId;
    }

    const BDD_ID &Manager::False(){
        return FalseId;
    }

    bool Manager::isConstant(const BDD_ID f){
        return f <= 1;
    }

    bool Manager::isVariable(const BDD_ID x){
        return unique_tb.at(x).topVar == x && !isConstant(x);
    }

    BDD_ID Manager::topVar(const BDD_ID f){
        return unique_tb.at(f).topVar;
    }

    BDD_ID Manager::ite(const BDD_ID i, const BDD_ID t, const BDD_ID e){
        // Check for terminal cases
        if (i == True()) {
            return t;
        }
        if (i == False())
        {
            return e;
        }
        if (t == True() && e == False()) {
            return i;
        }
        if (t == e)
        {
            return t;
        }

        // Standard Triplets
        if (i == t && !isConstant(t)) //ite( F, F, G) => ite( F, 1, G)
        {
            return ite(i, True(), e);
        }
        if (i == e) //ite( F, G, F) => ite( F, G, 0)
        {
            return ite(i, t, False());
        }
        if (i == neg(e))//ite( F, G, !F) => ite( F, G, 1)
        {
            return ite(i, t, True());
        }
        if (i == neg(t)) //ite( F, !F, G) => ite( F, 0, G)
        {
            return ite(i, False(), e);
        }
        // First, the following simplifications are applied to the arguments of the ite where possible:
        // if (!isConstant(t) && !isConstant(e))
        // {
        //     if (t == True() && topVar(i) > topVar(e)) //ite( F, 1, G) = ite( G, 1, F)
        //     {
        //         return ite(e, t, i);
        //     }
        //     if (e == False() && topVar(i) > topVar(t)) //ite( F, G, 0) = ite( G, F, 0)
        //     {
        //         return ite(t, i, e);
        //     }
        //     if (e == True() && topVar(i) > topVar(t)) //ite( F, G, 1) = ite( !G, !F, 1)
        //     {
        //         return ite(neg(t), neg(i), True());
        //     }
        //     if (t == False() && topVar(i) > topVar(e)) //ite( F, 0, G) = ite( !G, 0, !F)
        //     {
        //         return ite(neg(e), False(), neg(i));
        //     }
        //     if (e == neg(t) && topVar(i) > topVar(t)) //ite( F, G, !G) = ite( G, F, !F)
        //     {
        //         return ite(t, i, neg(i));
        //     }
        // }

        // Check if node already exists
        const auto ite_entry = computed_tb.find(uTableRow(i, t, e));
        if (ite_entry != computed_tb.end())
        {
            // Entry found -> return result
            return ite_entry->second;
        }

        // find the smallest top index for x
        BDD_ID x = topVar(i);
        if (topVar(t) < x && isVariable(topVar(t)))
        {
            x = topVar(t);
        }

        if (topVar(e) < x && isVariable(topVar(e))) {
            x = topVar(e);
        }

        // calculate r_high and r_low like Slide 2-17 VDS Lecture
        const BDD_ID high = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));
        const BDD_ID low = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));

        if (high == low)
        {
            computed_tb[uTableRow(i, t, e)] = high;
            return high;
        }

        const auto uniq_entry = rev_uniq_tb.find(uTableRow(high, low, x));
        if (uniq_entry != rev_uniq_tb.end())
        {
            computed_tb[uTableRow(i, t, e)] = uniq_entry->second;
            return uniq_entry->second;
        }


        // Entry not found
        // Add Entry
        const BDD_ID new_id = get_nextID();
        auto temp_2 = uTableRow(high, low, x);
        computed_tb.emplace(uTableRow(i, t, e), new_id);
        unique_tb.emplace(new_id, uTableRow(high, low, x));
        rev_uniq_tb[uTableRow{high, low, x}] = new_id;
        // Generate Label for Visualization
        // const auto label = "if " + unique_tb.at(x).label + " then " + unique_tb.at(high).label + " else " + unique_tb.at(low).label;
        // unique_tb.emplace(new_id, uTableRow(high, low, x, label));


        return new_id;

    }

    BDD_ID Manager::coFactorTrue(const BDD_ID f, BDD_ID x) {
        // Check for terminal Case and relevancy of x
        if (isConstant(f) || topVar(f) > x || isConstant(x)) {
            return f;
        }

        // CoFactor of f w.r.t x is high path
        if (topVar(f) == x) {
            return unique_tb.at(f).high;
        }

        //recursiv high and low
        BDD_ID high = coFactorTrue(unique_tb.at(f).high, x);
        BDD_ID low = coFactorTrue(unique_tb.at(f).low, x);

        if (high == low) {
            return high;
        }

        // auto entry = rev_uniq_tb.find(uTableRow{high, low, topVar(f)});
        // if (entry != rev_uniq_tb.end())
        // {
        //     return entry->second;
        // }
        // BDD_ID new_id = get_nextID();
        // unique_tb.emplace(new_id, uTableRow{high, low, topVar(f)});
        // rev_uniq_tb[uTableRow{high, low, topVar(f)}] = new_id;
        // return new_id;

        // compute result with ite-function
        return ite(topVar(f), high, low);
    }

    BDD_ID Manager::coFactorFalse(const BDD_ID f, BDD_ID x){
        // Check for terminal Case and relevancy of x
        if (isConstant(f) || topVar(f) > x || isConstant(x)) {
            return f;
        }

        // CoFactor of f w.r.t x is low path
        if (topVar(f) == x) {
            return unique_tb.at(f).low;
        }

        //recursiv high and low
        BDD_ID high = coFactorFalse(unique_tb.at(f).high, x);
        BDD_ID low = coFactorFalse(unique_tb.at(f).low, x);

        if (high == low) {
            return high;
        }

        // auto entry = rev_uniq_tb.find(uTableRow{high, low, topVar(f)});
        // if (entry != rev_uniq_tb.end())
        // {
        //     return entry->second;
        // }
        // BDD_ID new_id = get_nextID();
        // unique_tb.emplace(new_id, uTableRow{high, low, topVar(f)});
        // rev_uniq_tb[uTableRow{high, low, topVar(f)}] = new_id;
        // return new_id;

        // compute result with ite-function
        return ite(topVar(f), high, low);

    }

    BDD_ID Manager::coFactorTrue(const BDD_ID f){
        return unique_tb.at(f).high;
    }

    BDD_ID Manager::coFactorFalse(const BDD_ID f){
        return unique_tb.at(f).low;    
    }

    void Manager::swapID(BDD_ID &a, BDD_ID &b){
        const BDD_ID temp = a;
        a = b;
        b = temp;
    }

    void Manager::standard_triples(BDD_ID &i, BDD_ID &t, BDD_ID &e){
        //First, the following simplifications are applied to the arguments of the ite where possible:
        if (i == t) //ite( F, F, G) => ite( F, 1, G)
        {
            t = True();
        } else if (i == e) //ite( F, G, F) => ite( F, G, 0)
        {
            e = False();
        } else if (i == neg(e))//ite( F, G, !F) => ite( F, G, 1)
        {
            e = True();
        } else if (i == neg(t)) //ite( F, !F, G) => ite( F, 0, G)
        {
            t = False();
        }
        // First, the following simplifications are applied to the arguments of the ite where possible:
        if (t == True() && i > e) //ite( F, 1, G) = ite( G, 1, F)
        {
            swapID(i, e);
        } else if (e == False() && i > t) //ite( F, G, 0) = ite( F, G, 0)
        {
            swapID(i, t);
        } else if (e == True() && i > t) //ite( F, G, 1) = ite( !G, !F, 1)
        {
            i = neg(i);
            t = neg(t);
            swapID(i, t);
        } else if (t == False() && i > e) //ite( F, 0, G) = ite( !G, 0, !F)
        {
            e = neg(e);
            i = neg(i);
            swapID(i, e);
        } else if (e == neg(t) && i > t) //ite( F, G, !G) = ite( G, F, !F)
        {
            e = neg(i);
            swapID(i, t);
        }
        // Complement edges lead to the following equivalences:
        if (i != t && i != e && t != e) {    // F != G != H
            if (t > e) {                     // ite(F,G,H) = ite(!F,H,G)
                i = neg(i);
                swapID(t, e);
            }
        }
    }

    // Slide 2-15
    BDD_ID Manager::and2(const BDD_ID a, const BDD_ID b){
        return ite(a, b, False());
    }

    // Slide 2-15
    BDD_ID Manager::or2(const BDD_ID a, const BDD_ID b){
        return ite(a, True(), b);
    }

    // Slide 2-15
    BDD_ID Manager::xor2(const BDD_ID a, const BDD_ID b){
        return ite(a, neg(b), b);
    }

    // Slide 2-15
    BDD_ID Manager::neg(const BDD_ID a){
        return ite(a, False(), True());
    }

    // Abb. 4 https://agra.informatik.uni-bremen.de/doc/software/manual/index.html
    BDD_ID Manager::nand2(const BDD_ID a, const BDD_ID b){
        return ite(a, neg(b), True());
    }

    // Abb. 4 https://agra.informatik.uni-bremen.de/doc/software/manual/index.html
    BDD_ID Manager::nor2(const BDD_ID a, const BDD_ID b){
        return ite(a, False(), neg(b));
    }

    // Abb. 4 https://agra.informatik.uni-bremen.de/doc/software/manual/index.html
    BDD_ID Manager::xnor2(const BDD_ID a, const BDD_ID b){
        return ite(a, b, neg(b));
    }

    std::string Manager::getTopVarName(const BDD_ID &root){
        // return unique_tb.at(topVar(root)).label;
        return "dummy";
    }

    void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root){

        // Attempt to add current node to the set
        bool insert_successful = nodes_of_root.insert(root).second;

        // Check if node is already processed
        if (insert_successful)
        {
            // recursive call for following nodes (high and low)
            findNodes(unique_tb.at(root).high, nodes_of_root);
            findNodes(unique_tb.at(root).low, nodes_of_root);
        }
    }

    void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root){

        std::set<BDD_ID> nodes;
        findNodes(root, nodes);
        for(const BDD_ID &node : nodes)
        {
            BDD_ID top = topVar(node);

            // Check for terminal node
            if (isVariable(top))
            {
                vars_of_root.insert(top);
            }
        }
    }

    size_t Manager::uniqueTableSize(){
        return unique_tb.size();
    }

    void Manager::visualizeBDD(std::string filepath, BDD_ID &root){
        
        // Open file to write DOT-file
        std::ofstream file(filepath);

        // Check if file could be opened
        if (!file.is_open())
        {
            std::cerr << "Error opening file " << filepath << std::endl;
            return;
        }

        // Begin of DOT_Visualization
        file << "digraph {" << std::endl;
        file << "  rankdir=TB" << std::endl;

        // set to store reachable nodes
        std::set<BDD_ID> nodes_of_root;
        findNodes(root, nodes_of_root);

        // Set for variables
        std::set<BDD_ID> vars_of_root;
        findVars(root, vars_of_root);

        // Iterate through all Nodes
        for (const auto& node : nodes_of_root)
        {
            // take node from table
            const auto &nodeData = unique_tb.at(node);

            //create Node in DOT-format
            if (vars_of_root.find(node) != vars_of_root.end()) {
                file << "  " << node << " [label=\"" << getTopVarName(topVar(node)) << "\", shape=ellipse, color=blue];" << std::endl;
            } else {
                file << "  " << node << " [label=\"" << getTopVarName(topVar(node)) << "\", shape=box, color=black];" << std::endl;
            }

            // Add edges to following high and low
            if (!isConstant(node)) {
                file << "  " << node << " -> " << nodeData.high << " [label=\"1\"];" << std::endl;
                file << "  " << node << " -> " << nodeData.low << " [label=\"0\"];" << std::endl;
            }
        }

        // End of DOT-visualization
        file << "}" << std::endl;

        // Close file
        file.close();
    }

}

