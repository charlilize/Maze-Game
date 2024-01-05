/*
    * Name: Charlize Angeles, 5007982234, 1006, Assignment 7
    * Description: Go through a maze that Agent Jimmy must exit
    * Input: The maze file, maze input file, 
    * user input file, and which direction to go
    * Output: Game directions, menu, and two reconfigurations of the maze
*/

// g++ -std=c++11 main.cpp
// ./a.out maze1 maze1input

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// ===-----------------------------------------------------------------
// List of functions prototypes
bool startGame();
bool exitGame(bool enable);
bool processMazeFile(vector<vector<char>> &maze, char *filename,
int &start_room, int &finish_room, int &rowCount, int &colCount);
bool processInputFile(vector<char> &directions, char *filename);
void increaseMazeSize(vector<vector<char>> &maze, 
int numRows, int numCols);
void drawMaze(vector<vector<char>> &maze, int start_room, 
int finish_room, int rows, int columns);
void printGrid(vector<vector<char>> &maze, 
int startRoom, vector<int> &position);
int XYToIndex(int x, int y, int numCols); 
void indexToXY(int index, int num_cols, int &x, int &y);
bool inBounds(int x, int y, vector<vector<char>> &maze, char input);
bool moveThroughMaze(vector<vector<char>> &maze, 
int &x, int &y, char input);
void trackPathThroughMaze(vector<int> &position, 
int x, int y, int numCols);
bool retracePathThroughMaze(vector<int> &position, int numCols);
void printMaze(vector<vector<char>> &maze);

int main(int argc, char *argv[]) {
  if (argc > 2) {
    // VARIABLES
    int startRoom = 0, finishRoom = 0, rows = 0, columns = 0, size = 0;
    // winning message.
    const string exit_found_title =
    "    ______     _ __     ______                      ____\n"
    "   / ____/  __(_) /_   / ____/___  __  ______  ____/ / /\n"
    "  / __/ | |/_/ / __/  / /_  / __ \\/ / / / __ \\/ __  / /\n"
    " / /____>  </ / /_   / __/ / /_/ / /_/ / / / / /_/ /_/\n"
    "/_____/_/|_/_/\\__/  /_/    \\____/\\__,_/_/ /_/\\__,_(_)\n";
    // Represent the current coordinates
    int x = 0, y = 0;
    // Variable to store the returning values of the all functions
    bool returnValue = false;
    // Represents the maze & store the characters from the input file
    vector<vector<char>> maze;
    // The directions & store the characters from the input file
    vector<char> directions; 
    // Store the movements of Agent Jimmy 
    vector<int> shortestPath;

    // RUN THE INTRO OF THE GAME
    if (startGame() == false) {
      // Store whether the user wants to exit from the Game Over screen
      returnValue = exitGame(false);
      // If they want to exit, terminate the program
      if (returnValue == false) {
        cout << "\nDONE!";
        return 0;  
      }  
      // Result of choosing continue
      if (returnValue == true) {
        cout << "\nyou hit continue";
        return 0;
      } 
    }

    // START THE GAME
    // Iterate through each command line argument and open each file

    // Open the first maze file,
    // read in maze, draw the maze, and print the maze
    returnValue = processMazeFile(maze, argv[1], 
    startRoom, finishRoom, rows, columns);

    // If couldn't read the file, end the program
    if (returnValue == false) {
      return 0; 
    }

    // Open the second file and read in directions
    processInputFile(directions, argv[2]);

    // Increase the size of the maze
    increaseMazeSize(maze, rows, columns);

    // Redraw the larger maze with the right symbols
    drawMaze(maze, startRoom, finishRoom, rows, columns);

    // Print the second modified version of the maze
    // Update the size of the position vector
    size = rows * (columns) * 2;

    // Keep track of Agent Jimmy's position
    vector<int> position(size, 0); 
    printGrid(maze, startRoom, position);

    // Pass in starting coordinate
    x = startRoom; 

  // store where the other side of the maze's columns start
  int stepsCol = static_cast<int>(maze[0].size()) / 2;

    // To check if the exit has been found yet and for duplications
    bool valid = false;
    int xDup = 0, yDup = 0;
    bool foundExit = false;
    // Get ready to iterate through each direction in the vector
    for (int i = 0; i < static_cast<int>(directions.size()); i++) {
      // Move through the maze as long as exit has not been found
      valid = moveThroughMaze(maze, x, y, directions[i]);

      // Run as long as exit not found
      if (valid == false) {

        // Track where Agent Jimmy has been as long
        // as you're not next to the exit
        if (maze[x][(y + 1) + stepsCol] != 'X') {
          // Keep track of where Agent Jimmy has been
          trackPathThroughMaze(position, x, y, columns);
        } else if (maze[x][(y + 1) + stepsCol] == 'X') {
          foundExit = true;
        }

        // Print the maze
        printGrid(maze, startRoom, position);
        
        // if it is a duplicate, don't consider the coordinates
        if (xDup != x || yDup != y) {

          // Store the x, y of Agent Jimmy
          shortestPath.push_back(x);
          shortestPath.push_back(y);

      }

      // Store the coordinates for comparison
      xDup = x;
      yDup = y;

      }

    }

    // Output where Agent Jimmy was found
    cout << "\nINFO: Surveillance camera "
    << "found agent Jimmy at point(s):\n";
    retracePathThroughMaze(position, columns);

    if (foundExit == true) {
      // Output exit title
      cout << endl << exit_found_title;

      // Print the shortest path
      cout << "\nPossible shortest path to exit:\n";
      int counter = 0;

      // minus two to skip the duplicate ending coordinates
      while (counter < static_cast<int>(shortestPath.size())) {
        cout << "(" << shortestPath[counter] << 
        ", " << shortestPath[counter + 1] << ")\n";
        counter += 2;
      }
    } else {
      // Output Game Over if no exit found
      exitGame(false);
    }

  } else {
    cout << "ERROR: Invalid number of command-line arguments.\n"
         << "ERROR: Expecting 3 found " << argc << "." << endl;
  }
  return 0;
}

