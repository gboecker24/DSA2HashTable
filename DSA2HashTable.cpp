#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

struct Node {
    string key; // the word stored
    int value; // the number associated with the word
    Node* next; // pointer to the next node in the same section
    
    // constructor, sets next value to nullptr when node is created
    Node(string k, int v) : key(k), value(v), next(nullptr) {}
};


/* uses an array of pointers (vector, the guy from dispicable me)
   each pointer is the head of a linked list (chain)
   when multiple keys hash to the same index, they're stored in a chain at that index.
*/
class HashTable {
private:
    vector<Node*> table;
    int count;  // total number of keyvalue pairs stored
    bool debugMode; // when true more details will show
    
    /* Converts a string key into an array index
       hashValue starts at 0
       For each character in the string:
       - multiply the current hash by 31
       - add the ASCII value of the character
       These numbers are always set to positive
       Moduloed by table size to get the index to stay within the array bounds
    */
    int hash(const string& key) const {
        int hashValue = 0;
        for (char c : key) {
            hashValue = hashValue * 31 + c;
        }
        return abs(hashValue) % table.size();
    }
    

    /*
        Checks if too many items have been added
        Load factor = items / table size
        if the load factor is over 75%, the table expands
        this keeps the table from getting too slow
    */
    void checkAndResize() {
        float loadFactor = (float)count / table.size();
        
        if (debugMode) {
            cout << "\n[DEBUG] Load factor: " << loadFactor 
                 << " (" << count << "/" << table.size() << ")" << endl;
        }
        
        if (loadFactor >= 0.75f) {
            resize();
        }
    }
    

    /*
        doubles the table size and rehashes everything in the table
        saves the old table, then creates a new larger table
        for each item in the old table, calculate a new index and insert
        check to see if the new item count matches the old one
    */
    void resize() {
        int oldCapacity = table.size();
        int newCapacity = oldCapacity * 2;
        
        if (debugMode) {
            cout << "\n[DEBUG] ===== RESIZING =====" << endl;
            cout << "[DEBUG] Old capacity: " << oldCapacity << ", New capacity: " << newCapacity << endl;
        }
        
        // saves pointer to old table before replacing it
        vector<Node*> oldTable = table;
        // new table with double the size
        table = vector<Node*>(newCapacity, nullptr);
        
        int oldCount = count;
        count = 0;
        
        // rehashes every item to fit the new table
        for (int i = 0; i < oldCapacity; i++) {
            Node* current = oldTable[i];
            while (current != nullptr) {
                Node* next = current->next;
                int newIndex = hash(current->key);
                
                if (debugMode) {
                    cout << "[DEBUG] Rehashing: '" << current->key 
                         << "' (old index " << i << " -> new index " << newIndex << ")" << endl;
                }
                
                if (table[newIndex] == nullptr) {
                    table[newIndex] = current;
                    current->next = nullptr;
                } else {
                    Node* oldHead = table[newIndex];
                    table[newIndex] = current;
                    current->next = oldHead;
                }
                count++;
                
                current = next;
            }
        }
        
        if (debugMode) {
            cout << "[DEBUG] Resize complete. New size: " << newCapacity 
                 << ", Items: " << count << endl;
            cout << "[DEBUG] ====================" << endl;
        }
        
        // makes sure the number of items matches
        if (oldCount != count) {
            cerr << "ERROR: Count mismatch! Expected " << oldCount 
                 << ", got " << count << endl;
        }
    }
    
public:
    /*
        creates an empty hash table with a capacity of 11
        starts as a table of full nullptrs
    */
    HashTable(int capacity = 11, bool debug = false) 
        : table(capacity, nullptr), count(0), debugMode(debug) {
        if (debugMode) {
            cout << "[DEBUG] Created hash table with capacity " << capacity << endl;
        }
    }
    
    /*
        Creates a copy of the old hash table
    */
    HashTable(const HashTable& other) : table(other.table.size(), nullptr), 
                                        count(0), debugMode(other.debugMode) {
        if (debugMode) {
            cout << "[DEBUG] Copy constructor called" << endl;
        }
        // copy each node
        for (size_t i = 0; i < other.table.size(); i++) {
            Node* current = other.table[i];
            Node* last = nullptr;
            while (current != nullptr) {
                Node* newNode = new Node(current->key, current->value);
                if (last == nullptr) {
                    table[i] = newNode;
                } else {
                    last->next = newNode;
                }
                last = newNode;
                current = current->next;
                count++;
            }
        }
    }
    
    /*
        cleans up old data
    */
    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            // deletes old data
            for (size_t i = 0; i < table.size(); i++) {
                Node* current = table[i];
                while (current != nullptr) {
                    Node* next = current->next;
                    delete current;
                    current = next;
                }
            }
            // copies the other table
            table = vector<Node*>(other.table.size(), nullptr);
            count = 0;
            debugMode = other.debugMode;
            
