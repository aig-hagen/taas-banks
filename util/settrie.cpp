#include <map>
#include <set>
#include <map>
#include <vector>
#include <algorithm>

template <typename T>
class SetTrie {
private:
    // Internal node structure
    struct SetTrieNode {
        bool marked;
        std::map<T, SetTrieNode*> children;

        // Constructor
        SetTrieNode() : marked(false) {}

        // Destructor
        ~SetTrieNode() {
            for (auto& [key, child] : children) {
                delete child;
            }
        }

        // Retrieve all sets represented by this node
        std::vector<std::set<T>> sets() {
            std::vector<std::set<T>> result;
            for (const auto& [key, child] : children) {
                auto subsets = child->sets();
                for (auto& subset : subsets) {
                    subset.insert(key);
                    result.push_back(subset);
                }
            }
            if (marked) {
                result.emplace_back(); // Add an empty set
            }
            return result;
        }

        // Add a sorted set to the trie
        bool add(const std::vector<T>& set, size_t index, bool onlyForSubsetTests) {
            if (index >= set.size()) {
                if (marked) {
                    return false;
                }
                marked = true;
                if (onlyForSubsetTests) {
                    for (auto& [key, child] : children) {
                        delete child;
                    }
                    children.clear();
                }

                return true;
            }
            if (children.find(set[index]) == children.end()) {
                children[set[index]] = new SetTrieNode();
            }
            return children[set[index]]->add(set, index + 1, onlyForSubsetTests);
        }

        // Check if a sorted set is contained in the trie
        bool contains(const std::vector<T>& set, size_t index) const {
            if (index >= set.size()) {
                return marked;
            }
            auto it = children.find(set[index]);
            return it != children.end() && it->second->contains(set, index + 1);
        }

        // Check if this node contains a set with the given element
        bool containsElement(const T& elem) const {
            if (children.find(elem) != children.end()) {
                return true;
            }
            for (const auto& [key, child] : children) {
                if (child->containsElement(elem)) {
                    return true;
                }
            }
            return false;
        }

        // Check if the trie contains a subset of the given set
        bool containsSubsetOf(const std::vector<T>& set, size_t index) const {
            if (marked) {
                return true;
            }
            if (index >= set.size()) {
                return false;
            }
            auto it = children.find(set[index]);
            if (it != children.end() && it->second->containsSubsetOf(set, index + 1)) {
                return true;
            }
            return containsSubsetOf(set, index + 1);
        }

        // Count the number of sets stored in the trie
        int actualSize() const {
            int count = marked ? 1 : 0;
            for (const auto& [key, child] : children) {
                count += child->actualSize();
            }
            return count;
        }

        // Count the total number of nodes in the trie
        int numberOfNodes() const {
            int count = 1; // Count this node
            for (const auto& [key, child] : children) {
                count += child->numberOfNodes();
            }
            return count;
        }
    };

    SetTrieNode* root;
    bool onlyForSubsetTests;
    int size;

public:
    // Constructors
    SetTrie() : SetTrie(false) {}

    SetTrie(bool onlyForSubsetTests) : onlyForSubsetTests(onlyForSubsetTests) { root = new SetTrieNode(); }


    // Destructor
    ~SetTrie() {
        delete root;
    }

    // Get size of the trie
    int sizeOf() const {
        return size;
    }

    // Get actual size (number of sets)
    int actualSize() const {
        return root->actualSize();
    }

    // Get total number of nodes
    int numberOfNodes() const {
        return root->numberOfNodes();
    }

    // Add a set to the trie
    bool add(const std::set<T>& set) {
        if (onlyForSubsetTests && containsSubsetOf(set)) {
            return false;
        }
        std::vector<T> sorted(set.begin(), set.end());
        std::sort(sorted.begin(), sorted.end());
        bool added = root->add(sorted, 0, onlyForSubsetTests);
        if (added) {
            ++size;
        }
        return added;
    }

    // Check if the trie contains a set
    bool contains(const std::set<T>& set) const {
        std::vector<T> sorted(set.begin(), set.end());
        std::sort(sorted.begin(), sorted.end());
        return root->contains(sorted, 0);
    }

    // Check if the trie contains a subset of a given set
    bool containsSubsetOf(const std::set<T>& set) const {
        std::vector<T> sorted(set.begin(), set.end());
        std::sort(sorted.begin(), sorted.end());
        return root->containsSubsetOf(sorted, 0);
    }

    // Check if the trie contains a set with a specific element
    bool containsElement(const T& elem) const {
        return root->containsElement(elem);
    }

    // Retrieve all sets in the trie
    std::vector<std::set<T>> sets() const {
        return root->sets();
    }
};
