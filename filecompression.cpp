#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <bitset>

using namespace std;

// A Tree node
struct Node {
    char ch;
    int freq;
    Node *left, *right;
};

// Function to allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right) {
    Node* node = new Node();
    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

// Comparison object to be used to order the heap
struct comp {
    bool operator()(Node* l, Node* r) {
        // highest priority item has lowest frequency
        return l->freq > r->freq;
    }
};

// Traverse the Huffman Tree and store Huffman Codes in a map
void encode(Node* root, string str, unordered_map<char, string> &huffmanCode) {
    if (root == nullptr)
        return;

    // Found a leaf node
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

// Traverse the Huffman Tree and decode the encoded string
void decode(Node* root, int &index, const string &binaryString, ofstream &outputFile) {
    Node* currentNode = root;
    while (index < (int)binaryString.size()) {
        if (binaryString[index] == '0') {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }

        if (!currentNode->left && !currentNode->right) {
            outputFile << currentNode->ch;
            currentNode = root;
        }

        index++;
    }
}

// Write the binary encoded string to a file
void writeBinaryFile(const string &encodedString, const string &filename) {
    ofstream outFile(filename, ios::binary);
    bitset<8> bits;
    int bitIndex = 0;

    for (char ch : encodedString) {
        bits[7 - bitIndex] = ch - '0';
        bitIndex++;

        if (bitIndex == 8) {
            outFile.put(static_cast<unsigned char>(bits.to_ulong()));
            bitIndex = 0;
        }
    }

    // If there are leftover bits, write them as well
    if (bitIndex != 0) {
        outFile.put(static_cast<unsigned char>(bits.to_ulong()));
    }

    outFile.close();
}

// Function to read binary file and convert to binary string
string readBinaryFile(const string &filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Error: Could not open the encoded file." << endl;
        return "";
    }

    string binaryString = "";
    unsigned char byte;
    while (inFile.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        bitset<8> bits(byte);
        binaryString += bits.to_string();
    }

    inFile.close();
    return binaryString;
}

// Builds Huffman Tree and decodes given input text
void buildHuffmanTree(const string &inputFileName, const string &encodedFileName, const string &decodedFileName) {
    // Read input from the file
    ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open the input file." << endl;
        return;
    }

    string text((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    // Count frequency of appearance of each character and store it in a map
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }

    // Create a priority queue to store live nodes of Huffman tree
    priority_queue<Node*, vector<Node*>, comp> pq;

    // Create a leaf node for each character and add it to the priority queue
    for (auto pair : freq) {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    // Do until there's more than one node in the queue
    while (pq.size() != 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();

        // Create a new internal node with these two nodes as children
        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }

    // Root stores pointer to root of Huffman Tree
    Node* root = pq.top();

    // Traverse the Huffman Tree and store Huffman Codes in a map
    unordered_map<char, string> huffmanCode;
    encode(root, "", huffmanCode);

    // Print Huffman Codes
    cout << "Huffman Codes are:\n";
    for (auto pair : huffmanCode) {
        cout << pair.first << " " << pair.second << '\n';
    }

    // Encode the input string
    string encodedString = "";
    for (char ch : text) {
        encodedString += huffmanCode[ch];
    }

    // Write the binary encoded string to the output file
    writeBinaryFile(encodedString, encodedFileName);
    cout << "\nEncoded binary string saved to " << encodedFileName << endl;

    // Read binary encoded string from file
    string binaryString = readBinaryFile(encodedFileName);

    // Decode the encoded string and write to the decoded file
    ofstream decodedFile(decodedFileName);
    if (!decodedFile.is_open()) {
        cerr << "Error: Could not open the decoded file." << endl;
        return;
    }

    int index = 0;
    decode(root, index, binaryString, decodedFile);
    decodedFile.close();

    cout << "Decoded string saved to " << decodedFileName << endl;
}

// CLI Interface Function
void runCLI() {
    string inputFileName, encodedFileName, decodedFileName;

    cout << "Enter the input file name: ";
    cin >> inputFileName;

    cout << "Enter the output file name for the encoded binary string: ";
    cin >> encodedFileName;

    cout << "Enter the output file name for the decoded string: ";
    cin >> decodedFileName;

    buildHuffmanTree(inputFileName, encodedFileName, decodedFileName);
}

int main() {
    runCLI();
    return 0;
}