// ===----------------------------------
// Function definitions

// DISPLAYS THE GAME TITLE AND GIVES AN OPTION TO START THE GAME OR EXIT
bool startGame() {
    // Variables
    // title of the game that is used at the start of the game.
    const string header =
      "          __  ___     __        ________\n"
      "         /  |/  /__  / /_____ _/ / ____/__  ____ ______\n"
      "        / /|_/ / _ \\/ __/ __ `/ / / __/ _ \\/ __ `/ ___/\n"
      "       / /  / /  __/ /_/ /_/ / / /_/ /  __/ /_/ / /\n"
      "      /_/  /_/\\___/\\__/\\__,_/_/\\____/\\___/\\__,_/_/\n"
      "            / (_)___ ___  ____ ___  __  __\n"
      "       __  / / / __ `__ \\/ __ `__ \\/ / / /\n"
      "      / /_/ / / / / / / / / / / / / /_/ /\n"
      "      \\____/_/_/ /_/ /_/_/ /_/ /_/\\__, /\n"
      "                                 /____/\n"
      "                Rated B for boring.\n";

    // an array of the game menu
    string option[] = {"> EXIT", "NEW GAME", "EXIT", "> NEW GAME"};

    // Store the starting options to display the arrow and game menu
    string optionChoice1 = option[0], optionChoice2 = option[1];
    char choice = '\0';

    do {
        // display the game menu to the terminal
        cout << endl << header << endl 
        << optionChoice1 << endl << optionChoice2;

        // allow user to choose where to navigate through the menu
        cout << "\nMOVEMENT GUIDE: W (up), S (down), Q (select): ";
        cin >> choice;

        // if the user navigates up or down, move the arrow
        if (optionChoice1 == option[0] && optionChoice2 == option[1] 
        && (choice == 'S' || choice == 'W')) {
            optionChoice1 = option[2];
            optionChoice2 = option[3];
        } else if (optionChoice1 == option[2] 
        && optionChoice2 == option[3] 
        && (choice == 'S' || choice == 'W')) {
            optionChoice1 = option[0];
            optionChoice2 = option[1];
        }
    
    // keep the game menu looping until the user selects something
    } while(choice != 'Q');
    
    // Determine whether the user choice to EXIT or start a NEW GAME
    // user wanted to EXIT the game
    if (optionChoice1 == option[0] && optionChoice2 == option[1]) {
        return false;
    }
    // user wanted to start a new game
    return true;

}

