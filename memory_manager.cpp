#include "memory_manager.h"

int main(int argc, char* argv[]) {
    string currentExecName = argv[0]; // Name of the current exec program
    string filePath = "";

    AllocStrat selectedStrategy = AllocStrat::First;

    if (selectedStrategy == AllocStrat::Best) {
        cout << "cool";
    }

    // Error checking
    if (currentExecName.substr(currentExecName.size() - 8) == "firstfit") {
        selectedStrategy = AllocStrat::First;
    }
    else if (currentExecName.substr(currentExecName.size() - 7) == "bestfit") {
        selectedStrategy = AllocStrat::Best;
    }
    else {
        cout << "Invalid file name, must be one of: \"firstfit\" or \"bestfit\"." << endl;
        return EXIT_FAILURE;
    }

    if (argc != 2) {
        cout << "Invalid arguments, must be only one datafile argument." << endl;
        return EXIT_FAILURE;
    }
    
    filePath = argv[1];

    // Read input file
    ifstream file(filePath);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            //FIXME read correctly from datafile            
            istringstream iss(line);
            string arg1, arg2;

            getline(iss, arg1, ':');
            getline(iss, arg2);
        }
        file.close();
    } else {
        cout << "The file '" << filePath << "' does not exist." << endl;
        return EXIT_FAILURE;
    }
}