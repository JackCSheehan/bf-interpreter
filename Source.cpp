/*
BF Interpreter Written by Jack Sheehan
*/
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <climits>

//Symbolic constants
#define DEFAULT_CELL_VALUE 0

//BF instruction set
#define MOVE_RIGHT_INSTRUCTION '>'  //Instruction that moves tape pointer right
#define MOVE_LEFT_INSTRUCTION '<'   //Instruction that moves tape pointer left
#define INCREMENT_INSTRUCTION '+'   //Instruction that increments current cell
#define DECREMENT_INSTRUCTION '-'   //Instruction that decrements current cell
#define WRITE_INSTRUCTION '.'       //Instruction that outputs contents of current cell
#define READ_INSTRUCTION ','        //Instruction that reads into current cell
#define JUMP_START_INSTRUCTION '['  //Instruction that jumps to ] if cell is 0
#define JUMP_END_INSTRUCTION ']'    //Instruction that jumps to [ if cell is not 0


//Function prototypes
void moveRight(std::vector<unsigned char>&, unsigned int&, unsigned int);
void moveLeft(unsigned int&, unsigned int);
void increment(std::vector<unsigned char>&, unsigned int);
void decrement(std::vector<unsigned char>&, unsigned int);
void write(const std::vector<unsigned char>&, unsigned int);
void read(std::vector<unsigned char>&, unsigned int);
void jumpStart(const std::vector<unsigned char>&, const std::vector<unsigned char>&, unsigned int, unsigned int&);
void jumpEnd(const std::vector<unsigned char>&, const std::vector<unsigned char>&, unsigned int, unsigned int&);

int main(int argc, char *argv[])
{
   //Defining constants
   const unsigned int DEFAULT_POINTER_VALUE = 0;//The default value of the tape pointer
   const int FILE_NAME_ARGUMENT_INDEX = 1;      //The command line argument index of the file name
   const int EXPECTED_NUMBER_OF_ARGUMENTS = 2;  //The expected number of command arguments when executable is run

   //Declaring variables
   std::ifstream sourceFile;                    //Input file stream for the BF source file
   std::string sourceFilePath;                  //The file path of the BF source file

   std::vector<unsigned char> tape;             //The program tape
   unsigned int pointer;                        //The tape pointer used for program control

   std::vector<unsigned char> instructions;     //The instructions read from the source file
   char currentInstruction;                     //The current instruction being read from the source file

   //Check to make sure the correct number of arguments were given when executable was run
   if (argc != EXPECTED_NUMBER_OF_ARGUMENTS)
   {
      std::cout << "Failed to run. Expected " << EXPECTED_NUMBER_OF_ARGUMENTS << " arguments but " << argc << " arguments were encountered." << std::endl;

      //Return exit failure is incorrect number of arguments were passed
      return EXIT_FAILURE;
   }

   //Get the name of the file from the command line arguments
   sourceFilePath = argv[FILE_NAME_ARGUMENT_INDEX];

   //Attempt to open the source file and check for file validity
   sourceFile.open(sourceFilePath);

   //If invalid file, alert user and return exit failure
   if (!sourceFile)
   {
      std::cout << "Source file \"" << sourceFilePath << "\" could not be opened"  << std::endl;
      return EXIT_FAILURE;
   }

   //If file is valid, read each character into the instruction vector
   while (sourceFile >> std::noskipws >> currentInstruction)
   {
      instructions.push_back(currentInstruction);
   }
      
   //Close input file
   sourceFile.close();
   
   //Initialize first value of tape to default value
   tape.push_back(DEFAULT_CELL_VALUE);

   //Initialize pointer to default value
   pointer = DEFAULT_POINTER_VALUE;

   unsigned int instructionCount = 0;  //Keeps track of the current instruction being parsed in the instructions vector

   //Iterate through each value in the instructions and interpret the instructions
   while (instructionCount < instructions.size())
   {
      //Run functions according to what instruction is currently being encountered
      switch (instructions.at(instructionCount))
      {
      case MOVE_RIGHT_INSTRUCTION:
         moveRight(tape, pointer, instructionCount);
         instructionCount++;
         break;
      case MOVE_LEFT_INSTRUCTION:
         moveLeft(pointer, instructionCount);
         instructionCount++;
         break;
      case INCREMENT_INSTRUCTION:
         increment(tape, pointer);
         instructionCount++;
         break;
      case DECREMENT_INSTRUCTION:
         decrement(tape, pointer);
         instructionCount++;
         break;
      case WRITE_INSTRUCTION:
         write(tape, pointer);
         instructionCount++;
         break;
      case READ_INSTRUCTION:
         read(tape, pointer);
         instructionCount++;
         break;
      case JUMP_START_INSTRUCTION:
         //The jump command branches do not increment instructionCount because instructionCount is conditionally changed within the functions themselves
         jumpStart(tape, instructions, pointer, instructionCount);
         break;
      case JUMP_END_INSTRUCTION:
         jumpEnd(tape, instructions, pointer, instructionCount);
         break;
      default:
         //By BF language specifications, characters that are not part of the instruction set are simply ignored
         instructionCount++;
         break;
      }
   } //end for

   return 0;
} //end function main

