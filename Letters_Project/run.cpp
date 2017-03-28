#include <iostream>
#include "letters.cpp"

int main() {
	int minScore, maxScore, avg, counter, type = 0;
	myGame gameRun;
	std::cout << "Enter 0 for AI mode: ";
	std::cin >> type;
	if (type == 0) { // run in AI mode
		std::cout << "Enter number of games to run: "; // get number of times to run
		std::cin >> counter;
		std::cout << "Enter 0 to disable board printing: "; // option to disable printing
		std::cin >> type;
		if (type != 0)
			gameRun.noPrint = false;
		minScore = gameRun.runGameAI(); // run the game once to get the min score and max and avg so far
		maxScore = minScore;
		avg = minScore;

		gameRun.clearBoard(); // now clear the board
		for (int i = 1; i < counter; i++) { // run the game the number of times specified and compare extrema
			int sc = gameRun.runGameAI();
			if (sc < minScore) {
				minScore = sc;
			}
			if (sc > maxScore) {
				maxScore = sc;
			}
			avg += sc; // add to the average
			gameRun.clearBoard(); // clear a board after a round
		}

		std::cout << "\nMinimum Score: " << minScore << "\nMaximum Score: " << maxScore <<
			"\nAVG: " << (avg / counter) << "\nAVG without extrema: " << ((avg - maxScore - minScore) / (counter - 2)) << "\nTotal Runs: "
			<< counter << "\n";
	}
	else {
		gameRun.runGameHuman(); // otherwise just run the game as a human
	}

	return 0;
}