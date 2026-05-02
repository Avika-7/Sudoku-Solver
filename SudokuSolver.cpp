#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <string> // Required for to_string
#include <random> // For random number generation
#include <chrono> // For seeding the generator

using namespace std;

// Define the size of the Sudoku board
const int N = 9;

// Global RNG and distribution for use in randomizing value selection
// Seed the generator using the current time to get different results each run
static default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());


// --- 2.1 Constraint Checking and Domain Calculation ---

/**
 * @brief Calculates the domain: the set of legal numbers that can be placed at (row, col).
 */
vector<int> getPossibleValues(const vector<vector<int>>& board, int row, int col) {
    // Start with a full domain {1, 2, ..., 9}
    set<int> domain = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Eliminate conflicts from row, column, and 3x3 box
    for (int c = 0; c < N; ++c) if (board[row][c] != 0) domain.erase(board[row][c]);
    for (int r = 0; r < N; ++r) if (board[r][col] != 0) domain.erase(board[r][col]);

    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[startRow + i][startCol + j] != 0) {
                domain.erase(board[startRow + i][startCol + j]);
            }
        }
    }

    return vector<int>(domain.begin(), domain.end());
}


// --- 2.2 MRV Heuristic Implementation ---

/**
 * @brief Implements the Minimum Remaining Value (MRV) heuristic.
 * Finds the coordinates (row, col) of the empty cell with the minimum domain size.
 */
bool findMRVCell(const vector<vector<int>>& board, int& row, int& col) {
    int minOptions = N + 1; 
    bool foundEmpty = false;

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (board[r][c] == 0) {
                foundEmpty = true;
                
                vector<int> options = getPossibleValues(board, r, c);
                int numOptions = options.size();

                if (numOptions < minOptions) {
                    minOptions = numOptions;
                    row = r;
                    col = c;
                    
                    if (minOptions == 0) return true;
                }
            }
        }
    }
    return foundEmpty;
}


// --- 2.3 Main Backtracking Solver ---

/**
 * @brief The recursive backtracking function guided by the MRV heuristic.
 */
bool solveSudokuMRV(vector<vector<int>>& board) {
    int row, col;

    // 1. Variable Selection (MRV)
    if (!findMRVCell(board, row, col)) {
        return true; // Base Case: Solved.
    }

    vector<int> possibleValues = getPossibleValues(board, row, col);
    
    if (possibleValues.empty()) {
        return false; // Dead end.
    }

    // 2. Value Assignment and Recursion
    for (int num : possibleValues) {
        
        board[row][col] = num; // Try assignment
        
        if (solveSudokuMRV(board)) {
            return true; // Solution found!
        }
        
        board[row][col] = 0; // Backtrack: Reset cell
    }
    
    return false; // Failure
}


// --- 2.4 Board Generation Functions ---

/**
 * @brief Simple backtracking solver to fill an empty board completely (randomly).
 */
bool fillRandomBoard(vector<vector<int>>& board) {
    int row = -1, col = -1;
    
    // Find the first empty cell
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (board[r][c] == 0) {
                row = r;
                col = c;
                break;
            }
        }
        if (row != -1) break;
    }

    if (row == -1) {
        return true; // Base Case: Board is full
    }

    vector<int> possibleValues = getPossibleValues(board, row, col);
    
    // Shuffle the values to ensure a random valid board is generated
    shuffle(possibleValues.begin(), possibleValues.end(), generator);
    
    // Recursive Assignment
    for (int num : possibleValues) {
        board[row][col] = num;
        
        if (fillRandomBoard(board)) {
            return true;
        }
        
        board[row][col] = 0; // Backtrack
    }
    
    return false;
}

/**
 * @brief Creates a Sudoku puzzle by removing 'k' numbers from a full board.
 */
void generatePuzzle(vector<vector<int>>& board, int k) {
    int cellsToRemove = k;
    
    vector<int> cellIndices(N * N);
    for (int i = 0; i < N * N; ++i) {
        cellIndices[i] = i;
    }

    shuffle(cellIndices.begin(), cellIndices.end(), generator);

    for (int i = 0; i < min((int)cellIndices.size(), cellsToRemove); ++i) {
        int index = cellIndices[i];
        int r = index / N;
        int c = index % N;
        
        board[r][c] = 0;
    }
    for (int i = 0; i < N; ++i) {
        // MODIFICATION: Added one hyphen to make the horizontal line 22 characters long
        if (i % 3 == 0 && i != 0) cout << "----------------------" << endl; 
        for (int j = 0; j < N; ++j) {
            if (j % 3 == 0 && j != 0) cout << "| ";
            cout << (board[i][j] == 0 ? "." : to_string(board[i][j])) << " ";
        }
        cout << endl;
    }
}


// --- 2.5 Utility and Main Function (Modified for Clear Output) ---

void printBoard(const vector<vector<int>>& board) {
    for (int i = 0; i < N; ++i) {
        if (i % 3 == 0 && i != 0) cout << "----------------------" << endl; 
        for (int j = 0; j < N; ++j) {
            if (j % 3 == 0 && j != 0) cout << "| ";
            // Use '.' for empty cells (0)
            cout << (board[i][j] == 0 ? "." : to_string(board[i][j])) << " ";
        }
        cout << endl;
    }
}

int main() {
    // 1. Initialize an empty board
    vector<vector<int>> puzzle(N, vector<int>(N, 0));
    
    // The number of cells to leave empty. 55 is a good target for a challenging puzzle (26 clues).
    const int EMPTY_CELLS = 55; 

    cout << "--- Sudoku Solver Mini-Project ---" << endl;
    cout << "\nStarting generation of a random solvable puzzle..." << endl;
    
    // 2. Generate a full, solved board
    if (!fillRandomBoard(puzzle)) {
        cout << "\nError: Could not generate a full board." << endl;
        return 1;
    }
    
    // 3. Create the puzzle by removing numbers
    generatePuzzle(puzzle, EMPTY_CELLS); 

    // -----------------------------------------------------------------
    // --- DISPLAY 1: THE UNSOLVED PUZZLE STATE ---
    // -----------------------------------------------------------------
    
    cout << "\n--- UNSOLVED PUZZLE (" << EMPTY_CELLS << " empty cells) ---" << endl;
    printBoard(puzzle);

    // --- START SOLVER ---
    
    cout << "\n--- STARTING MRV BACKTRACKING SOLVER ---\n" << endl;
    
    // The solveSudokuMRV function attempts to solve the puzzle in-place
    if (solveSudokuMRV(puzzle)) {
        
        // -----------------------------------------------------------------
        // --- DISPLAY 2: THE SOLVED PUZZLE STATE ---
        // -----------------------------------------------------------------
        cout << "--- SOLUTION FOUND (MRV-Guided) ---" << endl;
        printBoard(puzzle);
    } else {
        cout << "\nError: The generated puzzle could not be solved." << endl;
    }

    return 0;
}