// DISPLAYS THE HEADER WHEN THE USER CHOOSES TO EXIT
bool exitGame(bool enable) {
  // Variables
  // title of the game when the player loses or exits the game.
  const string exit_title =
      "       ______                        ____\n"
      "      / ____/___ _____ ___  ___     / __ \\_   _____  _____\n"
      "     / / __/ __ `/ __ `__ \\/ _ \\   / / / / | / / _ \\/ ___/\n"
      "    / /_/ / /_/ / / / / / /  __/  / /_/ /| |/ /  __/ /\n"
      "    \\____/\\__,_/_/ /_/ /_/\\___/   \\____/ |___/\\___/_/\n"
      "           What's wrong!?... Jimmy... Jimmmyyyyyyy...\n";

    // an array of the game menu
    string option[] = {"> CONTINUE", "EXIT", "CONTINUE", "> EXIT"};

    // Store the starting options to display the arrow and game menu
    string optionChoice1 = option[0], optionChoice2 = option[1];
    char choice = '\0';    

    do {
      // display the game menu to the terminal
        cout << exit_title << endl << 
        optionChoice1 << "	" << optionChoice2;

      // allow user to choose where to navigate through the menu
      cout << "\nMOVEMENT GUIDE: A (left), D (right), Q (select):\n";
      cin >> choice;


      // if the user navigates left or right, move the arrow
      if (optionChoice1 == option[0] && optionChoice2 == option[1] 
      && (choice == 'A' || choice == 'D')) {
          optionChoice1 = option[2];
          optionChoice2 = option[3];
      } else if (optionChoice1 == option[2] &&
      optionChoice2 == option[3] 
      && (choice == 'A' || choice == 'D')) {
          optionChoice1 = option[0];
          optionChoice2 = option[1];
      }

    // keep the game menu looping until the user selects something
    } while(choice != 'Q');

    // Determine whether the user choice to EXIT or CONTINUE
    // user wanted to EXIT the game
    if (optionChoice1 == option[2] && optionChoice2 == option[3]) {
        return false;
    }

    // user wanted to continue
        return true;
    }