//Function definitions
/*
Function:      moveRight(std::vector<unsigned char> &tape, unsigned int &pointer, unsigned int instructionCount)
Description:   Moves the tape pointer to the right (increments tape pointer). If
               pointer is at the end of the tape, another char will be pushed onto
               the end of the tape, providing an "infinitely" large tape as specified
               by BF language specifications. Will show error message and quit program
               if attempting to go past the maximum size of a vector.
Input:         tape:
               The program tape. Passed by reference to prevent copying entire
               program tape into parameter.
               pointer:
               A reference variable that is an alias for the tape pointer.
               instructionCount:
               Keeps track of which character is being parsed by the for loop in main;
               used in this function to report where a tape overflow happens.
*/
void moveRight(std::vector<unsigned char> &tape, unsigned int &pointer, unsigned int instructionCount)
{
   static const std::string ERROR_MESSAGE = "ERROR: Attempted tape overflow at character "; //Error message

   //First, check to make sure that the tape isn't already at max possible size for this machine
   if (pointer == tape.max_size() - 1)
   {
      std::cout << ERROR_MESSAGE << instructionCount + 1 << std::endl;
      exit(EXIT_FAILURE);
   }

   //If the pointer is at the end of allocated tape, append a new cell to the end of the tape, then increment pointer
   if (pointer + 1 == tape.size())
   {
      tape.push_back(DEFAULT_CELL_VALUE);
      ++pointer;
   }
   //If pointer isn't at end of tape, simply increment pointer
   else
   {
      ++pointer;
   }
} //end function moveRight

/*
Function:      moveLeft(unsigned int &pointer, unsigned int instructionCount)
Description:   Moves the tape pointer to the left (decrements tape pointer). If
               pointer is at the beginning of the tape, program will end and an
               error message will be displayed.
Input:         pointer:
               A reference variable that is an alias for the tape pointer.
               instructionCount:
               Keeps track of which character is being parsed by the for loop in main;
               used in this function to report where a tape underflow happens.
*/
void moveLeft(unsigned int &pointer, unsigned int instructionCount)
{
   static const std::string ERROR_MESSAGE = "ERROR: Attempted tape underflow at character "; //Error message

   //If pointer is at beginning of tape, show error as the BF specification discourages pointer over/underflow;
   //NOTE: This error will stop the interpreter
   if (pointer == 0)
   {
      std::cout << ERROR_MESSAGE << instructionCount + 1 << std::endl;
      exit(EXIT_FAILURE);
   }
   //If pointer isn't at beginning of tape, simply decrement the pointer
   else
   {
      --pointer;
   }
} //end function moveLeft

/*
Function:      increment(std::vector<unsigned char> &tape, unsigned int pointer)
Description:   Increments the current cell pointed to by the tape pointer. If
               cell is at max unsigned char value, the cell will wrap around to
               0 as specified in BF language specification. The overflow logic is
               for this function is inherited from C++ overflow logic.
Input:         tape:
               The program tape that will be modified by this function.
               pointer:
               The tape pointer value.
*/
void increment(std::vector<unsigned char> &tape, unsigned int pointer)
{
   ++tape.at(pointer);
} //end function increment

/*
Function:      decrement(std::vector<unsigned char> &tape, unsigned int pointer)
Description:   Decrement the current cell pointed to by the tape pointer. If
               cell is at min unsigned char value, the cell will wrap around
               to the max unsigned char value as specified by BF language
               specification. The underflow logic for this function is
               inherited from the C++ underflow logic.
Input:         tape:
               The program tape that will be modified by this function.
               pointer:
               The tape pointer value.
*/
void decrement(std::vector<unsigned char> &tape, unsigned int pointer)
{
   --tape.at(pointer);
} //end function decrement

/*
Function:      write(const std::vector<unsigned char> &tape, unsigned int pointer)
Description:   Outputs the value at the cell pointed to by the tape pointer.
Input:         tape:
               The program tape that will be read from by this function. Passed 
               by reference to prevent copying entire program tape into parameter.
               pointer:
               The tape pointer value.
*/
void write(const std::vector<unsigned char> &tape, unsigned int pointer)
{
   std::cout << tape.at(pointer);
} //end function write

/*
Function:      read(std::vector<unsigned char> &tape, unsigned int pointer)
Description:   Allows one byte input into the cell currently pointed to by the tape
               pointer.
Input:         tape:
               The program tape that will be read into by this function.
               pointer:
               The tape pointer value.
*/
void read(std::vector<unsigned char> &tape, unsigned int pointer)
{
   tape.at(pointer) = std::cin.get();

   //If user types in additional characters, the rest of the characters need to be ignored
   std::cin.ignore(INT_MAX, '\n');
} //end function read

