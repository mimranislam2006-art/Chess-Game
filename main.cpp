#include "include/ChessGame.h"
#include <iostream>
#include <fstream>
#include <exception>

using namespace std;

int main(int argc, char* argv[]) {
    // Log errors to a file so we can see what crashes
    ofstream logFile("chess_log.txt");
    auto& log = logFile;

    try {
        string assetDir = "assets";
        if (argc > 1) assetDir = argv[1];

        log << "Starting ChessGame...\n";
        log << "Asset dir: " << assetDir << "\n";
        log.flush();

        ChessGame game;

        log << "Calling init...\n"; log.flush();
        if (!game.init(assetDir)) {
            log << "init() returned false\n";
            cerr << "Failed to initialise ChessGame.\n";
            return 1;
        }

        log << "init() OK, calling run()...\n"; log.flush();
        game.run();

        log << "run() returned normally.\n";
    }
    catch (const exception& e) {
        log << "EXCEPTION: " << e.what() << "\n";
        cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        log << "UNKNOWN EXCEPTION\n";
        cerr << "Unknown exception\n";
        return 1;
    }

    log << "Exiting cleanly.\n";
    return 0;
}