// PROCESS THE MAZE FILE
bool processMazeFile(vector<vector<char>> &maze, char *filename, 
int &start_room, int &finish_room, int &rowCount, int &colCount) {
  // Variables
  ifstream iFile;
  string emptyRow = "";
  // to store each row from the file
  string input = "";
  vector<char> row;

  // Open the file from the command line
  iFile.open(filename);

  // Display an error if the file cannot be opened and end the program
  if (!iFile.is_open()) {
    cout << "\nCannot open '" << filename << "'"
         << "\nERROR: Terminating program..."
         << "\n...Hasta la bye bye agent Jimmy!";
        return false;
  }

  // essientially get rid of the first line which is empty in the 
  // maze file before storing it in the 2D vector
  getline(iFile, emptyRow);

  // Read the maze file into the 2D vector row by row
  while (getline(iFile, input)) {

    // get each character in input string and put it into a 1D vector
    for (int i = 0; i < static_cast<int>(input.size()); i++) {
      row.push_back(input[i]);
    }

    // Push the 1D row vector into the 2D maze vector
    maze.push_back(row);

    // clear the row for the next iteration of the row
    row.clear();
  }

  // Close the file
  iFile.close();

  // Store the rows and columns
  rowCount = static_cast<int>(maze.size());
  colCount = static_cast<int>(maze[0].size());

  // Find out where the finishing room is by 
  //iterating through the last column go through each row
  for (int i = 0; i < static_cast<int>(maze.size()); i++) {
    // look at the last column of every row and find the finishing room
    // minus 1 because index starts at 0
    if (maze[i][colCount - 1] == '_') {
      finish_room = i;
    }
  }

  // Check if the maze has an exit
  if (finish_room == 0) {
      cout << "\nINFO: Maze '" << filename 
      << "' does not have an exit room!\n";
      return false;
  } 

  // Find out where the starting room is
  // by iterating through the first column go through each row
  for (int i = 0; i < static_cast<int>(maze.size()); i++) {
    // look at the first column of every row and find the starting room
    if (maze[i][0] == '_') { 
      // minus one because we're counting from where the maze begins
      start_room = i;
    }
  }

  // Process the maze file into the vector
  cout << "\n\n** Processing file '" << filename << "' **"
      << "\nINFO: Printing the satellite image\n\n";

  // print the unedited maze
  for (int i = 0; i < static_cast<int>(maze.size()); i++) {
    for (int j = 0; j < static_cast<int>(maze[i].size()); j++) {
      cout << maze[i][j] << " ";
    }
    cout << endl;
  }

  // Display information about the maze to the terminal
  cout << "\nINFO: Rows=" << rowCount << ", COLS=" << colCount
       << "\nINFO: Enter Row=" << start_room 
       << ", Exist Row=" << finish_room << endl;

  // Print the maze out
  drawMaze(maze, start_room, finish_room, rowCount, colCount);

  // Print the maze with the changes
  printMaze(maze);

  // Provide Agent Jimmy's starting coordinates
  cout << "\nINFO: Your current coordinates are=(" 
  << start_room << ", 0)\n\n"; 

  // true if the vector is filled and has a finishing/starting room
  if ((start_room != 0 && finish_room != 0) || !maze.empty()) {
    return true;
  } 
    return false;
}

// PROCESS THE DIRECTIONS FILE
bool processInputFile(vector<char> &directions, char *filename) {
  // Variables
  ifstream iFile;
  string emptyRow = "";
  string input = "";

  // Open the file
  iFile.open(filename);

  // skip the first empty row
  getline(iFile, emptyRow);

  // Read the directions file into the vector row by row
  while (getline(iFile, input)) {

    // get each character in the input string and put it into 1D vector
    // skip if its a space
    for (int i = 0; i < static_cast<int>(input.size()); i++) {
      if (input[i] == ' ') {
        continue;
      } 
        directions.push_back(input[i]);
    }
  }

  cout << "\n** Processing file '" << filename << "' **"
       << "\nINFO: Printing the updated image\n\n";
  
  // return true if vector is filled, otherwise return false
  if (directions.empty()) {
    return false;
  } 

    return true;

}

// MAKE THE FIRST MODIFIED IMAGE OF THE MAZE
void drawMaze(vector<vector<char>> &maze, int start_room, 
int finish_room, int rows, int columns) {
  // store the column size of the maze when it is doubled,
  // so reading is also done properly
  int doubleCols = columns * 2;

  // go through each row
  for (int i = 0; i < static_cast<int>(maze.size()); i++) {
    // go through each column
    for (int j = 0; j < static_cast<int>(maze[i].size()); j++) {
      // change the first and last row to the dashes
      if (i == 0 || i == rows - 1) {
        maze[i][j] = '-';
        // replace also if the maze is enlarged
        if (j == doubleCols) {
          maze[i][j] = '-';
        }
        // change the last column of the first and last row to a =
        if (j == columns - 1) {
          maze[i][j] = '=';
        }
      // change the middle rows if its an X to a |
      } else if (i != 0 && i != rows && maze[i][j] == 'X') {
        maze[i][j] = '|';
      // change the last column of enlarged maze to a |
      } else if (j == doubleCols - 1) {
        maze[i][j] = '|';
      // change the middle rows if its a _ to a #
      } else if (i != 0 && i != rows && maze[i][j] == '_') {
        maze[i][j] = '#';
      } 
      // place Agent Jimmy at the starting room
      if (i == start_room) {
        maze[i][0] = 'J';
      }
      // place the O at the finishing room
      if (i == finish_room) {
        maze[i][columns - 1] = 'O';
      }
  }
}

}

