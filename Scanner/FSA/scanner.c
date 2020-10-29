/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;
CharCode currentCharCode;
extern CharCode charCodes[];
int state;

void printToken(Token *token);
/***************************************************************/

void skipBlank() {
  // TODO
  // Skip until currentChar != ' '
  while( readChar() == ' ');
  
  return;
}

void skipComment() {
  // TODO
  int ln,cn;
  ln = lineNo;
  cn = colNo;
  readChar();
  while(currentChar != EOF && state != 5){
    if(state == 3){
      if(charCodes[currentChar] == CHAR_TIMES) state = 4;
    }
    else if(state == 4){
      if(charCodes[currentChar] == CHAR_RPAR) state = 5;
      else if(charCodes[currentChar] == CHAR_TIMES) state = 4;
      else state = 3;
    }
    readChar();
  }
  if(state == 5 ) return;
  else error(ERR_ENDOFCOMMENT,ln,cn); // errorMessage for END_OF_COMMENT
}

Token* readIdentKeyword(void) {
  // TODO
  int count = 0;
  int ln = lineNo;
  int cn = colNo;
  char *string = malloc(sizeof(char) * (MAX_IDENT_LEN +1));
  memset(string,'\0',sizeof(char) * (MAX_IDENT_LEN + 1));
  // Read all the digit and alpha characters
  while( isalpha(currentChar) != 0 || isdigit(currentChar) ){
      if(count < MAX_IDENT_LEN) string[count++] = currentChar;
      else count++;
      readChar();
  }

  if(count > MAX_IDENT_LEN){
    // Put error message when length of string larger than MAX_IDENT_LEN
    error(ERR_IDENTTOOLONG,ln,cn); // Error Message for INVALID_INDENT
    return makeToken(TK_NONE,ln,cn);
  }else if ( count <= MAX_IDENT_LEN ){
    TokenType tokenType = checkKeyword(string);
    Token *token;
    // Check if string is tokenType or not
    if(tokenType == TK_NONE){
      token =  makeToken(TK_IDENT,ln,cn);
      strcpy(token->string,string);
    }else token = makeToken(tokenType,ln,cn);
    return token;
  }
}

Token* readNumber(void) {
  // TODO
  //MAX_NUMBER_LENGTH = 10
  int ln = lineNo;
  int cn = colNo; 
  char *numString = malloc(sizeof(char) *(MAX_NUMBER_LENGTH+1));
  memset(numString,'\0',sizeof(char) * (MAX_NUMBER_LENGTH+1));
  int count = 0;
  numString[count++] = currentChar;
  while(isdigit(readChar())){
    if(count < MAX_NUMBER_LENGTH ) numString[count++] = currentChar;
    else count++;
  }
  long long toNumber = abs(atoi(numString));
  toNumber = toNumber < 0 ? -toNumber : toNumber;
  if(count > MAX_NUMBER_LENGTH || toNumber  > __INT_MAX__){
    error(ERR_INVALIDNUMBER,ln,cn);
    return makeToken(TK_NONE,ln,cn);
  }
  
  Token *token = makeToken(TK_NUMBER,ln,cn);
  memset(token->string,'\0',sizeof(char) * (MAX_IDENT_LEN + 1));
  strncpy(token->string,numString,MAX_NUMBER_LENGTH);
  free(numString);
  return token;
}

Token* readConstChar(void) {
  // TODO
  int ln = lineNo;
  int cn = colNo; 
  Token *token;
  int tempChar;
  while(1){
    if(state == 34 ){
      if(isprint(readChar()) != 0){
        state = 35;
        tempChar = currentChar;
      }
      else{
        error(ERR_INVALIDCHARCONSTANT,ln,cn);
        return makeToken(TK_NONE,ln,cn);
      }
    }else if(state == 35){
      if(charCodes[readChar()] == CHAR_SINGLEQUOTE){
        state = 36;
        token = makeToken(TK_CHAR,ln,cn);
        memset(token->string,'\0',sizeof(char)*MAX_IDENT_LEN);
        token->string[0] = tempChar;
        return token;
      }else{
        error(ERR_INVALIDCHARCONSTANT,ln,cn);
        return makeToken(TK_NONE,ln,cn);
      }
    }
  }
}

