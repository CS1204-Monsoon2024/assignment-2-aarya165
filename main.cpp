#include <iostream>
#include <vector>
using namespace std;

class HashTable {
private:
    struct Node {
        int key;
        bool deleted; // Just to mark deleted spots
        Node(int k) : key(k), deleted(false) {}
    };

    vector<Node*> table;  // Array of pointers to hold keys
    int cap;              // Total capacity of the hash table
    int count;            // How many elements are actually in the table
    const float loadThreshold = 0.8; // Resize when table is 80% full
    const int maxAttempts = 10;      // Limit for quadratic probing

    // Simple hash function, key % table size
    int hashFunc(int key) {
        return key % cap;
    }

    // Quadratic probing (h(k) + i^2) % cap
    int quadraticProbe(int hashVal, int i) {
        return (hashVal + i * i) % cap;
    }

    // Check if number is prime (needed for resizing)
    bool isPrime(int num) {
        if (num <= 1) return false;
        for (int i = 2; i * i <= num; ++i) {
            if (num % i == 0) return false;
        }
        return true;
    }

    // Get the next prime number after n
    int getNextPrime(int num) {
        while (!isPrime(num)) {
            ++num;
        }
        return num;
    }

    // When the table is too full, double the size and rehash everything
    void resizeTable() {
        int newCap = getNextPrime(2 * cap); // Find next prime
        vector<Node*> newTable(newCap, nullptr); // New, bigger table
        int oldCap = cap;
        cap = newCap;

        // Rehash all non-deleted entries into new table
        for (int i = 0; i < oldCap; ++i) {
            if (table[i] != nullptr && !table[i]->deleted) {
                int hashVal = hashFunc(table[i]->key);
                int j = 0, newIdx;
                // Quadratic probing again to place in new table
                do {
                    newIdx = quadraticProbe(hashVal, j);
                    ++j;
                } while (newTable[newIdx] != nullptr);
                newTable[newIdx] = new Node(table[i]->key);
            }
        }
        table = move(newTable); // Swap old table with new one
    }

public:
    // Constructor
    HashTable(int initSize) {
        cap = getNextPrime(initSize); // Make sure we start with a prime size
        table.resize(cap, nullptr);   // Empty table
        count = 0;
    }

    // Insert key using quadratic probing
    void insert(int key) {
        // First check if the key is already there
        if (search(key) != -1) {
            cout << "Duplicate key insertion is not allowed" << endl;
            return;
        }

        // Resize the table if it's getting full
        if ((float)count / cap >= loadThreshold) {
            resizeTable();
        }

        int hashVal = hashFunc(key);
        int i = 0, idx;
        while (i < maxAttempts) {
            idx = quadraticProbe(hashVal, i);
            if (table[idx] == nullptr || table[idx]->deleted) {
                table[idx] = new Node(key);
                ++count;
                return;
            }
            ++i;
        }
        // If we exceed the probing limit
        cout << "Max probing limit reached!" << endl;
    }

    // Search for a key in the table, return its index if found, else -1
    int search(int key) {
        int hashVal = hashFunc(key);
        int i = 0, idx;
        while (i < cap) { // Stop if we've searched the entire table
            idx = quadraticProbe(hashVal, i);
            if (table[idx] == nullptr) {
                return -1; // Not found
            }
            if (table[idx]->key == key && !table[idx]->deleted) {
                return idx; // Found it
            }
            ++i;
        }
        return -1; // Still not found
    }

    // Remove a key by marking it as deleted
    void remove(int key) {
        int idx = search(key);
        if (idx == -1) {
            cout << "Element not found" << endl;
            return;
        }
        table[idx]->deleted = true; // Lazy deletion
        --count;
    }

    // Print the table in the specified format
    void printTable() {
        for (int i = 0; i < cap; ++i) {
            if (table[i] == nullptr || table[i]->deleted) {
                cout << "- ";
            } else {
                cout << table[i]->key << " ";
            }
        }
        cout << endl;
    }

    // Destructor to clean up memory
    ~HashTable() {
        for (auto entry : table) {
            if (entry != nullptr) {
                delete entry;
            }
        }
    }
};

// Example main to show how it works (not the one for grading)
int main() {
    HashTable ht(5);

    // Insert a few numbers
    ht.insert(10);
    ht.insert(20);
    ht.insert(15);
    ht.insert(5);
    ht.insert(3);

    // Print the table
    ht.printTable();

    // Try inserting a duplicate
    ht.insert(15);  // Should print "Duplicate key insertion is not allowed"

    // Try removing an element
    ht.remove(15);
    ht.remove(100);  // Should print "Element not found"

    // Print the table again
    ht.printTable();

    return 0;
}