// PRINT THE FIRST MODIFIED IMAGE OF THE MAZE
void printMaze(vector<vector<char>> &maze) {
  cout << "INFO: Printing the modified image" << endl << endl;
  // Print the first modified image
  // go through each row
  for (int i = 0; i < static_cast<int>(maze.size()); i++) {
    // go through each column
    for (int j = 0; j < static_cast<int>(maze[i].size()); j++) {
      cout << maze[i][j] << " ";
      }
      cout << endl;
  }

}

// CREATE THE "OTHER SIDE" OF THE MAZE
void increaseMazeSize(vector<vector<char>> &maze, 
int numRows, int numCols) {
// Determine the double amount of columns
  int doubleCols = numCols * 2;

  // Loop through the maze
  // go through the rows
  for (int i = 0; i < numRows; i++) {
    // double the columns
      for (int j = numCols; j < doubleCols; j++) {
        maze[i].push_back(' ');
      }
  }

}

// PRINT THE "OTHER SIDE" OF THE MAZE
void printGrid(vector<vector<char>> &maze, 
int startRoom, vector<int> &position) {
  const int mazeCol = static_cast<int>(maze[0].size()) / 2;
  for (int i = 0; i < static_cast<int>(maze.size()); ++i) {
    for (int j = 0; j < static_cast<int>(maze[i].size()); ++j) {
      if (j < mazeCol) { 
        int index = i * mazeCol + j; 
        if (position[index] > 1) {
          maze[i][j + mazeCol] = 'o';
        }
      }
      if (j >= mazeCol) {
        if (i == startRoom && j == mazeCol) 
          maze[i][j] = 'S';
        if (maze[i][j] != '.' && maze[i][j] != 'X') {
          if (i == 0 || i == static_cast<int>(maze.size()) - 1) {
            maze[i][j] = '-';
          } else if (j == static_cast<int>(maze[i].size()) - 1) {
            maze[i][j] = '|';
          } else if (maze[i][j] != 'o' && maze[i][j] != 'S') {
            maze[i][j] = ' ';
          }
        }
      }
      cout << maze[i][j] << " ";
    }
    cout << endl;
  }
}

// MAKES SURE THE USER STAYS WITHIN THE PATH OF THE MAZE
bool inBounds(int x, int y, vector<vector<char>> &maze, char input) {

  // check the right
  // first check if it's an exit
  if (maze[x][y + 1] != 'O') {
    // continue if not an exit
    if (input == 'D' && maze[x][y + 1] != '#') {
      // meaning no, it is not in bounds
      return false;
    }
  }

  // check left
  if (input == 'A' && maze[x][y - 1] != '#') {
    return false;
  } 

  // check above
  if (input == 'W' && maze[x - 1][y] != '#') {
    return false;
  } 

  // check below
  if (input == 'S' && maze[x + 1][y] != '#') {
    return false;
  }

  return true;

}

