/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  Type *checkType = (Type*) malloc(sizeof(Type));
  checkType->typeClass = type->typeClass;
  if(checkType->typeClass == TP_ARRAY){
    checkType->arraySize = type->arraySize;
    checkType->elementType = duplicateType(type->elementType);
  }
  return checkType;
  // TODO
}

int compareType(Type* type1, Type* type2) {
  // TODO
  if(type1->typeClass == type2->typeClass){
    if(type1->typeClass == TP_ARRAY){
      if(type1->arraySize == type2->arraySize)
        return compareType(type1->elementType,type2->elementType);
      return 0;
    }else return 1;
  }else return 0;
}

void freeType(Type* type) {
  // TODO
  if(type->typeClass == TP_ARRAY){
    freeType(type->elementType);
    free(type);
  }else free(type);
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  // TODO
  ConstantValue *value = (ConstantValue *) malloc(sizeof(ConstantValue));
  value->intValue = i;
  value->type = TP_INT;
  return value;
}

ConstantValue* makeCharConstant(char ch) {
  // TODO
    ConstantValue *value = (ConstantValue *) malloc(sizeof(ConstantValue));
    value->charValue = ch;
    value->type = TP_CHAR;
    return value;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  // TODO
  ConstantValue *dupConstValue = (ConstantValue *) malloc(sizeof(ConstantValue));
  dupConstValue->type = v->type;
  if(v->type == TP_INT) dupConstValue->intValue = v->intValue;
  else dupConstValue->charValue = v->charValue;
  return dupConstValue;
}

/******************* Object utilities ******************************/

Scope* createScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* createProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program,NULL);
  symtab->program = program;
  return program;
}

Object* createConstantObject(char *name) {
  // TODO
  Object *obj = (Object *) malloc(sizeof(Object));
  strcpy(obj->name,name);
  obj->kind = OBJ_CONSTANT;
  obj->constAttrs = (ConstantAttributes *) malloc(sizeof(ConstantAttributes));
  return obj;
}

Object* createTypeObject(char *name) {
  // TODO
  Object *obj = (Object *) malloc(sizeof(Object));
  strcpy(obj->name,name);
  obj->kind = OBJ_TYPE;
  obj->typeAttrs = (TypeAttributes *) malloc(sizeof(TypeAttributes));
  return obj;
}

Object* createVariableObject(char *name) {
  // TODO
  Object *obj = (Object *) malloc(sizeof(Object));
  strcpy(obj->name,name);
  obj->kind = OBJ_VARIABLE;
  obj->varAttrs = (VariableAttributes *) malloc(sizeof(VariableAttributes));
  obj->varAttrs->scope = symtab->currentScope;
  return obj;
}

Object* createFunctionObject(char *name) {
  // TODO
  Object *obj = (Object *) malloc(sizeof(Object));
  strcpy(obj->name,name);
  obj->kind = OBJ_FUNCTION;
  obj->funcAttrs = (FunctionAttributes *) malloc(sizeof(FunctionAttributes));
  obj->funcAttrs->scope = createScope(obj,symtab->currentScope);
  return obj;
}

Object* createProcedureObject(char *name) {
  // TODO
  Object *obj = (Object *) malloc(sizeof(Object));
  strcpy(obj->name,name);
  obj->kind = OBJ_PROCEDURE;
  obj->procAttrs = (ProcedureAttributes *) malloc(sizeof(ProcedureAttributes));
  obj->procAttrs->paramList = NULL;
  obj->procAttrs->scope = createScope(obj,symtab->currentScope);
  return obj;
}

Object* createParameterObject(char *name, enum ParamKind kind, Object* owner) {
  // TODO
  Object *obj = (Object *) malloc(sizeof(Object));
  strcpy(obj->name,name);
  obj->kind = OBJ_PARAMETER;
  obj->paramAttrs = (ParameterAttributes *) malloc(sizeof(ParameterAttributes));
  obj->paramAttrs->kind = kind;
  obj->paramAttrs->function = owner;
  return obj;
}

void freeObject(Object* obj) {
  // TODO
  switch(obj->kind){
    case OBJ_CONSTANT:
    free(obj->constAttrs->value);
    free(obj->constAttrs);
    break;
    case OBJ_FUNCTION:
    freeType(obj->funcAttrs->returnType);
    freeScope(obj->funcAttrs->scope);
    freeObjectList(obj->funcAttrs->paramList);
    free(obj->funcAttrs);
    break;
    case OBJ_PARAMETER:
    freeType(obj->paramAttrs->type);
    free(obj->paramAttrs);
    break;
    case OBJ_PROCEDURE:
    freeObjectList(obj->procAttrs->paramList);
    freeScope(obj->procAttrs->scope);
    free(obj->procAttrs);
    break;
    case OBJ_PROGRAM:
    freeScope(obj->progAttrs->scope);
    free(obj->progAttrs);
    break;
    case OBJ_TYPE:
    freeType(obj->typeAttrs->actualType);
    free(obj->typeAttrs);
    break;
    case OBJ_VARIABLE:
    freeScope(obj->varAttrs->scope);
    freeType(obj->varAttrs->type);
    free(obj->varAttrs);
    break;
  }
  free(obj);
}

void freeScope(Scope* scope) {
  // TODO
  freeObjectList(scope->objList);
  free(scope);
}

void freeObjectList(ObjectNode *objList) {
  // TODO
  
  while(objList != NULL){
    ObjectNode *tmp = objList; 
    objList = objList->next;
    free(tmp->object);
    free(tmp);
  }
}

void freeReferenceList(ObjectNode *objList) {
  // TODO
  while(objList != NULL){
    ObjectNode *tmp = objList; 
    objList = objList->next;
    free(tmp);
  }
}

void addObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  // TODO
  while(strcmp(objList->object->name,name) != 0){
    objList = objList->next;
  }
  return objList->object;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  addObject(&(symtab->currentScope->objList), obj);
}


