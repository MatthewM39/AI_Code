/*
	Letters game by Matthew McCrackin for AI Fall 2016

	When launching the program, the user is presented with the option of playing themselves or letting the AI play.
	When playing themselves, the user will be prompted to select the coordinate of a tile to move.
	The user enters in the X and then Y coordinate of the tile to move and then the slot to swap to
	The values for X and Y are shown below:

				|0|1|2|3|4|  
				|1|
				|2|
				|3|
				|4|

	The game is over when the board is full. Every turn, 3 letters will drop on the board. The player is shown these letters
	the previous turn before they drop. If the board is filled with 22 tiles and 3 more drop, there is no way for the player
	to win so the game ends. A chain only activates and earns the player points when the player ACTIVELY moves the tile to 
	a slot.

	My chain identifier starts from the tile the player selects and scans around it. So it is possible to get larger chains,
	but only if the selected tile is in the middle. See the report included for more information.

	If the user selects AI mode instead of player mode, then the user will be prompted to enter how many games to run.
	Finally, the user will be prompted to decide whether to display board printing for the AI or not. 

	The AI is coded using a greedy approach. It analyzes the entire statespace, decides which move will earn the highest score, 
	and then chooses that move. If no moves can be made as all the letters on the board are different,
	it selects a tile not connected to any others and moves it somewhere random.

*/




#include <iostream> 
#include <list>
#include <ctime>
#include "StopWatch.h"

struct point { // struct used to access values in the 2D array of slots on the board

	point(int xp, int yp) { // need an x and y coord for a point
		x = xp;
		y = yp;
	}

	int x, y;

	bool point::operator==(const point &other) const { // overload for the std list :|
		return (this->x == other.x && this->y == other.y);
	}
};

class myGame {

public:

	char myBoard[5][5]; // 25 tile board of chars
	int openSlots; // number of open slots on the board
	int currentRandom; // number of letters possible. starts at 6. increases every 10,000 pts.
	char currentFall[3]; // char array to hold the 3 falling tiles
	std::list<point> available; // a list of open points for rand to choose from
	std::list<point> toRemove; // a list of points to remove when a chain is formed
	int score;
	bool noPrint; // flag for the AI

	myGame(){
		srand(time(NULL)); // seeding rand with the time
		clearBoard(); 
		currentRandom = 6;
		score = 0;
		noPrint = true;
	}

