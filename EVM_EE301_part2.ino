#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Keypad.h>

#define Password_Length 8

char Data[Password_Length];
char Master[Password_Length] = "123A456";
byte data_count = 0, master_count = 0;
char customKey;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { A0, A1, A2, A3 };
byte colPins[COLS] = { 5, 4, 3, 2 };

Keypad keyp = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


// O_RDWR

LiquidCrystal lcd(11, 10, 9, 8, 7, 6);
int verified = -1;

int buzz = 13;

struct voter {
  int num;
  int voter_id;
  bool b;
};

struct candidate {
  int candidate_number;
  int votes;
};


void initialise_memory() {
  int address = 0;
  for (int i = 1; i <= 10; i++) {
    voter v;
    v.num = i;
    v.voter_id = (i * 100) + 7;
    v.b = 0;
    EEPROM.put(address, v);
    address += sizeof(voter);
  }
  for (int i = 1; i <= 5; i++) {
    candidate c;
    c.candidate_number = i;
    c.votes = 0;
    EEPROM.put(address, c);
    address += sizeof(candidate);
  }
}







// lcd interface functions
void setup_lcd(int voter_id) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voter Verified");
  delay(1000);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print("voter_id: " + String(voter_id));
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Candidate");
}





// sd card and fingerprint interface functions

int get_voterid() {
  char key = keyp.getKey();
  switch (key) {
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case '#':
      return 20;
    case '*':
      return 30;
    default:
      return 0;
  }
}


int check_identity(int finger_print) {
  int address = 0;
  for (int i = 1; i <= 10; i++) {
    voter v;
    EEPROM.get(address, v);
    if (v.num == finger_print && v.b == 0) {
      v.b = 1;
      EEPROM.put(address, v);
      return v.voter_id;
    }
    address += sizeof(voter);
  }
  return -1;
}


void update_memory(int candidate_number) {
  int address = 0;
  for (int i = 0; i < 10; i++) {
    address += sizeof(voter);
  }
  for (int i = 0; i < 5; i++) {
    candidate c;
    EEPROM.get(address, c);
    if (c.candidate_number == candidate_number) {
      c.votes += 1;
      EEPROM.put(address, c);
    }
    address += sizeof(candidate);
  }
}


// ballot interface functions

int fetch() {
  char key = keyp.getKey();
  if (key == 'A') {
    return 1;
  } else if (key == 'B') {
    return 2;
  } else if (key == 'C') {
    return 3;
  } else if (key == 'D') {
    return 4;
  } else {
    return 0;
  }
  /*
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Hold for: ");
     for(int i=0;i<10;i++){

        lcd.setCursor(11,0);
        lcd.print(String(10-i-1));
        delay(500);
       
       if(key!= key1){
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("Hold for 10s");
         delay(1000);
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("Press again");
         delay(1000);
         break;
       } 
     }*/
}

//check password
bool checkpassword()
{
    while(data_count !=7)
    {
        customKey = keyp.getKey();
        if (customKey)
        {
            Data[data_count] = customKey;
            lcd.setCursor(data_count, 1);
            lcd.print(Data[data_count]);
            data_count++;
        }
    }
    if (data_count == Password_Length - 1)
    {
        lcd.clear();
        Data[data_count] = '\0'; // Null-terminate the Data array to convert it to a string

        if (strcmp(Data, Master) == 0)
        {
            lcd.print("Correct");
            lcd.clear();
            data_count = 0;
            return true;
            // digitalWrite(signalPin, HIGH);
            delay(5000);
            // digitalWrite(signalPin, LOW);
        }
        else
        {
            lcd.print("Incorrect");
             digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(200);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(200);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(200);
        }

        lcd.clear();
        data_count = 0; // Reset the data_count for the next password entry
    }

    return false;
}

// tally function
void count_votes() {
  int address = 0;
  for (int i = 0; i < 10; i++) {
    address += sizeof(voter);
  }
  for (int i = 0; i < 5; i++) {
    candidate c;
    EEPROM.get(address, c);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Candidate No:" + String(c.candidate_number));
    lcd.setCursor(0, 1);
    lcd.print("No of Votes: " + String(c.votes));
    delay(1000);
    address += sizeof(candidate);
  }
}



void setup() {

  pinMode(buzz, OUTPUT);
  pinMode(12, OUTPUT);

  lcd.begin(16, 2);
  initialise_memory();
}


void loop() {

  lcd.print("   Welcome To ");
  lcd.setCursor(1, 1);
  lcd.print("Voting Machine");
  delay(1000);

  int voter_identity = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press And Hold");
  lcd.setCursor(1, 1);
  lcd.print("Your Finger");



  while (voter_identity == 0) {
    voter_identity = get_voterid();
  }

  lcd.clear();
  lcd.print("Lift Finger");
  delay(1000);

  if (voter_identity == 20) {
    lcd.clear();
    lcd.print("Enter password:");
    //delay(1000);
    if (checkpassword())
      count_votes();

  } else {
    verified = check_identity(voter_identity);

    if (verified == -1) {
      lcd.clear();
      lcd.print("INVALID VOTER");
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(200);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(200);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(200);
    }


    if (verified != -1) {
      setup_lcd(verified);
      int candidate_number = 0;
      while (true) {
        candidate_number = fetch();
        if (candidate_number != 0) {
          break;
        }
      }
      update_memory(candidate_number);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vote Casted");
      digitalWrite(buzz, HIGH);
      digitalWrite(12, HIGH);
      delay(1000);
      digitalWrite(buzz, LOW);
      digitalWrite(12, LOW);
      delay(1000);
      lcd.clear();
    }
  }
}
