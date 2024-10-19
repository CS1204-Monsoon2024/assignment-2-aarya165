#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

// Hash table class
class HashTable {
private:
    struct Entry {
        int key;
        int value;
        bool isDeleted; // To handle lazy deletion
        Entry(int k, int v) : key(k), value(v), isDeleted(false) {}
    };

    vector<Entry*> table;  // Array of entry pointers
    int capacity;          // Current size of the hash table (m)
    int size;              // Number of elements in the hash table
    const float loadFactorThreshold = 0.8; // Threshold to trigger resize

    // Hash function: h(k) = k % m
    int hash(int key) {
        return key % capacity;
    }

    // Quadratic probing formula: (h(k) + i^2) % m
    int probe(int hashValue, int i) {
        return (hashValue + i * i) % capacity;
    }

    // Check if a number is prime
    bool isPrime(int n) {
        if (n <= 1) return false;
        for (int i = 2; i * i <= n; ++i) {
            if (n % i == 0) return false;
        }
        return true;
    }

    // Find the next prime number greater than n
    int nextPrime(int n) {
        while (!isPrime(n)) {
            ++n;
        }
        return n;
    }

    // Resize the table when load factor exceeds the threshold
    void resize() {
        int newCapacity = nextPrime(2 * capacity); // New prime capacity
        vector<Entry*> newTable(newCapacity, nullptr); // New larger table
        int oldCapacity = capacity;
        capacity = newCapacity;
        
        for (int i = 0; i < oldCapacity; ++i) {
            if (table[i] != nullptr && !table[i]->isDeleted) {
                int hashValue = hash(table[i]->key);
                int j = 0;
                int newIndex;
                // Quadratic probing to rehash entries
                do {
                    newIndex = probe(hashValue, j);
                    ++j;
                } while (newTable[newIndex] != nullptr);

                newTable[newIndex] = new Entry(table[i]->key, table[i]->value);
            }
        }
        
        table = move(newTable); // Move new table to current table
    }

public:
    // Constructor
    HashTable(int initialCapacity) {
        capacity = nextPrime(initialCapacity); // Initialize with a prime size
        table.resize(capacity, nullptr);       // Resize table
        size = 0;
    }

    // Insert function with quadratic probing
    void insert(int key, int value) {
        // Resize if load factor exceeds threshold
        if ((float)size / capacity >= loadFactorThreshold) {
            resize();
        }

        int hashValue = hash(key);
        int i = 0;
        int index;
        do {
            index = probe(hashValue, i);
            ++i;
        } while (table[index] != nullptr && !table[index]->isDeleted && table[index]->key != key);

        // Insert or update
        if (table[index] == nullptr || table[index]->isDeleted) {
            table[index] = new Entry(key, value);
            ++size;
        } else {
            table[index]->value = value; // Update value if key exists
        }
    }

    // Search for a key
    int search(int key) {
        int hashValue = hash(key);
        int i = 0;
        int index;
        do {
            index = probe(hashValue, i);
            if (table[index] == nullptr) {
                return -1; // Key not found
            }
            if (table[index]->key == key && !table[index]->isDeleted) {
                return table[index]->value; // Key found
            }
            ++i;
        } while (i < capacity);
        return -1; // Key not found
    }

    // Delete a key
    void remove(int key) {
        int hashValue = hash(key);
        int i = 0;
        int index;
        do {
            index = probe(hashValue, i);
            if (table[index] == nullptr) {
                return; // Key not found
            }
            if (table[index]->key == key && !table[index]->isDeleted) {
                table[index]->isDeleted = true; // Lazy delete
                --size;
                return;
            }
            ++i;
        } while (i < capacity);
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

int main() {
    HashTable ht(5);

    // Insertions
    ht.insert(10, 100);
    ht.insert(20, 200);
    ht.insert(15, 150);
    ht.insert(5, 50);
    ht.insert(3, 30);

    // Search
    cout << "Value for key 10: " << ht.search(10) << endl;
    cout << "Value for key 5: " << ht.search(5) << endl;

    // Deletion
    ht.remove(15);
    cout << "Value for key 15 after deletion: " << ht.search(15) << endl;

    return 0;
}