            for (size_t i = 0; i < other.table.size(); i++) {
                Node* current = other.table[i];
                Node* last = nullptr;
                while (current != nullptr) {
                    Node* newNode = new Node(current->key, current->value);
                    if (last == nullptr) {
                        table[i] = newNode;
                    } else {
                        last->next = newNode;
                    }
                    last = newNode;
                    current = current->next;
                    count++;
                }
            }
        }
        return *this;
    }
    
    /*
        destructor
        clears up the allocated memory
        deletes every node to prevent memory loss
        traves the chains deleting nodes one by one
    */
    ~HashTable() {
        for (size_t i = 0; i < table.size(); i++) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* next = current->next; // saves next before deleting current
                delete current;
                current = next;
            }
        }
    }
    
    /*
        the insert/update operation
        - hash the key to find which vector it belongs to
        - seach the chain in that vector for the key
        - if found: update the value
        - if not found: create a new node and add it to the beginning of the chain
        - increment the count of items and check if the table should resize
    */
    void put(const string& key, int value) {
        if (debugMode) {
            cout << "\n[DEBUG] PUT: " << key << " = " << value << endl;
        }
        // finds the right vector
        int index = hash(key);
        
        if (debugMode) {
            cout << "[DEBUG] Hash of '" << key << "' = " << index << endl;
        }
        // searches for existing key in the vector
        Node* current = table[index];
        while (current != nullptr) {
            if (current->key == key) {
                // updates the value
                if (debugMode) {
                    cout << "[DEBUG] Found existing key, updating value from " 
                         << current->value << " to " << value << endl;
                }
                current->value = value;
                return;
            }
            current = current->next;
        }
        
        Node* newNode = new Node(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        count++;
        
        if (debugMode) {
            cout << "[DEBUG] Added new node at slot " << index << endl;
            cout << "[DEBUG] New count: " << count << endl;
        }
        // checks if the table is too full
        checkAndResize();
    }
    
    /*
        Retrieves a value for a key
        returns a bool depending if the value was found or not
        - hashes the key to find its vector
        - searches the chain of the vector
        - if found: set outValue and return true
        - if not found: return false
    */
    bool get(const string& key, int& outValue) const {
        if (debugMode) {
            cout << "\n[DEBUD] GET: " << key << endl;
        }
        
        // find array
        int index = hash(key);
        
        if (debugMode) {
            cout << "[DEBUG] Hash of '" << key << "' = " << index << endl;
        }
        // search chain
        Node* current = table[index];
        int position = 0;
        
        while (current != nullptr) {
            if (current->key == key) {
                // if found
                if (debugMode) {
                    cout << "[DEBUG] Found at chain position " << position << endl;
                }
                outValue = current->value;
                return true;
            }
            current = current->next;
            position++;
        }
        // if not found
        if (debugMode) {
            cout << "[DEBUG] Key not found" << endl;
        }
        return false;
    }
    
    /*
        deletes a key value pair
        - hashes key
        - finds vector fitting the key
        - searches the chain keeping track of the previous node
        - if found: removes from the chain and deletes node
        - decrement count and return true
        - if not found: return false
    */
    bool removeKey(const string& key) {
        if (debugMode) {
            cout << "\n[DEBUG] REMOVE: " << key << endl;
        }
        
        // find vector
        int index = hash(key);

        if (debugMode) {
            cout << "[DEBUG] Hash of '" << key << "' = " << index << endl;
        }
        
        // search keeping track of previous pointer
        Node* current = table[index];
        Node* prev = nullptr;
        
        while (current != nullptr) {
            if (current->key == key) {
                // found, remove from family, never to see them again
                if (prev == nullptr) {
                    // remove the first node in chain
                    table[index] = current->next;
                } else {
                    // remove middle or last node
                    prev->next = current->next;
                }
                
                if (debugMode) {
                    cout << "[DEBUG] Removed " << key << " from slot " << index << endl;
                }
                // delete current and update count
                delete current;
                count--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        // not found ):
        if (debugMode) {
            cout << "[DEBUG] Key not found" << endl;
        }
        return false;
    }
    
    /*
        displays the hash table contents
        shows table capacity, item count, load factor, and each chain
    */
    void printTable() const {
        cout << "\n=== HASH TABLE CONTENTS ===" << endl;
        cout << "Capacity: " << table.size() << ", Items: " << count << endl;
        cout << "Load factor: " << (float)count / table.size() << endl;
        cout << "------------------------" << endl;
        
        for (size_t i = 0; i < table.size(); i++) {
            cout << "[" << i << "]: ";
            
            if (table[i] == nullptr) {
                cout << "EMPTY";
            } else {
                Node* current = table[i];
                while (current != nullptr) {
                    cout << "[" << current->key << ":" << current->value << "]";
                    if (current->next != nullptr) {
                        cout << " -> "; // the chain ooo
                    }
                    current = current->next;
                }
            }
            cout << endl;
        }
        cout << "============================\n" << endl;
    }
    
    // debug mode control
    void setDebug(bool debug) {
        debugMode = debug;
        cout << "Debug mode: " << (debugMode ? "ON" : "OFF") << endl;
    }
    
    bool getDebug() const {
        return debugMode;
    }
    
    // getter for testing
    int getCount() const {
        return count;
    }
    // ditto
    int getCapacity() const {
        return table.size();
    }
    
    /*
        removes all items from the table
        deletes all nodes and resets count to 0
    */
    void clear() {
        for (size_t i = 0; i < table.size(); i++) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            table[i] = nullptr;
        }
        count = 0;
    }
};

/*
    displays the options
    gives the interface for testing the hash table
*/
void showMenu() {
    cout << "\n======HASH TABLE MENU======" << endl;
    cout << "1. Insert/Update (PUT)" << endl;
    cout << "2. Search (GET)" << endl;
    cout << "3. Delete (REMOVE)" << endl;
    cout << "4. Print table" << endl;
    cout << "5. Toggle debug mode" << endl;
    cout << "6. Run test cases" << endl;
    cout << "7. Exit" << endl;
    cout << "Choice: ";
}

/*
    runs all tests needed by the assignment
    1. insert and search
    2. update existing key
    3. delete a key
    4. collision handling
    5. resize
*/
void runTests(HashTable& ht) {
    cout << "\n=====TEST CASES======" << endl;
    
    // turns debug back on
    bool originalDebug = ht.getDebug();
    ht.setDebug(true);
    
    // starts the test with a new table
    ht.clear();
    
    cout << "\n======TEST 1: Basic Insert and Search======" << endl;
    ht.put("apple", 10);
    ht.put("banana", 7);
    ht.put("cherry", 3);
    ht.printTable();

    cout << "\n======TEST 2: Update Existing Key======" << endl;
    ht.put("apple", 25);
    int appleVal;
    if (ht.get("apple", appleVal)) {
        cout << "apple value: " << appleVal << endl;
    } else {
        cout << "apple value: not found ):" << endl;
    }

    cout << "\n======TEST 3: Delete a Key=====" << endl;
    ht.removeKey("banana");
    int bananaVal;
    if (ht.get("banana", bananaVal)) {
        cout << "banana after delete: FOUND (be sad)" << endl;
    } else {
        cout << "banana after delete: not found (correct)" << endl;
    }

    cout << "\n--- TEST 4: Force Collisions ---" << endl;
    ht.put("bigbird", 15);
    ht.put("tinybird", 42);
    ht.put("widebird", 8);
    ht.put("thinbird", 12);
    ht.put("bluebird", 99);
    ht.put("redbird", 50);
    ht.put("reallycoolbird", 100);
    ht.printTable();

    cout << "\n--- TEST 5: Trigger Resize ---" << endl;
    cout << "Before resize - Capacity: " << ht.getCapacity()
         << ", Count: " << ht.getCount() << endl;

    ht.put("tiger", 200);
    ht.put("elephant", 300);
    ht.put("giraffe", 400);
    ht.put("weezer", 500);
    
    cout << "After adding more items - Capacity: " << ht.getCapacity()
         << ", Count: " << ht.getCount() << endl;
    ht.printTable();
    
    // sets debug back to what it was
    ht.setDebug(originalDebug);
}

/*
  starts the program
  gives the user options to test the table  
*/
int main() {
    cout << "======NEW HASH TABLE======" << endl;
    
    // debug starts as true
    HashTable ht(11, true);
    
    int choice;
    string key;
    int value;
    
    do {
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: //put
                cout << "Enter key: ";
                cin >> key;
                cout << "Enter value: ";
                cin >> value;
                ht.put(key, value);
                break;
                
            case 2: // get
                cout << "Enter key: ";
                cin >> key;
                {
                    int result;
                    if (ht.get(key, result)) {
                        cout << "Value: " << result << endl;
                    } else {
                        cout << "Key not found!" << endl;
                    }
                }
                break;
                
            case 3: // remove
                cout << "Enter key: ";
                cin >> key;
                if (ht.removeKey(key)) {
                    cout << "Key removed!" << endl;
                } else {
                    cout << "Key not found!" << endl;
                }
                break;
                
            case 4: // print
                ht.printTable();
                break;
                
            case 5: // toggle debug mode
                ht.setDebug(!ht.getDebug());
                break;
                
            case 6: // run tests
                runTests(ht);
                break;
                
            case 7: // exit
                cout << "Goodbye!" << endl;
                break;
                
            default:
                cout << "Invalid choice!" << endl;
        }
    } while (choice != 7);
    
    return 0;
}