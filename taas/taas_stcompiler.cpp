#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

class SatSetTrieStableCompiler {

public:
    // Constructor
    SatSetTrieStableCompiler() {}

    // Compile method
    SetTrieCompilation compile(struct AAF* aaf) {
        auto trie = std::make_shared<SetTrie<int>>(false);
        // Base encoding for stable semantics
        IpasirSolver solver;
        sat__init(solver, aaf->number_of_arguments);
        // Rules for stable semantics
        for (int a = 1; a <= aaf->number_of_arguments; a++) {
            if (aaf->number_of_attackers[a-1] == 0) {
                sat__addClause1(solver,a);
            } else {
                std::vector<int> attackersOr;
                for(GSList* node2 = aaf->parents[a-1]; node2 != NULL; node2 = node2->next){
                    attackersOr.push_back(*(int*)node2->data);
                    sat__addClause2(solver,-a,-*(int*)node2->data);
                }
                attackersOr.push_back(a);
                attackersOr.push_back(0);
                sat__addClauseZTVec(solver,attackersOr);
            }
        }
        // Iteratively find and add extensions
        while (true) {
            int sat = sat__solve(solver);
            if (sat == 20) {
                break; // No more extensions
            }
            std::set<int> ext;
            std::vector<int> clause;
            for (int a = 1; a <= aaf->number_of_arguments; a++){
                if (sat__get(solver,a) > 0) {
                    ext.insert(a);
                    clause.push_back(-a);
                }
            }
            clause.push_back(0);
            trie->add(ext);
            sat__addClauseZTVec(solver,clause);
        }

        return SetTrieCompilation(trie, aaf->number_of_arguments);
    }
};
