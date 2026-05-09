#ifndef MYTOKEN_H
#define MYTOKEN_H
#include<stdlib.h>
#include<stdio.h>
typedef enum{
	TOKEN_VOID,
	
	TOKEN_EOF,
	TOKEN_WORD,
	TOKEN_NUMBER,
	TOKEN_STRING,
	TOKEN_CHAR,
	
	TOKEN_LBRACE,//{
	TOKEN_RBRACE,//}
	TOKEN_LPAREN,//(
	TOKEN_RPAREN,//)
	TOKEN_LBRACKET,//[
	TOKEN_RBRACKET,//]
	
	TOKEN_SEMICOLON,//;
	TOKEN_COMMA,//,
	TOKEN_COLON,//:
	
	TOKEN_OPERATOR,//+-*/...
	TOKEN_UNARY,//++,--...
	TOKEN_DOT,//.->
	
	TOKEN_PREPROC,//#
	TOKEN_COMMENT,// //
}tokenType;
typedef struct{
	tokenType T;
	int len;
	int size;
	char* text;
}token;
token* newToken(){
	token* tok=(token*)malloc(sizeof(token));
	if(tok==NULL){
		fprintf(stderr,"内存分配失败\n");
		return NULL;
	}
	char* s=(char*)malloc(sizeof(char)*10);
	if(s==NULL){
		free(tok);
		fprintf(stderr,"内存分配失败\n");
		return NULL;
	}
	tok->len=0;
	tok->size=10;
	tok->T=TOKEN_VOID;
	tok->text=s;
	return tok;
}
int pushToken(token* tok,char ch){
	if(tok->len>=tok->size-1){
		if(tok->size>=1342177280){
			fprintf(stderr,"字符串过长\n");
			return 2;
		}
		char* ptr=(char*)realloc(tok->text,tok->size*2);
		if(ptr==NULL){
			fprintf(stderr,"内存分配失败\n");
			return 1;
		}
		tok->text=ptr;
		tok->size*=2;
	}
	tok->text[tok->len]=ch;
	tok->text[tok->len+1]='\0';
	tok->len++;
	return 0;
}
void freeToken(token* tok){
	if(tok==NULL)return;
	free(tok->text);
	free(tok);
}
#endif
