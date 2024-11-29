#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <unordered_set>

enum class InferenceMode { CREDULOUS, SKEPTICAL };

class SetTrieCompilation {
private:
    // A set trie representing the extensions
    std::shared_ptr<SetTrie<int>> setTrie;

    // The number of all arguments
    int num_arguments;

public:
    // Constructor
    SetTrieCompilation(std::shared_ptr<SetTrie<int>> setTrie, int n_args)
        : setTrie(std::move(setTrie)) {  num_arguments=n_args; }

    // Returns all models in the set trie
    std::vector<std::set<int>> getModels() const {
        return setTrie->sets();
    }

    // Returns the number of models
    int numModels() const {
        return setTrie->actualSize();
    }

    // Query for an argument with a specific inference mode
    bool query(int a, InferenceMode inferenceMode) const {
        if (inferenceMode == InferenceMode::CREDULOUS) {
            return setTrie->containsElement(a);
        } else {
            // SKEPTICAL inference mode
            std::set<int> remainingArgs = {};
            for(int i = 1; i <= num_arguments; i++ )
                remainingArgs.insert(i);
            remainingArgs.erase(a); // Remove the queried argument
            return !setTrie->containsSubsetOf(remainingArgs);
        }
    }
};
