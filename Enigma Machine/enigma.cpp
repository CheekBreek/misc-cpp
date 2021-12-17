/******************************************************************************

Source File: enigma.cpp

Purpose:

    This program is a simulation of a simplified version of the German world
    war two encryption machine named Enigma. 

Details:
    The simulation is simplified in that it only uses two rotors instead of the
    minimum of three. The initial translation settings for both rotors and the
    reflector are read from a file.

    The program prompts for the names of five files for:
         - the two rotors
         - a reflector
         - an input data file
         - an output result file

    This program preserves a very important property of the Enigma machine:
    decryption is done by running the enigma with the encoded message.

Input files:

    The rotor and reflector files specify a permutation of the 26 lower case 
    letters by giving the result of applying the permutation to the 26 
    letters listed in alphabetical order. 

    A valid reflector must be be composed of thirteen 2-cycles, 
          i.e. letter1 -> letter2 and letter2 -> letter1

    The input data file contains a "message"  to be encrypted or decrypted.
    It must consist entirely of lower case alphabetic characters, spaces, and
    newlines.

Output:

   The output result file contains the message resulting from the encrypted or
   decrypted of the input message.

******************************************************************************/

#include <iostream>
#include <fstream>
using namespace std;

const int ARRAY_SIZE=26;
const int FILENAME_LEN = 80;
const char LITTLE_A = 'a';
const char LITTLE_Z = 'z';

bool loadArray(int array[], const char filename[]);
bool loadRotor(int rotor[], const char filename[]);
bool loadReflector(int reflect[], const char filename[]);
void showTranslation(const int array[]);
char intToChar (int value);
int charToInt (char letter);
char translateLetter (int index, int shift);
char indexToLetter(int index);
char lookupForward(char letter, const int translation[]);
char lookupBackward(char letter, const int translation[]);
void rotateRotor(int rotor[]);

int main()
{
  int rotorOne[ARRAY_SIZE];      // the loadable components of the Enigma
  int rotorTwo[ARRAY_SIZE];
  int reflector[ARRAY_SIZE];

  int rotation = 0;  // the counter for rotating the rotors
  char ch;           // the character to be encrypted

  char rotorOneFileName[FILENAME_LEN];
  char rotorTwoFileName[FILENAME_LEN];
  char reflectorFileName[FILENAME_LEN];
  char plainFileName[FILENAME_LEN];
  char cypherFileName[FILENAME_LEN];

  ifstream infile;      // for reading the input text
  ofstream outfile;     // for writing the output text

  cout << "Enter the file name for the 1st rotor: ";
  cin >> rotorOneFileName;
  cout << "Enter the file name for the 2nd rotor: ";
  cin >> rotorTwoFileName;
  cout << "Enter the file name for the reflector: ";
  cin >> reflectorFileName;
  cout << "Enter the file name for the plain text: ";
  cin >> plainFileName;
  cout << "Enter the file name for the cypher text: ";
  cin >> cypherFileName;

  infile.open(plainFileName);

  if (infile.fail())
  {
      cout << "Could not open file: " << plainFileName
	   << " for input." << endl;
      cout << "Exiting program." << endl;
  }
  else
  {
      outfile.open(cypherFileName);

      if (outfile.fail())
      {
	  cout << "Could not open file: " << cypherFileName
	       << " for output." << endl;
	  cout << "Exiting program." << endl;
	  infile.close();
      }
      else
      {
          if (!loadRotor(rotorOne, rotorOneFileName))
          {
              cout << "Problem with " << rotorOneFileName << endl;
              cout << "Exiting program." << endl;
              infile.close();
              outfile.close();
          }
          else
          {
              if (!loadRotor(rotorTwo, rotorTwoFileName))
              {
                  cout << "Problem with " << rotorTwoFileName << endl;
                  cout << "Exiting program." << endl;
                  infile.close();
                  outfile.close();
              }
              else
              {
                  if (!loadReflector(reflector, reflectorFileName))
                  {
                      cout << "Problem with " << reflectorFileName << endl;
                      cout << "Exiting program." << endl;
                      infile.close();
                      outfile.close();
                  }
                  else
                  {
                      infile.get(ch);

                      while (!infile.eof())
                      {
                          if (ch != ' ' && ch != '\n')
                          {
			      ch = lookupForward(ch,rotorOne);
			      ch = lookupForward(ch,rotorTwo);
			      ch = lookupForward(ch,reflector);
			      ch = lookupBackward(ch,rotorOne);
			      ch = lookupBackward(ch,rotorTwo);
			      rotateRotor(rotorOne);
			      rotation++;
			      if(rotation == ARRAY_SIZE)
			      {
				  rotateRotor(rotorTwo);
				  rotation = 0;
			      }
                          }
                          outfile.put(ch);
                          infile.get(ch);
		      }
                      infile.close();
                      outfile.close();

                      cout << "Encryption successfully completed." << endl;
                  }
              }
          }
      }
  }

  return 0;
}


