Token* readConstChar(void) {
  // TODO
  int ln = lineNo;
  int cn = colNo; 
  Token *token;
  int isPrint = isprint(readChar());
  char string[2]="\0";
  if(currentChar != 92 ) string[0] = currentChar; 
  else{
      readChar();
      if(currentChar == 39)
        string[0] = currentChar;
      else{
        token = makeToken(TK_NONE,ln,cn);
        error(ERR_INVALIDCHARCONSTANT,ln,cn);
        readChar();
        return token;
      }
  }
  
  readChar();
  if(isPrint && currentChar == 39){
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