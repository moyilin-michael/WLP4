#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;
const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

bool checkInt(string n) {
  int res;
  if (n.length() > 1 && n[0] == '0') {
    return false;
  }
  if (n.length() < 10) {
    return true;
  }
  if (n.length() == 10) {
    if (n[0] < '2') {
      return true;
    }
    else if (n[0] > '2') {
      return false;
    }
    else {
      res = stoi(n.substr(1));
      if (res <= 147483647) {
        return true;
      }
      else {
        return false;
      }
    }
  }
  return false;
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
  std::istream& in = std::cin;
  std::string s;
  vector<char> alphabet;
  vector<string> accept;
  string initialst;
  vector<vector<string>> transitions;
  string temp = R"(.ALPHABET
a-z
A-Z
0-9
{ } [ ] ( ) = < ! > + - * / % , ; &
.STATES
st
ID!
NUM!
LPAREN!
RPAREN!
LBRACE!
RBRACE!
BECOMES!
EQ!
NE!
LT!
GT!
LE!
GE!
PLUS!
MINUS!
STAR!
SLASH!
PCT!
COMMA!
SEMI!
LBRACK!
RBRACK!
AMP!
exclaim
dslash
.TRANSITIONS
st 0-9 NUM
st { LBRACE
st } RBRACE
st [ LBRACK
st ] RBRACK
st ( LPAREN
st ) RPAREN
st = BECOMES
st < LT
st ! exclaim
st > GT
st + PLUS
st - MINUS
st * STAR
st / SLASH
SLASH / dslash
st % PCT
st , COMMA
st ; SEMI
st & AMP
BECOMES = EQ
LT = LE
exclaim = NE
GT = GE
NUM 0-9 NUM
st a-z A-Z ID
ID a-z A-Z 0-9 ID)";
  istringstream dfa(temp);

  std::getline(dfa, s); // Alphabet section (skip header)
  // Read characters or ranges separated by whitespace
  while(dfa >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        alphabet.push_back(s[0]);
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
          alphabet.push_back(c);
        }
      } 
    }
  }

  std::getline(dfa, s); // States section (skip header)
  // Read states separated by whitespace
  while(dfa >> s) {
    if (s == TRANSITIONS) { 
      break; 
    } else {
      static bool initial = true;
      bool accepting = false;
      if (s.back() == '!' && !isChar(s)) {
        accepting = true;
        s.pop_back();
      }
      //// Variable 's' contains the name of a state
      if (initial) {
        initialst = s;
        initial = false;
      }
      if (accepting) {
        accept.push_back(s);
      }
    }
  }

  std::getline(dfa, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(dfa, s)) {
    if (s.substr(0, 6) == INPUT) { 
      // Note: Since we're reading line by line, once we encounter the
      // input header, we will already be on the line after the header
      break; 
    } else {
      std::string fromState, symbols, toState, last;
      std::istringstream line(s);
      line >> fromState;
      line >> s;
      while (line >> last) {
        if (isChar(s)) {
          symbols += s;
        } else if (isRange(s)) {
          for(char c = s[0]; c <= s[2]; ++c) {
            symbols += c;
          }
        }
        s = last;
      }
      toState = s;
      for ( char c : symbols ) {
        vector<string> trans;
        trans.push_back(fromState);
        trans.push_back(to_string(c));
        trans.push_back(toState);
        transitions.push_back(trans);
      }
    }
  }
  // Input section (already skipped header)
  while (getline(in, s)) {
    bool jump = false;
    istringstream line(s);
    while (line >> s) {
      string cur = initialst;
      bool rej = true;
      string token = "";
      for (int i = 0; i < s.length(); ++i) {
        for (int j = 0; j < transitions.size(); ++j) {
          if (transitions[j][0] == cur && transitions[j][1] == to_string(s[i])) {
            token += s[i];
            cur = transitions[j][2];
            rej = false;
            break;
          }
        }
        if (cur == "dslash") {
          jump = true;
          break;
        }
        if (rej == true) {
          if (find(accept.begin(), accept.end(), cur) != accept.end()) {
            if (token == "delete") {
              cout << "DELETE ";
            }
            else if (token == "else") {
              cout << "ELSE ";
            }
            else if (token == "if") {
              cout << "IF ";
            }
            else if (token == "int") {
              cout << "INT ";
            }
            else if (token == "NULL") {
              cout << "NULL ";
            }
            else if (token == "new") {
              cout << "NEW ";
            }
            else if (token == "println") {
              cout << "PRINTLN ";
            }
            else if (token == "return") {
              cout << "RETURN ";
            }
            else if (token == "wain") {
              cout << "WAIN ";
            }
            else if (token == "while") {
              cout << "WHILE ";
            }
            else if (cur == "NUM") {
              if (checkInt(token)) {
                cout << "NUM ";
              }
              else {
                cerr << "ERROR" << endl;
                return 0;
              }
            }
            else {
              cout << cur << " ";
            }
            cout << token << endl;
            cur = initialst;
            token = "";
            i--;
          }
          else {
            cerr << "ERROR" << endl;
            return 0;
          }
        }
        rej = true;
      }
      if (jump) {
        break;
      }
      if (find(accept.begin(), accept.end(), cur) != accept.end()) {
        if (token == "delete") {
          cout << "DELETE ";
        }
        else if (token == "else") {
          cout << "ELSE ";
        }
        else if (token == "if") {
          cout << "IF ";
        }
        else if (token == "int") {
          cout << "INT ";
        }
        else if (token == "NULL") {
          cout << "NULL ";
        }
        else if (token == "new") {
          cout << "NEW ";
        }
        else if (token == "println") {
          cout << "PRINTLN ";
        }
        else if (token == "return") {
          cout << "RETURN ";
        }
        else if (token == "wain") {
          cout << "WAIN ";
        }
        else if (token == "while") {
          cout << "WHILE ";
        }
        else if (cur == "NUM") {
          if (checkInt(token)) {
            cout << "NUM ";
          }
          else {
            cerr << "ERROR" << endl;
            return 0;
          }
        }
        else {
          cout << cur << " ";
        }
        cout << token << endl;
      }
      else {
        cerr << "ERROR" << endl;
        return 0;
      }
    }
  }
}