/******************************************************************************

Name:     showTranslation

Purpose:  Displays to the screen the contents of an array of 26 integers but by,
          showing:
            - the array indices as if they are characters starting at 'a'
            - the array element contents as a translation character, meaning
              if the index character is input then it would convert to the 
              translation character, ex. if the contents of array element 0
              were 23 then the output would be:
                     a ...
                     | |||
                     x ...

Input Parameters:  An array of exactly 26 characters.

******************************************************************************/

void showTranslation(const int array[])
{
  char ch;     // the current character to write
  int index;   // index into the array

  for (ch = LITTLE_A; ch <= LITTLE_Z; ch = intToChar(charToInt(ch) + 1))
    cout << ch << " ";

  cout << endl;

  for (ch = LITTLE_A; ch <= LITTLE_Z; ch = intToChar(charToInt(ch) + 1))
    cout << "| ";

  cout << endl;

  index = 0;

  for (index = 0; index < ARRAY_SIZE; index++)
  {
    cout << translateLetter(index, array[index])  << " ";
  }

  cout << endl;

  return;
}

char intToChar (int value)
{
  char letter;

  letter = static_cast <char> (value);

  return letter;
}

int charToInt (char letter)
{
  int number;

  number = static_cast<int>(letter);

  return number;
}

/******************************************************************************

Name:     translateLetter

Purpose:  Given a starting letter in the form of an index and an integer 
          difference, i.e. a shift, computes the translation character resulting
          from this combination.

Details:  The starting letter is generated by adding the index with the 
          character 'a'.
          The translation character is index + 'a' + shift

Input Parameters:
          An array index between 0 and 25, which represents a letter.
          An integer difference, the shift between the starting letter, the 
          index, and the translation character.

Returns;  The translation character, a lower case letter between 'a' and 'z'

******************************************************************************/

char translateLetter (int index, int shift)
{
  char ch;

  ch = intToChar (index + charToInt(LITTLE_A) + shift);

  return ch;
}

/******************************************************************************

Name:     indexToLetter

Purpose:  Translates a numeric array index into the corresponding letter that
          this index represents.

Details:  C++ only allows numeric array indices, so this function converts 
          a numeric index into an alphabetic index. This is done by 
          starting letter is generated by adding the character 'a' to 
          the numeric array index.
          'a' + 0 = 'a'
          'a' + 1 = 'b'

          This assumes that ASCII character encoding is being used. 
          This works because ASCII characters are consecutive, in
           alphabetic order starting with the 'a'

Input Parameters:
          An array index between 0 and 25.

Returns:  The corresponding lower case letter.

******************************************************************************/

char indexToLetter(int index)
{
  char ch;

  ch = intToChar(index + charToInt(LITTLE_A));

  return ch;
} 

/******************************************************************************

Name:     loadArray

Purpose:  Populates an array with integer differences between two alphabetic
          letters.

Details:  The two letters are a starting letter and a translation letter.
          The starting letter is current index into the array. The translation
          letter comes from a file, which must contain at least 26 
          non-whitespace characters.
          The integer difference is the translation letter - the starting letter

Input Parameters:  The name of the input file from which to read characters.

Output Parameters: An array populated with the contents of the file.

Returns:  The status of the (attempted) file opening.

******************************************************************************/

bool loadArray(int array[], const char filename[])
{
  ifstream fin;
  char ch;
  int index;

  fin.open(filename);

  if (!fin.fail())
  {
    for(index = 0; index < ARRAY_SIZE; index++)
    {
      fin >> ch;
      array[index] = charToInt(ch) - (charToInt(LITTLE_A) + index);
    }

    fin.close();
  }
  return !fin.fail();
}


