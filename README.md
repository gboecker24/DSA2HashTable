# DSA2HashTable
Project1DSA2
Gavin Boecker

This project is meant to create a hash table data structure in C++. The hash table stores string keys and integer valeas and supports insertion, search, deletion, resizing, and the option to switch in and out of a debug mode. No built in hash table, dictionary, or map class was used.

The goal of this project was to understand how hashing works, how collisions are handled, and how resizing affects performance.

I chose separate chaining as my collision resolution stategy.

In my implementation, the hash table is a vector<Node*>
Each index in the vector is a bucket.
Each bucket stores a linked list of nodes. (chain)
When two keys hash to the same index, they are stored in the same linked list

I chose chaining as it seemed to be the simplest solution.
Chaining made more sense to me when researching it compared to open addressing.
The worst case time becomes O(n) only if many keys are hashed to the same bucket.

Hash Function
The hash function converts a string into an integer index using this line:
hashValue = hashValue * 31 + c;
hashValue = 0
For each character, the current value is multiplied by 31, then the ACII value of the character is added. 
The absolute value is taken. The number is the moduloed by the table size.
This makes the hash fit withing the table

31 was used because that's what String.hashCode() in Java uses.

Operations

Insert/Update:
1. Hashes the key
2. Searches the chain at that index
3. If found: Update the value
4. If not found: Insert at the head of the chain
5. Increase count
6. Ckeck load factor and resize if needed

Search:
1. Hashes the key
2. Travel chain at that index
3. If found: insert at head of chain
4. If not found: return false

Remove:
1. Hashes the key
2. Travel chain at that index, keeping track if the previous node
3. Remove node if found
4. Decrement count

Print:
Displays capacity, item count, load facor, and all chains

Debug Mode:
Makes info appear about what is happening in the background if turned on
Can be toggled

Test Cases:
Does:
1. Insert and search
2. updates an existing key
3. removes a key
4. collison handling
5. resizing when load is over .75

Resizing and Load Factor:

Starts off with size 11
The load factor is calculator is calculated as: loadFactor = count / table.size()
Whenever this is greater than or equal to .75 the table size doubles.
All old elements are rehashed into the new table
Count is checked to make sure nothing is missed.
This has a time complexity of O(1)

Time complexities:

Insert: Best/Average = O(1) Worst = O(n)
Search: Best/Average = O(1) Worst = O(n)
Remove: Best/Average = O(1) Worst = O(n)
Resizing helps keep time complexity low

Limitations/Bugs:

Putting a word instead of a number for the value will make the program enter a forever loop.
The table doesn't shrink if items are removed.

AI Assistance:
AI assisted with debugging and helping me understand hash function design.
