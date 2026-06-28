#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
#include <chrono>
#include <random>
#include <thread>
#include <algorithm>

#include "renderer.h"

#include <unistd.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>

#include <string>
#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#include <mmsystem.h>
#endif

#define CHARACTER_COUNT 256
#define STEP 4

#define LIST_ID(NAME) int lId = static_cast<int>(film[instructions[currentInstruction + 1]]); \
    if (lId < 0 || lId >= static_cast<int>(matrix.size())) \
    { \
        fprintf(stderr, "\n" NAME ": Matrix Error: list index %d out of bounds (have %zu lists)!\n", lId, matrix.size()); \
        if (debugFile) {\
            fprintf(stderr, "%s\nCode: %s\n", \
            lines.at((currentInstruction / STEP) * 2).c_str(), \
            lines.at((currentInstruction / STEP) * 2 + 1).c_str()); \
        }   \
        exitCode = -1; \
        goto _final; \
    }
#define ELEMENT_ID(NAME) int idx = static_cast<int>(film[instructions[currentInstruction + 2]]); \
    if (idx < 0 || idx >= static_cast<int>(matrix[lId].size())) \
    { \
        fprintf(stderr, "\n" NAME ": Matrix Error: index %d out of bounds for list %d (size %zu)!\n", idx, lId, matrix[lId].size()); \
if (debugFile) {\
fprintf(stderr, "%s\nCode: %s\n", \
lines.at((currentInstruction / STEP) * 2).c_str(), \
lines.at((currentInstruction / STEP) * 2 + 1).c_str()); \
}   \
        exitCode = -1; \
        goto _final; \
    }

#include <cstdio>

std::vector<char> printBuffer;

std::vector<std::vector<double>> matrix; // Escape the Matrix

int instructionCount = 0;
int exitCode = 0;

double modulo(const double num, const double base) {
    const double r = std::fmod(num, base);
    if (std::abs(r) < std::abs(base) * 1e-9) return 0.0;
    return (r < 0) != (base < 0) ? r + base : r;
}

char doubleToChar(const double number)
{
    const int index = static_cast<int>(modulo(number, CHARACTER_COUNT));
    return static_cast<char>(index);
}

std::string doubleVecToString(const std::vector<double>& vec)
{
    std::string result;
    for (const double d : vec)
    {
        result += doubleToChar(d);
    }
    return result;
}

void readFile(const std::vector<double>& filename, std::vector<double>& chars)
{
    std::string name = doubleVecToString(filename);
    std::ifstream file(name);
    if (!file)
    {
        fprintf(stderr, "IO Error: failed to open file for reading: %s!\n", name.c_str());
        chars.push_back(0.0);
        return;
    }
    char c;
    while (file.get(c))
    {
        chars.push_back(static_cast<double>(static_cast<unsigned char>(c)));
    }
}

void store(const std::vector<double>& filename, const std::vector<double>& vec)
{
    std::string name = doubleVecToString(filename);
    std::ofstream file(name);
    if (!file)
    {
        fprintf(stderr, "IO Error: failed to open file for writing: %s!\n", name.c_str());
        return;
    }
    for (const double d : vec)
    {
        file << doubleToChar(d);
    }
    if (!file)
    {
        fprintf(stderr, "IO Error: write failed for file: %s!\n", name.c_str());
    }
}

std::string doubleVectorToString(const std::vector<double>& input)
{
    std::string result;
    result.reserve(input.size());

    for (const double d : input)
    {
        result += doubleToChar(d);
    }

    return result;
}

char* getStringFrom(const int start, const int end, const double * film)
{
    char* result = new char[end - start + 1]();
    for (int i = start; i < end; i++)
    {
        result[i - start] = doubleToChar(film[i]);
    }
    result[end - start] = '\0';


    return result;
}

void addToBuffer(const char c)
{
    printBuffer.push_back(c);
}

void addToBuffer(const std::string& s)
{
    printBuffer.insert(printBuffer.end(), s.begin(), s.end());
}