Token* getToken(void) {
  Token *token;
  int ln, cn;
  ln = lineNo;
  cn = colNo;
  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);
  while(1){
    switch(state){
      case 0:
        if (currentChar == EOF) 
          return makeToken(TK_EOF, lineNo, colNo);
        currentCharCode = charCodes[currentChar]; 
        switch(currentCharCode){
          case CHAR_SPACE:
            state = 1;
            break;
          case CHAR_LPAR:
            state = 2;
            break;
          case CHAR_LETTER:
            state = 8;
            break;
          case CHAR_DIGIT:
            state = 10;
            break;
          case CHAR_PLUS:
            state = 12;
            break;
          case CHAR_MINUS:
            state = 13;
            break;
          case CHAR_TIMES:
            state = 14;
            break;
          case CHAR_SLASH:
            state = 15;
            break;
          case CHAR_EQ:
            state = 16;
            break;
          case CHAR_COMMA:
            state = 17;
            break;
          case CHAR_SEMICOLON:
            state = 18;
            break;
          case CHAR_PERIOD:
            state = 19;
            break;
          case CHAR_GT:
            state = 22;
            break;
          case CHAR_LT:
            state = 25;
            break;
          case CHAR_EXCLAIMATION:
            state = 28;
            break;
          case CHAR_COLON:
            state = 31;
            break;
          case CHAR_SINGLEQUOTE:
            state = 34;
            break;
          case CHAR_RPAR:
            state = 39;
            break;
          default:
            state = 38;
            break;
        }
        break;
      case 1:
        skipBlank();
        state = 0;
        break;
      case 2:
        ln = lineNo;
        cn = colNo;
        readChar();
        if(charCodes[currentChar] == CHAR_PERIOD)
          state = 6;
        else if(charCodes[currentChar] == CHAR_TIMES){
          state = 3; 
        }else state = 7;
        break;
      case 3:
        skipComment();
        //readChar();
        state = 0;
        break;  
      case 6:
        readChar();
        return makeToken(SB_LSEL,ln,cn);
        break;
      case 7:
        return makeToken(SB_LPAR,ln,cn);
        break;
      case 8:
        token = readIdentKeyword();
        if(token->tokenType != TK_NONE) state = 9;
        else return token;
        break;
      case 9:
        return token; // TOKEN IDENTITY
        break;
      case 10:
        token = readNumber();
        if(token->tokenType != TK_NONE) state = 11;
        else return token;
        break;
      case 11:
        return token; // TOKEN NUMBER
        break;
      case 12:
        token =  makeToken(SB_PLUS,lineNo,colNo);
        readChar();
        return token;
        break;
      case 13:
        token =  makeToken(SB_MINUS,lineNo,colNo);
        readChar();
        return token;
        break;
      case 14:
        token = makeToken(SB_TIMES,lineNo,colNo);
        readChar();
        return token;
        break;
      case 15:
        token = makeToken(SB_SLASH,lineNo,colNo);
        readChar();
        return token;
        break;
      case 16:
        token =  makeToken(SB_EQ,lineNo,colNo);
        readChar();
        return token;
        break;
      case 17:
        token = makeToken(SB_COMMA,lineNo,colNo);
        readChar();
        return token;
        break;
      case 18:
        token = makeToken(SB_SEMICOLON,lineNo,colNo);
        readChar();
        return token;
        break;
      case 19:
        readChar();
        if(charCodes[currentChar] == CHAR_RPAR)
          state = 20;
        else state = 21;
        break;
      case 20:
        readChar();
        return makeToken(SB_RSEL,ln,cn);
        break;
      case 21:
        return makeToken(SB_PERIOD,ln,cn);
        break;
      case 22:
        if(charCodes[readChar()] == CHAR_EQ)
          state = 23;
        else state = 24;
        break;
      case 23:
        readChar();
        return makeToken(SB_GE,ln,cn);
        break;
      case 24:
        return makeToken(SB_GT,lineNo,colNo);
        break;
      case 25:
        if(charCodes[readChar()] == CHAR_EQ)
          state = 26;
        else state = 27;
        break;
      case 26:
        readChar();
        return makeToken(SB_LE,ln,cn);
        break;
      case 27:
        return makeToken(SB_LT,lineNo,colNo);
        break;
      case 28:
        ln = lineNo;
        cn = colNo;
        if(charCodes[readChar()] == CHAR_EQ)
          state = 29;
        else state = 30;
        break;
      case 29:
        readChar();
        return makeToken(SB_NEQ,ln,cn);
        break;
      case 30:
        error(ERR_INVALIDSYMBOL,ln,cn);
        return makeToken(TK_NONE,ln,cn);
        break;
      case 31:
        ln = lineNo;
        cn = colNo;
        readChar();
        if(charCodes[currentChar] == CHAR_EQ)
          state = 32;
        else state = 33;
        break;
      case 32:
        readChar();
        return makeToken(SB_ASSIGN,ln,cn);
        break;
      case 33:
        return makeToken(SB_COLON,ln,cn);
        break;
      case 34:
        token = readConstChar();
        if(token->tokenType == TK_NONE){
          return token;
        }
        break;
      case 36:
        readChar();
        return token; // TOKEN CHAR
        break;
      case 38:
        error(ERR_INVALIDSYMBOL,ln,cn);
        readChar();
        return makeToken(TK_NONE,ln,cn);
        break;
      case 39:
        token = makeToken(SB_RPAR, lineNo, colNo);
        readChar(); 
        return token;
        break;
    }
  }
}


/******************************************************************/



int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;
  state = 0;
  token = getToken();
  while(token->tokenType != TK_EOF){
    printToken(token);
    state = 0;
    token = getToken();
  }
  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {

  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}


void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}