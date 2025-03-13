# Turing Machine
Turing machine is the hypothetical machine proposed by Alan Turing, which in theory can do of every operation that modern computers are capable of.
This program simulates the Turing Machine displaying its state and the condition of tape it's operating on.

# Compiling
To compile the code run in console
```
  make build
```
Program was tested on Ubuntu Linux, but should work on Windows aswell. A GCC compiler is neccessary.

# Running the program
Program has to be run from the console. To work you need to provide a path to the compatible program file.

# Writing programs
Each program is a simple text file with specific structure.
The first line of the program file has to be define alphabet the machine will use. Every character on that line will be treated as part of the alphabet (except for '\n') and as such there should be no spaces between each character.
The order of characters is also important, as the first character of the alphabet will be treated as the EMPTY character, which means that any not defined tapes fragments will be filled with that character automatically.

The second line of the program file is the initial state of the tape. All of the tape has to be written inside this line. Only characters defined in the alphabet should be used.

In next lines the states of the turing machine are defined. Every state definition consists of 4 lines inside program file.
First line is the name of the state (can be left blank) which will display when machine is in that state.
Second line defines what should be the next state when machine reads an alphabet character. It consists of numbers seperated by space. The order is consitent with the alphabet defined on line 1.
Third line defines what how should the head of the machine move after reading an alphabet character.
Fourth line defines what character should be written in a place of old one.

Next lines follow the same rules as the four lines described above, each 4 lines define a new state.
Examples of turing machine programs are located inside ./programs/ directory.
