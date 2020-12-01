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

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  // TODO
  // Skip until currentChar != ' '
  while( readChar() == ' '){
  }
  return;
}

void skipComment() {
  // TODO
  readChar();
  char previousChar = currentChar; 
  while(currentChar != EOF){ 
    // Read all comment content until get the *) if not it will read until end of file 
    if(currentChar == '/' && previousChar == '*' ){
      readChar();
      return;
    }
    previousChar = currentChar;
    readChar();
  }
  // When get the EOF char, put the error message
  error(ERR_ENDOFCOMMENT,lineNo,colNo);
}

void skipComment2(){
	while(currentChar != '\n')
			readChar();
}

Token* readIdentKeyword(void) {
  // TODO
  int count = 0;
  int ln = lineNo;
  int cn = colNo;
  char *string = malloc(sizeof(char) * (1000));
  memset(string,'\0',sizeof(char) * (1000));
  string[count++] = currentChar;
  readChar();
  // Read all the digit and alpha character
  while( isalpha(currentChar) != 0 || isdigit(currentChar) ){
      string[count++] = currentChar;
      readChar();
  }

  if(count > MAX_IDENT_LEN){
    // Put error message when length of string larger than MAX_IDENT_LEN
    error(ERR_IDENTTOOLONG,ln,cn);
    return makeToken(TK_NONE,ln,cn);
  }
  else if ( count <= MAX_IDENT_LEN ){
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
  int isPrint = isprint(readChar());
  char string[2]="\0";
  string[0] = currentChar;
  if(readChar() == 39 && isPrint){
    token = makeToken(TK_CHAR,ln,cn);
    strcpy(token->string,string);
    readChar();
    return token;
  }else{
    token = makeToken(TK_NONE,ln,cn);
    error(ERR_INVALIDCHARCONSTANT,ln,cn);
    if(currentChar == 39) readChar();
    return token;
  }
}

Token* getToken(void) {
  Token *token;
  int ln, cn;
  ln = lineNo;
  cn = colNo;
  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
    case CHAR_SPACE: skipBlank(); return getToken();
    case CHAR_LETTER: return readIdentKeyword();
    case CHAR_DIGIT:  return readNumber();
    case CHAR_PERIOD:
      readChar();
      if(currentChar == ')'){
        token = makeToken(SB_RSEL,ln,cn);
        readChar();
        return token;
      }else{
        token = makeToken(SB_PERIOD,ln,cn);
        return token;
      }
    case CHAR_GT:
      readChar();
      if(currentChar == '='){
        token = makeToken(SB_GE,ln,cn);
        readChar();
        return token;
      }else{
        token = makeToken(SB_GT,ln,cn);
        return token;
      }
      case CHAR_COLON:
       readChar();
       if(currentChar == '=' ){
         token = makeToken(SB_ASSIGN,ln,cn);
         readChar();
         return token;
       }else{
         token = makeToken(SB_COLON,ln,cn);
         //readChar();
         return token;
       }
       case CHAR_LT:
        readChar();
        if(currentChar == '=' ){
          token = makeToken(SB_LE,ln,cn);
          readChar();
          return token;
        }else{
          token = makeToken(SB_LT,ln,cn);
          //readChar();
          return token;
        }
       
    // 1 case CHAR
    case CHAR_PLUS: 
      token = makeToken(SB_PLUS, lineNo, colNo);
      readChar(); 
      return token;
    case CHAR_MINUS: 
      token = makeToken(SB_MINUS, lineNo, colNo);
      readChar(); 
      return token;
    case CHAR_TIMES: 
      token = makeToken(SB_TIMES, lineNo, colNo);
      readChar();
      return token;
      case CHAR_SLASH:
		readChar();
		if( currentChar == '*'){
			skipComment(); return getToken();
		}else if(currentChar == '/') skipComment2(); return getToken();
      token = makeToken(SB_SLASH, lineNo, colNo);
      readChar();
      return token;
    case CHAR_EQ: 
      token = makeToken(SB_EQ, lineNo, colNo);
      readChar(); 
      return token;
    case CHAR_COMMA: 
      token = makeToken(SB_COMMA, lineNo, colNo);
      readChar(); 
      return token;
    case CHAR_SEMICOLON: 
      token = makeToken(SB_SEMICOLON, lineNo, colNo);
      readChar(); 
      return token;
    case CHAR_RPAR: 
      token = makeToken(SB_RPAR, lineNo, colNo);
      readChar(); 
      return token;
    case CHAR_EXCLAIMATION:
      if(readChar() == '='){
        token = makeToken(SB_NEQ, ln, cn);
        readChar(); 
        return token;
      }else{
        error(ERR_INVALIDSYMBOL,ln,cn);
        token = makeToken(TK_NONE, ln, cn);
        return token;
      }
    case CHAR_SINGLEQUOTE:
      return readConstChar();
    case CHAR_LPAR:
       readChar();
       if(currentChar == '.'){
        token = makeToken(SB_LSEL,ln,cn);
        readChar();
        return token;
       }else{
         token = makeToken(SB_LPAR,ln,cn);
         return token;
       }
       break;
    default:
      token = makeToken(TK_NONE, lineNo, colNo);
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
      readChar(); 
      return token;
  }
}


/******************************************************************/

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

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();

  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
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