	// clears the board and resets everything
	void clearBoard() {
		available.clear();
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				myBoard[i][j] = ' ';
				available.emplace_back(point(i, j));
			}
		}
		openSlots = 25;
		currentRandom = 6;
		toRemove.clear();
		score = 0;
	}

	// Letter Chain Scans... Horizontal, Vertical, and Sideways
	// traverse right and left until the character chain is broken and return the number
	int horScan(int x, int y, char c) {
		int counter = 0;
		int shift = x + 1; // shift one right at first
		std::list<point> tempList;
		while (shift < 5 && myBoard[y][shift] == c) { // go right and check shift first to make sure array is in bounds :)
			tempList.emplace_back(point(shift, y));
			shift++;
			counter++;
		}
		shift = x - 1; // shift one left first
		while (shift > -1 && myBoard[y][shift] == c) { // go left
			tempList.emplace_back(point(shift, y));
			shift--;
			counter++;
		}

		if (counter > 1) { //if counter is bigger than 1, then there's at least 3 succesive letters...
			std::list<point>::iterator myIt = tempList.begin();
			while (myIt != tempList.end()) {
				toRemove.emplace_back(*myIt); // add them from the temp list to the remove list
				myIt++;
			}
			return counter;
		}
		return 0;
	}

	int vertScan(int x, int y, char c) {
		int counter = 0;
		int shift = y  - 1; // board descends low to high so decrement to go up...
		std::list<point> tempList;
		while (shift > -1 && myBoard[shift][x] == c) { // go right and check shift first to make sure array is in bounds :)
			tempList.emplace_back(point(x, shift));
			shift--;
			counter++;
		}
		shift = y + 1; // shift one down first
		while (shift < 5 && myBoard[shift][x] == c) { // go down board
			tempList.emplace_back(point(x, shift));
			shift++;
			counter++;
		}

		if (counter > 1) { //if counter is bigger than 1, then there's at least 3 succesive letters...
			std::list<point>::iterator myIt = tempList.begin();
			while (myIt != tempList.end()) {
				toRemove.emplace_back(*myIt); // add them from the temp list to the remove list
				myIt++;
			}
			return counter;
		}
		return 0;
	}

	// scans / ways
	int forwardSlashScan(int x, int y, char c) {
		int counter = 0;
		std::list<point> tempList;
		int shiftX = x + 1; // check upper right first
		int shiftY = y - 1; // so y needs to go down
		while (shiftX < 5 && shiftY > -1 && myBoard[shiftY][shiftX] == c) {
			tempList.emplace_back(point(shiftX, shiftY));
			shiftX++;
			shiftY--;
			counter++;
		}
		shiftX = x - 1;
		shiftY = y + 1; // now downer left check
		while (shiftX > -1 && shiftY < 5 && myBoard[shiftY][shiftX] == c) {
			tempList.emplace_back(point(shiftX, shiftY));
			shiftX--;
			shiftY++;
			counter++;
		}
		if(counter > 1) { //if counter is bigger than 1, then there's at least 3 succesive letters...
			std::list<point>::iterator myIt = tempList.begin();
			while (myIt != tempList.end()) {
				toRemove.emplace_back(*myIt); // add them from the temp list to the remove list
				myIt++;
			}
			return counter;
		}
		return 0;
	}

	// scans \ ways
	int backSlashScan(int x, int y, char c) {
		int counter = 0;
		std::list<point> tempList;
		int shiftX = x + 1; // check down right
		int shiftY = y + 1; // so y needs to go up
		while (shiftX < 5 && shiftY < 5 && myBoard[shiftY][shiftX] == c) {
			tempList.emplace_back(point(shiftX, shiftY));
			shiftX++;
			shiftY++;
			counter++;
		}
		shiftX = x - 1;
		shiftY = y - 1; // now upper left check
		while (shiftX > -1 && shiftY > -1 && myBoard[shiftY][shiftX] == c) {
			tempList.emplace_back(point(shiftX, shiftY));
			shiftX--;
			shiftY--;
			counter++;
		}
		if (counter > 1) { //if counter is bigger than 1, then there's at least 3 succesive letters...
			std::list<point>::iterator myIt = tempList.begin();
			while (myIt != tempList.end()) {
				toRemove.emplace_back(*myIt); // add them from the temp list to the remove list
				myIt++;
			}
			return counter;
		}
		return 0;
	}

	// scans the board from point x,y for AI computes the longest chain only, for nonAI calls, computes score and removes chains
	int scanBoard(int x, int y, bool isAI) {

		toRemove.clear();
		int chainLen = vertScan(x, y, myBoard[y][x]) + horScan(x, y, myBoard[y][x]) + forwardSlashScan(x, y, myBoard[y][x]) +
			backSlashScan(x, y, myBoard[y][x]) + 1; // need to add the total chain size from the origin tile...

		if (!isAI && chainLen > 1) { // if it isn't the ai doing computations and a valid chain is reached....
			std::list<point>::iterator myIt = toRemove.begin(); // iterate through the remove list
			while (myIt != toRemove.end()) {
				available.emplace_back(*myIt); // add the current tile to the available list
				myBoard[myIt->y][myIt->x] = ' '; // empty the tile
				myIt++;
			}
			myBoard[y][x] = ' ';
			available.emplace_back(point(x, y));
			openSlots += chainLen;
				switch (chainLen) { // now compute the score to add
				case 3: 
					score += 20;
					break;
				case 4:
					score += 30;
					break;
				case 5: 
					score += 45;
					break;
				case 6: 
					score += 65;
					break;
				case 7: 
					score += 90;
					break;
				case 8: 
					score += 120;
					break;
				case 9: 
					score += 155;
					break;
				default: 
					score += 200;
					break;
				}
			
		}

		return chainLen; // return value matters for AI
	}

	// loop through the char array and print it all fancy like
	void printBoard() {
		for (int i = 0; i < 5; i++) {
			std::cout << std::endl << "--------------------" << std::endl << "| ";
			for (int j = 0; j < 5; j++) {
				std::cout << myBoard[i][j] << " | ";
			}
		}
		std::cout << std::endl << "--------------------" << std::endl;
		std::cout << std::endl;
	}

	// assign 3 random letters to fall
	void permuteFall() {
		char myLetter = 64 + getRandomLetter();
		currentFall[0] = myLetter;
		myLetter = 64 + getRandomLetter();
		currentFall[1] = myLetter;
		myLetter = 64 + getRandomLetter();
		currentFall[2] = myLetter;
	}

	// return a number between 1 and the maximum number of current random letters
	int getRandomLetter() {
		int permute = rand() % currentRandom + 1;
		return permute;
	}

	// call rand with the # of open slots and iterate to that item in the list of open slots
	void getRandomSlot(int &x, int &y) {
		std::list<point>::iterator myIt = available.begin();
		int random = rand() % openSlots;
		for (int i = 0; i < random; i++) {
			++myIt;
		}
		x = myIt->x;
		y = myIt->y;
	}

	// swap the letters of points a and b
	void swapPoints(point &a, point &b) {
		char temp = myBoard[a.y][a.x];
		myBoard[a.y][a.x] = myBoard[b.y][b.x];
		myBoard[b.y][b.x] = temp;
		return;
	}

	// ai scans the whole board, moves every tile everywhere, and picks the highest scoring move
	void aiScan() {

		int maxScore = -1;
		point maxPoint = point(0, 0);
		point swapPoint = point(0, 0);

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (myBoard[i][j] != ' ') {  // identify a lettered tile
					for (int i1 = 0; i1 < 5; i1++) { 
						for (int j1 = 0; j1 < 5; j1++) {
							if (myBoard[i1][j1] == ' ') {// identify an empty tile
								point a = point(j, i); //  a is the lettered tile
								point b = point(j1, i1); // b is the empty tile
								swapPoints(a, b); // a is empty, b is lettered
								int scanL = scanBoard(b.x, b.y, true); // scan from b now that it has a letter
								swapPoints(a, b); // now swap the letters 
								if (scanL > maxScore) {  // if the chain scanned is the biggest
									maxScore = scanL;  //  set new max score
									maxPoint = a; // the tile to swap
									swapPoint = b; // where to swap to
								}
							}
						}
					}
				}
			}
		}
		
		
		if (maxPoint == swapPoint) { // just pick a random point
			int random = rand() % openSlots;
			std::list<point>::iterator myIt;
			for (int i = 0; i < random; i++) {
				myIt++;
			}
			swapPoint = *myIt;
			for (int i = 0; i < 5; i++) { // find a letter
				for (int j = 0; j < 5; j++) {
					if (myBoard[i][j] != ' ') {
						maxPoint = point(j, i);
						break;
					}

				}
			} 

		

			available.remove(swapPoint);
			available.emplace_back(maxPoint); // we can now add the tile to swap to the open tiles
			swapPoints(maxPoint, swapPoint); // now swap the char values
			scanBoard(swapPoint.x, swapPoint.y, false); // now scan
			return;

		}

		
		available.remove(swapPoint); // swap the two tiles
		available.emplace_back(maxPoint); // we can now add the tile to swap to the open tiles
		swapPoints(maxPoint, swapPoint); // now swap the char values
		if(!noPrint)
			std::cout << "\nMoving char " << myBoard[swapPoint.y][swapPoint.x] << " from " <<
				maxPoint.x << ", " << maxPoint.y << "  to " << swapPoint.x << ", " << swapPoint.y <<
				"\n";
		scanBoard(swapPoint.x, swapPoint.y, false); // now scan
	}

	// let the player pick a tile and where to move it
	void playerMove() {
		int x, y;
		bool flag = true;
		point filled = point(0, 0);
		point empty = point(0, 0);
		std::list<point>::iterator myIt = available.begin();
		while (flag) {
			std::cout << "Select tile to move \nEnter x coord: ";
			std::cin >> x;
			std::cout << "Enter y coord: ";
			std::cin >> y;
			std::cout << std::endl;
			filled.x = x;
			filled.y = y;
			myIt = available.begin();
			flag = false;
			while (myIt != available.end()) { // or i could just check if the char is != " "
				if (*myIt == filled)
					flag = true;
				myIt++;
			}
			if (x < 0 || x > 4 || y < 0 || y > 4) // valid bounds check, should flip if above comment is implemented
				flag = true;
			std::cout << std::endl;
		}

		while (!flag) {
			std::cout << "Select place to move tile \nEnter x coord: ";
			std::cin >> x;
			std::cout << "Enter y coord: ";
			std::cin >> y;
			std::cout << std::endl;
			empty.x = x;
			empty.y = y;
			myIt = available.begin();
			flag = false;
			while (myIt != available.end()) { // or i could just check if the char is == " "
				if (*myIt == empty) {
					flag = true;
				}
				myIt++;
			}
			if (x < 0 || x > 4 || y < 0 || y > 4) // same as before
				flag = false;
		}
		available.remove(empty);
		available.emplace_back(filled);
		myBoard[empty.y][empty.x] = myBoard[filled.y][filled.x];
		myBoard[filled.y][filled.x] = ' '; //swap the two tiles
		scanBoard(x, y, false); // now see what the player action did!
	}

	void runGameHuman() {
		int x = 0; // for choosing coords
		int y = 0;

		permuteFall(); // initial random letters

		while (openSlots > 3) {

			getRandomSlot(x, y); // get a slot
			available.remove(point(x, y)); // remove slot from open slots
			myBoard[y][x] = currentFall[0]; // change slot to be random letter
			openSlots--; // decrease open slots

			getRandomSlot(x, y); // x2 
			available.remove(point(x, y));
			myBoard[y][x] = currentFall[1];
			openSlots--;

			getRandomSlot(x, y); // x3
			available.remove(point(x, y));
			myBoard[y][x] = currentFall[2];
			openSlots--;

			permuteFall(); // now get 3 more random letters and display them
			std::cout << "\nLetters to fall next: " << currentFall[0] << " " << currentFall[1] << " " << currentFall[2] << "\nCurrent Score: "
				<< score << "\n";
			printBoard(); // print the board
			playerMove(); // let the player move
			currentRandom = 6 + (score / 10000); // accounting for the every 10,000 add a letter rule. 
		}
	}

	int runGameAI() {

		int x = 0; // for choosing coords
		int y = 0;
		int counter = 1;
		permuteFall(); // initial random letters
		CStopWatch myTimer;
		myTimer.startTimer();
		while (openSlots > 3) {


			getRandomSlot(x, y); // get a slot
			available.remove(point(x, y)); // remove slot from open slots
			myBoard[y][x] = currentFall[0]; // change slot to be random letter
			openSlots--; // decrease open slots

			getRandomSlot(x, y); // x2 
			available.remove(point(x, y));
			myBoard[y][x] = currentFall[1];
			openSlots--;

			getRandomSlot(x, y); // x3
			available.remove(point(x, y));
			myBoard[y][x] = currentFall[2];
			openSlots--;

			permuteFall(); // now get 3 more random letters and display them
			if (!noPrint) {
				std::cout << "\n\n\n----------------------BEGIN MOVE\nMove Number " << counter << "\nLetters to fall next: " << currentFall[0] << " " << currentFall[1] << " " << currentFall[2] << "\nCurrent Score: "
					<< score << "\n";
				printBoard(); // print the board
				counter++;
				aiScan();
				printBoard();
				std::cout << "\n------------------------ END MOVE\n\n\n";
			}
			else {
				counter++;
				aiScan();
			}
			currentRandom = 6 + (score / 10000);
		}
		myTimer.stopTimer();
		std::cout << "\nElapsed Time for Round: " << myTimer.getElapsedTime() << "\nAverage Time Per Move: " << (myTimer.getElapsedTime() / counter) << "\n";

		
		return score;
	}
};