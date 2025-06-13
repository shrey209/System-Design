#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <functional>

using namespace std;

const int WINDOW_SIZE = 256;
const int MIN_CHUNK = 8 * 1024;     // 8 KB
const int AVG_CHUNK = 32 * 1024;    // 32 KB
const int MAX_CHUNK = 64 * 1024;    // 64 KB
const uint32_t CHUNK_MASK = AVG_CHUNK - 1;

class RabinFingerprint {
    static const int WINDOW = WINDOW_SIZE;
    uint8_t window[WINDOW];
    int pos;
    uint64_t hash;

public:
    RabinFingerprint() : pos(0), hash(0) {
        fill(begin(window), end(window), 0);
    }

    void reset() {
        pos = 0;
        hash = 0;
        fill(begin(window), end(window), 0);
    }

    void slide(uint8_t byte) {
        uint8_t out = window[pos];
        window[pos] = byte;
        pos = (pos + 1) % WINDOW;
        hash = ((hash << 1) ^ byte) ^ (out << 3);
    }

    uint64_t getHash() const {
        return hash;
    }
};

// Temporary dummy hash using std::hash
string dummyHash(const vector<uint8_t>& data) {
    string s(data.begin(), data.end());
    size_t h = hash<string>{}(s);
    stringstream ss;
    ss << hex << h;
    return ss.str();
}

int main() {
    ifstream file("file2.txt", ios::binary);
    if (!file) {
        cerr << "❌ Error: Could not open file1.pdf\n";
        return 1;
    }

    RabinFingerprint rf;
    size_t byteIndex = 0;
    size_t chunkStart = 0;
    size_t currentSize = 0;
    size_t totalChunks = 0;

    vector<uint8_t> chunkData;
    char ch;

    while (file.get(ch)) {
        uint8_t byte = static_cast<uint8_t>(ch);
        chunkData.push_back(byte);
        rf.slide(byte);
        currentSize++;
        byteIndex++;

        bool isBoundary = (rf.getHash() & CHUNK_MASK) == 0;

        if ((currentSize >= MIN_CHUNK && isBoundary) || currentSize >= MAX_CHUNK) {
            string hash = dummyHash(chunkData);
            cout << "📦 Chunk #" << totalChunks + 1 << ": " << chunkStart << " - " << (byteIndex - 1)
                 << " | Size: " << currentSize << " bytes"
                 << " | Hash: " << hash << endl;

            chunkStart = byteIndex;
            currentSize = 0;
            rf.reset();
            chunkData.clear();
            totalChunks++;
        }
    }

    // Remaining chunk
    if (!chunkData.empty()) {
        string hash = dummyHash(chunkData);
        cout << "📦 Chunk #" << totalChunks + 1 << ": " << chunkStart << " - " << (byteIndex - 1)
             << " | Size: " << currentSize << " bytes"
             << " | Hash: " << hash << endl;
        totalChunks++;
    }

    cout << "\n✅ Total Chunks Created: " << totalChunks << endl;

    return 0;
}
