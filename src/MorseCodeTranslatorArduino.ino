// using wire.h for I2C communication protocol. 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// creating a LCD object from LiquidCrystal_I2C.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timing thresholds for dot, character, and word detection.
const unsigned int is_dot  = 300;
const unsigned int is_char  = 900;
const unsigned int is_word  = 3000;

// Variables for measuring button press duration and input timing.
unsigned long pressStart = 0;
unsigned long lastInputTime = 0;

// Button pin and state variables for edge detection.
const byte btn = 3;
bool btnState;
bool lastBtnState = HIGH;


String currentMorse = "";      // Stores the current Morse code sequence being entered.
String translatedText = "";    // Stores the translated text.   

bool wordAdded = false;        // Prevents adding multiple spaces after a word.


//Morse dictionary.
char decodeMorse(String m) {
  if (m == ".-") return 'A';
  if (m == "-...") return 'B';
  if (m == "-.-.") return 'C';
  if (m == "-..") return 'D';
  if (m == ".") return 'E';
  if (m == "..-.") return 'F';
  if (m == "--.") return 'G';
  if (m == "....") return 'H';
  if (m == "..") return 'I';
  if (m == ".---") return 'J';
  if (m == "-.-") return 'K';
  if (m == ".-..") return 'L';
  if (m == "--") return 'M';
  if (m == "-.") return 'N';
  if (m == "---") return 'O';
  if (m == ".--.") return 'P';
  if (m == "--.-") return 'Q';
  if (m == ".-.") return 'R';
  if (m == "..." ) return 'S';
  if (m == "-" ) return 'T';
  if (m == "..-" ) return 'U';
  if (m == "...-" ) return 'V';
  if (m == ".--" ) return 'W';
  if (m == "-..-" ) return 'X';
  if (m == "-.--" ) return 'Y';
  if (m == "--.." ) return 'Z';
  return '?';
}

void setup()
{
    pinMode(btn, INPUT_PULLUP); // btn is pushed (LOW), released (HIGH).

    lcd.init();
    lcd.backlight();

    lcd.clear();

    // Show a blinking cursor at the current typing position.
    lcd.cursor();
    lcd.blink();

    lcd.setCursor(0,0);
}

//reding signal from btn.
void readButton()
{
    btnState = digitalRead(btn);

    if(btnState == LOW && lastBtnState == HIGH) {pressStart = millis();}

    if(btnState == HIGH && lastBtnState == LOW)
    {
        unsigned long pressTime = millis() - pressStart;

        if(pressTime < is_dot)
            currentMorse += ".";
        else if (pressTime < 1000)
            currentMorse += "-";
        else
            deleteLastChar();

        refreshLCD(); // Update the LCD with the latest content.

        lastInputTime = millis();
    }

    lastBtnState = btnState; // Store the current button state for the next iteration.
}

//replacing morse with character.
void checkCharacterGap()
{
    if(currentMorse.length() == 0)
        return; // Exit if there is no Morse code to decode.

    if(millis() - lastInputTime < is_char)
        return; // Wait until the character timeout has elapsed.

    char decoded = decodeMorse(currentMorse);
    translatedText += decoded;
    currentMorse = "";  
    /* Decode the entered Morse sequence into a character,
    append the decoded character to the translated text & 
    Clear the current Morse sequence for the next character. */

    refreshLCD();
}

//seperating words.
void checkWordGap()
{
    // Ignore word detection until at least one character exists.
    if(translatedText.length() == 0) {return;}
    // Reset the word flag when a new character is being entered.
    if(currentMorse.length() > 0) {wordAdded = false; return;}

    if(millis() - lastInputTime < is_word) {return;}

    // Add a space only once for each completed word.
    if(wordAdded == false) {
        translatedText += " "; // Append a space to separate words.
        
        refreshLCD();

        wordAdded = true; // Prevent multiple spaces from being added.
    }
}

//deleting the last char written.
void deleteLastChar()
{   
    // Check if the user is still entering a Morse character & Remove the last entered Morse symbol.
    if(currentMorse.length() > 0) {currentMorse.remove(currentMorse.length() - 1);}
    // Otherwise remove the last translated character from the translated text.
    else if(translatedText.length() > 0) {translatedText.remove(translatedText.length() - 1);}

    refreshLCD();

    lastInputTime = millis(); // Reset the input timer after editing.
}

void refreshLCD()
{
    // Combine translated text and current Morse input for display.
    String display = translatedText + currentMorse;

    lcd.clear();
    // Checking if the text fits on the LCD, Display the whole text from the beginning.
    if(display.length() <= 16) {lcd.setCursor(0,0); lcd.print(display);}
    else
    // Show only the last 16 characters when the text is longer than the LCD width.
    {
        lcd.setCursor(0,0);
        lcd.print(display.substring(display.length() - 16));
    }
}

void loop() 
{
    readButton();
    checkCharacterGap();
    checkWordGap();
}