/******************************************************************************

Name:     loadReflector

Purpose:  Populates and validates a reflector array with data from a file.

Details:  The file must contain at least 26 non-whitespace characters. 
          A reflector is valid:
           -  if for each plain text letter translates to a cipher letter and
              the same cipher letter translates to the original plain text 
              letter.
          AND
           - no letter translates to itself.
          The function returns the success of both actions: the file existing
          and meeting the requirements of a reflector.

Input Parameters:  The name of the file from which to read.

Output Parameters: The reflector array, populated with numeric difference
                   between each input letter and the translation letter
                   from the file.

Returns:  The status of the (attempted) load operation.

******************************************************************************/

bool loadReflector(int reflector[], const char filename[])
{
  bool OK;     // the status of the load operation
  int index;   // index of the current plain text letter being checked
  int t_index; // index corresponding cipher text letter

  OK = loadArray(reflector, filename);

  if (OK)
  {
      index = 0;
      while (index < ARRAY_SIZE && OK)
      {
	  t_index = reflector[index] + index;

	  if (translateLetter(t_index, reflector[t_index]) != 
              indexToLetter(index) ||
              (reflector[index] == 0)) 
              OK = false;
          index++;
      }
  }

  return OK;
}


/******************************************************************************

Name:     loadRotor

Purpose:  Populates and validates a rotor array with data from a file. 
          The file must contain at least 26 non-whitespace characters. 
          < insert rotor validation rules >
          The function returns the success of both actions: the file existing
          and meeting the requirements of a rotor.

Input Parameters:  The name of the file from which to read.

Output Parameters: A rotor populated with the numeric difference between
                   each input letter and the translation letter, from the
                   file.

Returns:  The status of the (attempted) load operation.

Method: <complete>

******************************************************************************/

bool loadRotor(int rotor[], const char filename[])
{
  bool OK;     // the status of the load operation
  int index = 0, counter = 0;
  char ch;
  bool check[26];

  OK = loadArray(rotor, filename);

  if(OK)
  {
      while(counter < 26)
      {
	  ch = translateLetter(counter, rotor[counter]);
	  if(ch >= LITTLE_A && ch <= LITTLE_Z)
	  {
	      index = charToInt(ch) - charToInt(LITTLE_A);
	      check[index] = true;
	  }
	  counter++;
      }
      counter = 0;
      
      while(counter < 26 && OK == true)
      {
	  if(check[index] != true)
	  {
	      OK = false;
	      counter = 26;
	  }
	  counter++;
      }
  }
  
  return OK;
}

/******************************************************************************

Name:     lookupForward

Purpose:  Looks up the character after being translated forward into the rotor

Input Parameters:  The character being translated, the rotor used in the 
                   translation.

Returns:  The new character after the translation

******************************************************************************/

char lookupForward(char letter, const int translation[])
{
    char output = static_cast<char>(charToInt(letter) + translation[charToInt(letter) - charToInt(LITTLE_A)]);
    return output;
}

/******************************************************************************

Name:     lookupBackward

Purpose:  Looks up the character after being translated backward into the rotor

Input Parameters:  The character being translated, the rotor used in the
                   translation.

Returns:  The new character after the translation

******************************************************************************/

char lookupBackward(char letter, const int translation[])
{
    int counter = 0, temp;
    char output;
    bool check;
    while(counter < ARRAY_SIZE || check)
    {
	temp = charToInt(LITTLE_A) + counter;
	if(lookupForward(static_cast<char>(temp), translation) == letter)
	{
	    output = static_cast<char>(charToInt(letter) - translation[temp - charToInt(LITTLE_A)]);
	    check = false;
	}
	counter++;
    }
    return output;
}

/******************************************************************************

Name:     rotateRotor

Purpose:  Rotates the values in the inputted rotor to add further encrytion to
          the message

Input Parameters:  The rotor to be rotated

Returns:  The rotated rotor

******************************************************************************/

void rotateRotor(int rotor[])
{
    int counter = 0;
    int check, first = rotor[0];

    while(counter < ARRAY_SIZE)
    {
	if(counter == 25)
	{
	    rotor[counter] = first;
	}
	else
	{
	    rotor[counter] = rotor[counter + 1];	    
	}

	check = counter + rotor[counter];
	if(check < 0)
	{
	    rotor[counter] = ARRAY_SIZE + rotor[counter];
	}
	else if(check > 25)
	{
	    rotor[counter] = 0 - (ARRAY_SIZE - rotor[counter]);
	}
	counter++;
    }
}