// MOVE AGENT JIMMY FROM THE START ROOM TO THE FINISH ROOM
bool moveThroughMaze(vector<vector<char>> &maze, 
int &x, int &y, char input) {
  // Variables
  bool validMove = false;
  bool foundExit = false;

  // x and y is the coordinates of the actual maze (row, column)
  // store where the other side of the maze's columns start
  int stepsCol = static_cast<int>(maze[0].size()) / 2;

  // Display current coordinates of Agent Jimmy
  cout << "Your current coordinates are=(" << x << ", " << y << ")";
  cout << "\nMOVEMENT GUIDE: D (right), A (left), W (up), S (down)"
       << "\nENTER Q TO EXIT THE GAME"
       // for the test cases, output the input from the direction vector
       << "\nWhich way do you want to move? " << input << endl; 

  // Moving right
  if (input == 'D') {
    validMove = inBounds(x, y, maze, input);
    if (validMove == false) {
      cout << "ERROR: cannot move right!\n";
    } else if (validMove == true) {
        // check if the exit is to the right
        if (maze[x][y + 1] == 'O') {
          foundExit = true;
          cout << "\nINFO: Found exit to the maze at"
               << "(" << x << ", " << y << ")\n";

          // Add the exit to the maze
          maze[x][(y + 1) + stepsCol] = 'X';

        } 

        if (foundExit == false) {
          y += 1;                     // Adjust the coordinates
          maze[x][y + stepsCol] = '.'; // trace path
          maze[x][y] = 'J';           // move J
          maze[x][y - 1] = '#';       // replace path    
        }  
      }
  }

  // Moving left
  if (input == 'A') {
    validMove = inBounds(x, y, maze, input);
    if (validMove == false) {
      cout << "ERROR: cannot move left!\n";
    } else if (validMove == true) {
      y -= 1;                     // Adjust the coordinates
      maze[x][y + stepsCol] = '.'; // trace path
      maze[x][y] = 'J';           // move J
      maze[x][y + 1] = '#';       // replace path
    }
  }

  // Moving up
  if (input == 'W') {
    validMove = inBounds(x, y, maze, input);
    if (validMove == false) {
      cout << "ERROR: cannot move up!\n";
    } else if (validMove == true) {
      x -= 1;                      // Adjust the coordinates
      maze[x][y + stepsCol++] = '.'; // trace path
      maze[x][y] = 'J';           // move J
      maze[x + 1][y] = '#';       // replace path
    }
  }

  // Moving down
  if (input == 'S') {
    validMove = inBounds(x, y, maze, input);
    if (validMove == false) {
      cout << "ERROR: cannot move down!\n";
    } else if (validMove == true) {
      x += 1;                      // Adjust the coordinates
      maze[x][y + stepsCol--] = '.'; // trace path
      maze[x][y] = 'J';           // move J
      maze[x - 1][y] = '#';       // replace path
    }
  }

  // if user chooses to exit
  if (input == 'Q') {
    return true;
  }

  return false;

}

// two points into a single point 
int XYToIndex(int x, int y, int numCols) {

  int index = x * numCols + y;

  return index;

}

// CONVERTS THE INDEX TO TWO POINTS
void indexToXY(int index, int num_cols, int &x, int &y) {

  // to prevent dividing by zero
  if (num_cols != 0) {
    // given formulas
    x = index % num_cols;
    y = (index - y) / num_cols;
  }

}

// KEEPS TRACK OF WHEN AGENT JIMMY RETRACES HIS STEPS
void trackPathThroughMaze(vector<int> &position, 
int x, int y, int numCols) {
  int index = XYToIndex(x, y, numCols);
  // indicate how often he's visited certain spots
  position[index]++; 
}

// KEEPS TRACK OF THE COORDINATES OF WHERE HE'S VISITED MORE THAN ONCE
bool retracePathThroughMaze(vector<int> &position, int numCols) {
  bool flag = false;
  int x = 0, y = 0;
  for (int i = 0; i < static_cast<int>(position.size()); ++i) {
    if (position[i] > 1) {
      indexToXY(i, numCols, x, y);
      cout << "(" << y << "," << x << ")" << endl;
      flag = true;
    }
  }
  return flag;
}
