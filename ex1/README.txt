Contributors:
-------------
Name: Yuval Namir Barr
ID: 207004615

Name: Roni Segal
ID: 322270851

Input and Output Format:
-------------------------

Each input file describes:
- A game board and its size.
- Initial positions and directions of player tanks.
- Obstacles, walls, and other board elements.

Legend:
- '1' = Player 1 tank
- '2' = Player 2 tank
- '@' = Shell on board
- '#' = Wall or obstacle
- ' ' = Empty space

Each output file (output_<input>.txt) includes:
- A turn-by-turn log of the game.
- Actions taken by each tank.
- Result of the game (Player 1 wins / Player 2 wins / Tie).

Input files:
------------
input_a.txt — Tie scenario: both tanks are destroyed.  
input_b.txt — Player 2 wins: outmaneuvers and shoots Player 1.  
input_c.txt — Player 1 wins: destroys Player 2 with a well-placed shot.

How to Run:
-----------
Compile the program and run:
    ./tanks_game <input_file>

Example:
    ./tanks_game input_b.txt

This will generate:
    output_input_b.txt