/*
Function:      jumpStart(const std::vector<unsigned char> &tape, const std::vector<unsigned char> &instructions, unsigned int pointer, unsigned int &instructionCount)
Description:   Skips to the matching ] instruction if the cell pointed at by the
               tape pointer is equal to 0. Commonly used for looping structures.
Input:         tape:
               The program tape. Passed by reference to prevent copying entire
               program tape into parameter.
               instructions:
               The instructions read from the source file. Passed by reference to
               prevent copying entire instruction vector into parameter.
               pointer:
               The program tape pointer.
               instructionCount:
               The value that keeps track of which instruction in the instructions
               vector is currently being parsed.
*/
void jumpStart(const std::vector<unsigned char> &tape, const std::vector<unsigned char> &instructions, unsigned int pointer, unsigned int &instructionCount)
{
   //Base case; if the current cell is not 0 then this instruction should do nothing but move the instruction counter forward
   if (tape.at(pointer) != 0)
   {
      instructionCount++;
      return;
   }

   static std::string ERROR_MESSAGE = "ERROR: Unbounded jump instruction; expected corresponding ']' but was not found";  //Error message

   int openBracketsCount = 0;                //Keeps track of how many open brackets have been found
   int closedBracketsCount = 0;              //Keeps track of how many closed brackets have been found
   bool hasMatchingBracketBeenFound = false; //Keeps track of whether or not a matching bracket has been found

   //Iterate through each value in the instruction counter to find the corresponding ]
   for (unsigned int count = instructionCount; count < instructions.size() && !hasMatchingBracketBeenFound; count++)
   {
      //Increment the bracket counters as needed to find corresponding closing bracket
      if (instructions.at(count) == JUMP_START_INSTRUCTION)
      {
         openBracketsCount++;
      }
      else if (instructions.at(count) == JUMP_END_INSTRUCTION)
      {
         closedBracketsCount++;
      }

      //Once the brackets have been balanced, then instructions.at(count) points to the corresponding closing bracket for instructions.at(pointer)
      if (openBracketsCount == closedBracketsCount)
      {
         //Since the base case was tested at the top of the function, it is already known that the current is cell is 0, so the program must jump to the corresponding ]
         instructionCount = count;

         //Set found flag to true
         hasMatchingBracketBeenFound = true;
      }
   } //end for

   //Return an error message and quit program if no matching ] was found
   if (!hasMatchingBracketBeenFound)
   {
      std::cout << ERROR_MESSAGE << std::endl;
      exit(EXIT_FAILURE);
   }

} //end function jumpStart

/*
Function:      jumpEnd(const std::vector<unsigned char> &tape, const std::vector<unsigned char> &instructions, unsigned int pointer, unsigned int &instructionCount)
Description:   Skips to the matching [ instruction if the cell pointed at by
               the tape pointer is not equal to 0.
Input:         tape:
               The program tape. Passed by reference to prevent copying entire
               program tape into parameter.
               instructions:
               The instructions read from the source file. Passed by reference to
               prevent copying entire instruction vector into parameter.
               pointer:
               The program tape pointer.
               instructionCount:
               The value that keeps track of which instruction in the instructions
               vector is currently being parsed.
*/
void jumpEnd(const std::vector<unsigned char> &tape, const std::vector<unsigned char> &instructions, unsigned int pointer, unsigned int &instructionCount)
{
   //Base case; if the current cell is 0 then this instruction should do nothing but move the instruction counter forward
   if (tape.at(pointer) == 0)
   {
      instructionCount++;
      return;
   }

   static std::string ERROR_MESSAGE = "ERROR: Unbounded jump instruction; expected corresponding '[' but was not found";  //Error message

   int openBracketsCount = 0;                //Keeps track of how many open brackets have been found
   int closedBracketsCount = 0;              //Keeps track of how many closed brackets have been found
   bool hasMatchingBracketBeenFound = false; //Keeps track of whether or not a matching bracket has been found

   //Iterate through each value in the instruction counter to find the corresponding [
   for (unsigned int count = instructionCount; count > 0 && !hasMatchingBracketBeenFound; count--)
   {
      //Increment the bracket counters as needed to find corresponding closing bracket
      if (instructions.at(count) == JUMP_START_INSTRUCTION)
      {
         openBracketsCount++;
      }
      else if (instructions.at(count) == JUMP_END_INSTRUCTION)
      {
         closedBracketsCount++;
      }

      //Once the brackets have been balanced, then instructions.at(count) points to the corresponding closing bracket for instructions.at(pointer)
      if (openBracketsCount == closedBracketsCount)
      {
         //Since the base case was tested at the top of the function, it is already known that the current is cell is not 0, so the program must jump to the corresponding [
         instructionCount = count;

         //Set found flag to true
         hasMatchingBracketBeenFound = true;
      }
   } //end for

   //Return an error message and quit program if no matching [ was found
   if (!hasMatchingBracketBeenFound)
   {
      std::cout << ERROR_MESSAGE << std::endl;
      exit(EXIT_FAILURE);
   }
} //end function jumpEnd