void addDoubleToBuffer(const double value, const int precision = 2)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    addToBuffer(oss.str());
}

constexpr int opp_amount = 66;

int getIntFrom(const unsigned char* buff, const int start)
{
    const int value = *reinterpret_cast<const int*>(&buff[start]);

    return value;
}

double getDoubleFrom(const unsigned char* buff, const int start)
{
  const double value = *reinterpret_cast<const double*>(&buff[start]);

    return value;
}

void onCrash(const int sig) {
  auto name = "UNKNOWN";
    switch (sig) {
        case SIGSEGV: name = "SIGSEGV"; break;
        case SIGABRT: name = "SIGABRT"; break;
        case SIGFPE:  name = "SIGFPE";  break;
        case SIGILL:  name = "SIGILL";  break;
        default: "UNKNOWN";
    }

    char buf[64];
#ifdef _WIN32
    int len = snprintf(buf, sizeof(buf), "Caught signal: %s\n", name);
    fwrite(buf, 1, len, stdout);
#else
    const int len = snprintf(buf, sizeof(buf), "Caught signal: %s\n", name);
    write(STDOUT_FILENO, buf, len);
#endif
    _exit(1);
}

int main(int argc, char* argv[])
{
    signal(SIGSEGV, onCrash);
    signal(SIGABRT, onCrash);
    signal(SIGFPE,  onCrash);
    signal(SIGILL,  onCrash);

    bool debugFile = false;
    std::vector<std::string> lines;

    {
        std::filesystem::path p = argv[1];
        p.replace_extension(".fsdbi");

        std::ifstream file(p);
        if (file.good()) {
            debugFile = true;
            std::string line;
            while (std::getline(file, line))
                lines.push_back(line);
        }
    }

    double* __restrict__ film;
    int* __restrict__ instructions;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    std::random_device random_device_shuffle;
    std::mt19937 g_shuffle(random_device_shuffle());

    if (argc < 2)
    {
        std::cerr << "Usage: runner PATH_TO_LOAD_FROM\n";
        return 1;
    }

    const char* path = argv[1];

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> vec(fileSize);
    if (!file.read(reinterpret_cast<char*>(vec.data()), fileSize))
    {
        std::cerr << "Failed to read file\n";
        return 1;
    }
    file.close();

    auto* buffer = new unsigned char[vec.size()];
    std::memcpy(buffer, vec.data(), vec.size());

    unsigned int bufferSize = vec.size();

    int mChars = 0;
    int startMargin = 0;

    for (int i = 4; i < bufferSize; i++)
    {
        if (buffer[i] == 255)
        {
            mChars++;
        }
        else
        {
            mChars = 0;
        }

        if (mChars == 64)
        {
            startMargin = i - 63;
            break;
        }
    }

    if (startMargin == 0)
    {
        fprintf(stderr, "Error reading file! No margin found!\n");
        return -1;
    }

    int length = getIntFrom(buffer, 0);
    film = new double[length]();

    for (int i = 4; i < startMargin; i += 12)
    {
        int id = getIntFrom(buffer, i);
        double value = getDoubleFrom(buffer, i + 4);

        film[id] = value;
    }

    const int instructionAmount = static_cast<int>(bufferSize) - startMargin - 64;
    if (instructionAmount % 16 != 0)
    {
        fprintf(stderr, "Error reading file!\n");
        return -1;
    }

    instructions = new int[instructionAmount / 4]{};

    int c = 0;
    for (int i = startMargin + 64; i < bufferSize; i += 16)
    {
        for (int b = 0; b < 4; b++)
        {
            instructions[c] = getIntFrom(buffer, i + b * 4);
            c++;
        }
    }

    delete[] buffer;

    instructionCount = instructionAmount / 4;

    bool graphicalMode = false;

    for (int i = 0; i < instructionCount / 4; i++)
    {
        int opp = instructions[i * 4];
        if (opp > 51 && opp < 66)
        {
            graphicalMode = true;
            break;
        }
    }

    if (graphicalMode)
    {
        InitGraphics();
    }

    #ifdef _WIN32
    timeBeginPeriod(1);
    #endif

    auto start = std::chrono::high_resolution_clock::now();

    static const void* labels[] = {
        &&_copy, &&_add, &&_subtract, &&_multiply, &&_divide,
        &&_squareRoot, &&_sin, &&_cos, &&_tan, &&_asin,
        &&_acos, &&_atan2, &&_pow, &&_mod, &&_equal,
        &&_compare, &&_notOp, &&_andOp, &&_orOp, &&_jumpIf,
        &&_print, &&_printNumbers, &&_iterate, &&_copyFrom, &&_pointer,
        &&_position, &&_jump, &&_end, &&_floor, &&_round,
        &&_ceil, &&_time, &&_newList, &&_copyFromList, &&_lengthOfList,
        &&_listAmount, &&_addList, &&_removeAt, &&_emptyList, &&_removeAll,
        &&_addAt, &&_reverseList, &&_shuffleList, &&_opp_sort, &&_sleep,
        &&_random, &&_input, &&_listSet, &&_updateConsole, &&_printVector,
        &&_printVectorNumbers, &&_printVectorSeparated, &&_oppSetNewColor, &&_oppSetPosition, &&_oppSetTextSize,
        &&_oppDrawTriangles, &&_oppDrawText, &&_oppBeginDrawing, &&_oppEndDrawing, &&_oppIsPressed,
        &&_mousePressed, &&_getMouseX, &&_getMouseY, &&_getScroll, &&_getWidth,
        &&_getHeight, &&_load, &&_store
    };

    int currentInstruction = 0;

    goto *labels[instructions[currentInstruction]];

    //region Labels
    _copy:
    film[instructions[currentInstruction + 2]] = film[instructions[currentInstruction + 1]];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _add:
    film[instructions[currentInstruction + 3]] = film[instructions[currentInstruction + 1]] + film[instructions[currentInstruction + 2]];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _subtract:
    film[instructions[currentInstruction + 3]] = film[instructions[currentInstruction + 1]] - film[instructions[currentInstruction + 2]];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _multiply:
    film[instructions[currentInstruction + 3]] = film[instructions[currentInstruction + 1]] * film[instructions[currentInstruction + 2]];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _divide:
    film[instructions[currentInstruction + 3]] = film[instructions[currentInstruction + 1]] / film[instructions[currentInstruction + 2]];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _squareRoot:
    film[instructions[currentInstruction + 2]] = sqrt(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _sin:
    film[instructions[currentInstruction + 2]] = sin(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _cos:
    film[instructions[currentInstruction + 2]] = cos(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _tan:
    film[instructions[currentInstruction + 2]] = tan(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _asin:
    film[instructions[currentInstruction + 2]] = asin(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _acos:
    film[instructions[currentInstruction + 2]] = acos(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _atan2:
    film[instructions[currentInstruction + 3]] = atan2(film[instructions[currentInstruction + 1]], film[instructions[currentInstruction + 2]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _pow:
    {
        double base = film[instructions[currentInstruction + 1]];
        double exponent = film[instructions[currentInstruction + 2]];

        double result;

        if (base >= 0)
        {
            result = std::pow(base, exponent);
        }
        else
        {
            long long n = std::llround(1.0 / exponent);
            bool is_unit = std::abs(1.0 / n - exponent) < 1e-9;

            if (is_unit && n % 2 != 0)
            {
                result = -std::pow(-base, exponent);
            }
            else
            {
                result = std::pow(-base, exponent);
            }
        }

        film[instructions[currentInstruction + 3]] = result;
    }
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _mod:
    film[instructions[currentInstruction + 3]] = modulo(film[instructions[currentInstruction + 1]], film[instructions[currentInstruction + 2]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _equal:
    film[instructions[currentInstruction + 3]] = film[instructions[currentInstruction + 1]] == film[instructions[currentInstruction + 2]];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _compare:
    film[instructions[currentInstruction + 3]] = film[instructions[currentInstruction + 1]] > film[instructions[currentInstruction + 2]];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _notOp:
    film[instructions[currentInstruction + 2]] = (film[instructions[currentInstruction + 1]] == 0) ? 1 : 0;
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _andOp:
    film[instructions[currentInstruction + 3]] = (film[instructions[currentInstruction + 1]] != 0 && film[instructions[currentInstruction + 2]] != 0.0) ? 1.0 : 0.0;
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _orOp:
    film[instructions[currentInstruction + 3]] = (film[instructions[currentInstruction + 1]] != 0.0 || film[instructions[currentInstruction + 2]] != 0.0) ? 1.0 : 0.0;
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _jumpIf:
    currentInstruction = (film[instructions[currentInstruction + 1]] != 0) ?
    static_cast<int>(film[instructions[currentInstruction + 2]]) * 4 : currentInstruction + STEP;
    goto *labels[instructions[currentInstruction]];

    _print:
    {
        char* toPrint = getStringFrom(instructions[currentInstruction + 1], instructions[currentInstruction + 1] + instructions[currentInstruction + 2], film);
        addToBuffer(toPrint);
        delete[] toPrint;
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }
    _printNumbers:
    for (int i = instructions[currentInstruction + 1]; i < (instructions[currentInstruction + 1] + instructions[currentInstruction + 2]); i++)
    {
        addDoubleToBuffer(film[i]);
        if (i < (instructions[currentInstruction + 1] + instructions[currentInstruction + 2]) - 1) addToBuffer(" ");
    }
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _iterate:
    film[instructions[currentInstruction + 1]] += 1;
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _copyFrom:
    film[instructions[currentInstruction + 2]] = film[static_cast<int>(film[instructions[currentInstruction + 1]])];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _pointer:
    film[instructions[currentInstruction + 2]] = instructions[currentInstruction + 1];
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _position:
    film[instructions[currentInstruction + 1]] = currentInstruction;
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _jump:
    currentInstruction = static_cast<int>(film[instructions[currentInstruction + 1]]) * 4;
    goto *labels[instructions[currentInstruction]];

    _end:
    exitCode = static_cast<int>(film[instructions[currentInstruction + 1]]);
    goto _final;

    _floor:
    film[instructions[currentInstruction + 2]] = std::floor(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _round:
    film[instructions[currentInstruction + 2]] = std::round(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _ceil:
    film[instructions[currentInstruction + 2]] = std::ceil(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _time:
    {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        film[instructions[currentInstruction + 1]] = static_cast<double>(ms);
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];

    }
    _newList:
    matrix.emplace_back();
    film[instructions[currentInstruction + 1]] = static_cast<double>(matrix.size() - 1);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _copyFromList:
    {
        LIST_ID("lget")
        ELEMENT_ID("lget")
        film[instructions[currentInstruction + 3]] = matrix[lId][idx];
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _lengthOfList:
    {
        LIST_ID("llen")
        film[instructions[currentInstruction + 2]] = static_cast<double>(matrix[lId].size());
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _listAmount:
    film[instructions[currentInstruction + 1]] = static_cast<double>(matrix.size());
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _addList:
    {
        LIST_ID("ladd")
        matrix[lId].push_back(film[instructions[currentInstruction + 2]]);
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _removeAt:
    {
        LIST_ID("lremove")
        ELEMENT_ID("lremove")
        matrix[lId].erase(matrix[lId].begin() + idx);
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }
    _emptyList:
    {
        LIST_ID("lempty")
        matrix[lId].clear();
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _removeAll:
    matrix.clear();
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _addAt:
    {
        LIST_ID("linsert")
        int index = static_cast<int>(film[instructions[currentInstruction + 3]]);
        int value = static_cast<int>(film[instructions[currentInstruction + 2]]);

        if (index < 0) index = 0;
        if (index > static_cast<int>(matrix[lId].size())) index = static_cast<int>(matrix[lId].size());
        matrix[lId].insert(matrix[lId].begin() + index, value);
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }
    _reverseList:
    {
        LIST_ID("lreverse")
        std::reverse(matrix[lId].begin(), matrix[lId].end());
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _shuffleList:
    {
        LIST_ID("lshuffle")
        std::shuffle(matrix[lId].begin(), matrix[lId].end(), g_shuffle);
        currentInstruction += STEP;
    }
    goto *labels[instructions[currentInstruction]];

    _opp_sort:
    {
        LIST_ID("lsort")
        if (film[instructions[currentInstruction + 2]] == 0)
            std::sort(matrix[lId].begin(), matrix[lId].end());
        else
            std::sort(matrix[lId].begin(), matrix[lId].end(), std::greater<>());
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }
    _sleep:
    {
        int ms = static_cast<int>(film[instructions[currentInstruction + 1]]);

        #ifdef _WIN32
        auto target = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(ms);

        if (ms > 2)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms - 2));
        }
        while (std::chrono::high_resolution_clock::now() < target)
        {
            std::this_thread::yield();
        }
        #else
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        #endif

        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _random:
    {
        film[instructions[currentInstruction + 1]] = dis(gen);
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];

    }

    _input:
    {
        LIST_ID("linput")

        string line;
        std::getline(cin, line);

        size_t len = line.length();
        const char *str = line.c_str();

        for (size_t i = 0; i < len; ++i) {
            matrix[lId].push_back(static_cast<double>(str[i]));
        }
        matrix[lId].push_back(10.0);
    }
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _listSet:
    {
        LIST_ID("lset")
        ELEMENT_ID("lset")
        matrix[lId][idx] = film[instructions[currentInstruction + 3]];
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _updateConsole:
    if (!printBuffer.empty())
    {
        std::cout.write(printBuffer.data(), static_cast<long>(printBuffer.size()));
        std::cout.flush();
        printBuffer.clear();
    }
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _printVector:
    {
        LIST_ID("lprint")
        for (double d : matrix[lId]) addToBuffer(doubleToChar(d));
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _printVectorNumbers:
    {
        LIST_ID("lprintNum")

        addToBuffer("[");
        const std::vector<double>& vector = matrix[lId];
        for (size_t i = 0; i < vector.size(); ++i)
        {
            addDoubleToBuffer(vector[i], 2);
            if (i + 1 != vector.size()) addToBuffer(", ");
        }
        addToBuffer("]");
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }
    _printVectorSeparated:
    {
        LIST_ID("lprintSep")

        int separateEach = static_cast<int>(film[instructions[currentInstruction + 3]]);
        if (separateEach <= 0)
        {
            fprintf(stderr, "lprintSep: sep must be > 0, got %d!\n", separateEach);
            exitCode = -1;
            goto _final;
        }

        const std::vector<double>& vector = matrix[lId];
        char separator = doubleToChar(static_cast<int>(film[instructions[currentInstruction + 2]]));
        for (size_t i = 0; i < vector.size(); ++i)
        {
            addToBuffer(doubleToChar(vector[i]));
            if ((i + 1) % separateEach == 0 && i + 1 != vector.size()) addToBuffer(separator);
        }
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];

    }
    _oppSetNewColor:
    setNewColor(film[instructions[currentInstruction + 1]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _oppSetPosition:
    setCursorPosition(film[instructions[currentInstruction + 1]], film[instructions[currentInstruction + 2]]);
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _oppSetTextSize:
    setTextSize(
          static_cast<float>(film[instructions[currentInstruction + 1]]));
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _oppDrawTriangles:
    {
        LIST_ID("ldrawTriangles")
        drawTriangles(matrix[lId]);
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _oppDrawText:
    {
        LIST_ID("ldrawText")
        drawText(doubleVectorToString(matrix[lId]));
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _oppBeginDrawing:
    if (WindowShouldClose())
    {
        exitCode = 1;
        goto _final;
    }

    beginDrawing();
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _oppEndDrawing:
    endDrawing();
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _oppIsPressed:
    film[instructions[currentInstruction + 2]] = isPressed(static_cast<int>(film[instructions[currentInstruction + 1]]));
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _mousePressed:
    switch (static_cast<int>(film[instructions[currentInstruction + 1]]))
    {
        case 0: film[instructions[currentInstruction + 2]] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        break;
        case 1: film[instructions[currentInstruction + 2]] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
        break;
        case 2: film[instructions[currentInstruction + 2]] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);
        break;
        default: cerr << "Unknown mouse button: " << static_cast<int>(film[instructions[currentInstruction + 1]]) << endl;
    }
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _getMouseX:
    film[instructions[currentInstruction + 1]] = GetMousePosition().x;
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _getMouseY:
    film[instructions[currentInstruction + 1]] = GetMousePosition().y;
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _getScroll:
    film[instructions[currentInstruction + 1]] = GetMouseWheelMove();
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _getWidth:
    film[instructions[currentInstruction + 1]] = GetScreenWidth();
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _getHeight:
    film[instructions[currentInstruction + 1]] = GetScreenHeight();
    currentInstruction += STEP;
    goto *labels[instructions[currentInstruction]];

    _load:
    {
        int destId = static_cast<int>(film[instructions[currentInstruction + 1]]);
        int nameId = static_cast<int>(film[instructions[currentInstruction + 2]]);

        if (destId < 0 || destId >= static_cast<int>(matrix.size()))
        {
            fprintf(stderr, "\nlload: Matrix Error: destination list index %d out of bounds (have %zu lists)!\n", destId, matrix.size());
            if (debugFile) {
                fprintf(stderr, "%s\nCode: %s\n", \
                lines.at((currentInstruction / STEP) * 2).c_str(),
                lines.at((currentInstruction / STEP) * 2 + 1).c_str());
            }
            exitCode = -1;
            goto _final;
        }
        if (nameId < 0 || nameId >= static_cast<int>(matrix.size()))
        {
            fprintf(stderr, "\nlload: Matrix Error: filename list index %d out of bounds (have %zu lists)!\n", nameId, matrix.size());
            if (debugFile) {
                fprintf(stderr, "%s\nCode: %s\n",
                lines.at((currentInstruction / STEP) * 2).c_str(),
                lines.at((currentInstruction / STEP) * 2 + 1).c_str());
            }
            exitCode = -1;
            goto _final;
        }

        readFile(matrix[nameId], matrix[destId]);
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    _store:
    {
        int srcId = static_cast<int>(film[instructions[currentInstruction + 1]]);
        int nameId = static_cast<int>(film[instructions[currentInstruction + 2]]);

        if (srcId < 0 || srcId >= static_cast<int>(matrix.size()))
        {
            fprintf(stderr, "\nlstore: Matrix Error: data list index %d out of bounds (have %zu lists)!\n", srcId, matrix.size());
            if (debugFile) {
                fprintf(stderr, "%s\nCode: %s\n",
                lines.at((currentInstruction / STEP) * 2).c_str(),
                lines.at((currentInstruction / STEP) * 2 + 1).c_str());
            }
            exitCode = -1;
            goto _final;
        }
        if (nameId < 0 || nameId >= static_cast<int>(matrix.size()))
        {
            fprintf(stderr, "\nlstore: Matrix Error: filename list index %d out of bounds (have %zu lists)\n", nameId, matrix.size());
            if (debugFile) {
                fprintf(stderr, "%s\nCode: %s\n",
                lines.at((currentInstruction / STEP) * 2).c_str(),
                lines.at((currentInstruction / STEP) * 2 + 1).c_str());
            }
            exitCode = -1;
            goto _final;
        }

        store(matrix[nameId], matrix[srcId]);
        currentInstruction += STEP;
        goto *labels[instructions[currentInstruction]];
    }

    //endregion

    _final:
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    #ifdef _WIN32
    timeEndPeriod(1);
    #endif

    if (graphicalMode && IsWindowReady())
    {
        Cleanup();
    }

    std::cout << std::endl << "[Program finished in " << duration.count() << "ms with exit code: " << exitCode << "]" <<
    std::endl;

    return 0;
}
