#ifndef MYTOKEN_H
#define MYTOKEN_H
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<windows.h>
#define SAFE_PUSH_NEXT(tok,ch) do{\
	int ret=pushToken(tok,ch);\
	if(ret==1){\
		tok->T=TOKEN_ERR;\
		return 1;\
	}\
	if(ret==2){\
		tok->T=TOKEN_ERR;\
		return 2;\
	}\
	nextChar(rfp,pch);\
}while(0)
#define SAFE_PUSH(tok,ch) do{\
	int ret=pushToken(tok,ch);\
	if(ret==1){\
		tok->T=TOKEN_ERR;\
		return 1;\
	}\
	if(ret==2){\
		tok->T=TOKEN_ERR;\
		return 2;\
	}\
}while(0)
 //并非SAFE
typedef enum{
    TOKEN_VOID,TOKEN_ERR,TOKEN_EOF,TOKEN_WORD,TOKEN_NUMBER,TOKEN_STRING,TOKEN_CHAR,TOKEN_LBRACE, //{
    TOKEN_RBRACE, //}
    TOKEN_LPAREN, //(
    TOKEN_RPAREN, //)
    TOKEN_LBRACKET, //[
    TOKEN_RBRACKET, //]
    TOKEN_SEMICOLON, //;
    TOKEN_COMMA, //,
    TOKEN_COLON, //:
    TOKEN_OPERATOR, //+-*/...
    TOKEN_UNARY, //++,--...
    TOKEN_DOT, //.->
    TOKEN_PREPROC, //#
    TOKEN_COMMENT, // //
}
tokenType;
typedef struct{
    tokenType T;
    int len;
    int size;
    char*text;
}
token;
token*newToken(){
    token*tok=(token*)malloc(sizeof(token));
    if(tok==NULL){
        fprintf(stderr,"内存分配失败\n");
        return NULL;
    }
    char*s=(char*)malloc(sizeof(char)*10);
    if(s==NULL){
        free(tok);
        fprintf(stderr,"内存分配失败\n");
        return NULL;
    }
    tok->len=0;
    tok->size=10;
    tok->T=TOKEN_VOID;
    tok->text=s;
    tok->text[0]='\0';
    return tok;
}
int pushToken(token*tok,char ch){
    if(tok->len>=tok->size-1){
        if(tok->size>=1342177280){
            fprintf(stderr,"字符串过长\n");
            return 2;
        }
        char*ptr=(char*)realloc(tok->text,tok->size*2);
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
void freeToken(token*tok){
    if(tok==NULL)return;
    free(tok->text);
    free(tok);
}
void nextChar(FILE*rfp,int*pch){
    pch[0]=fgetc(rfp);
}
void skipWhiteSpace(FILE*rfp,int*pch){
    while(pch[0]!=EOF){
        if(pch[0]==' '||pch[0]=='\t'||pch[0]=='\r'||pch[0]=='\n'){
            nextChar(rfp,pch);
        }
        else break;
    }
}
int readIdentifier(FILE*rfp,int*pch,token*tok){
    while(isalnum(pch[0])||pch[0]=='_'){
        SAFE_PUSH_NEXT(tok,pch[0]);
    }
    tok->T=TOKEN_WORD;
    return 0;
}
int readNumber(FILE*rfp,int*pch,token*tok){
    while(isdigit(pch[0])||pch[0]=='.'||pch[0]=='X'||pch[0]=='x'||pch[0]=='E'||pch[0]=='e'||((pch[0]=='+'||pch[0]=='-')&&(tok->len>0&&(tok->text[tok->len-1]=='e'||tok->text[tok->len-1]=='E')))){
        SAFE_PUSH_NEXT(tok,pch[0]);
    }
    tok->T=TOKEN_NUMBER;
    return 0;
}
int readString(FILE*rfp,int*pch,token*tok){
    SAFE_PUSH_NEXT(tok,'"');
    while(pch[0]!='"'&&pch[0]!=EOF){
        if(pch[0]=='\\'){
            SAFE_PUSH_NEXT(tok,pch[0]);
        }
        SAFE_PUSH_NEXT(tok,pch[0]);
    }
    if(pch[0]=='"'){
        SAFE_PUSH_NEXT(tok,pch[0]);
    }
    tok->T=TOKEN_STRING;
    return 0;
}
int readChar(FILE*rfp,int*pch,token*tok){
    SAFE_PUSH_NEXT(tok,'\'');
    while(pch[0]!='\''&&pch[0]!=EOF){
        if(pch[0]=='\\'){
            SAFE_PUSH_NEXT(tok,pch[0]);
        }
        SAFE_PUSH_NEXT(tok,pch[0]);
    }
    if(pch[0]=='\''){
        SAFE_PUSH_NEXT(tok,pch[0]);
    }
    tok->T=TOKEN_CHAR;
    return 0;
}
int readLineComment(FILE*rfp,int*pch,token*tok){
    SAFE_PUSH(tok,'/');
    SAFE_PUSH(tok,'/');
    nextChar(rfp,pch);
    while(pch[0]!='\n'&&pch[0]!=EOF){
        SAFE_PUSH_NEXT(tok,pch[0]);
    }
    tok->T=TOKEN_COMMENT;
    return 0;
}
int readBlockComment(FILE*rfp,int*pch,token*tok){
    SAFE_PUSH(tok,'/');
    SAFE_PUSH(tok,'*');
    nextChar(rfp,pch);
    int close=0;
    while(!close&&*pch!=EOF){
        if(*pch=='*'){
            SAFE_PUSH_NEXT(tok,*pch);
            if(*pch=='/'){
                SAFE_PUSH_NEXT(tok,*pch);
                close=1;
            }
        }
        else{
            SAFE_PUSH_NEXT(tok,*pch);
        }
    }
    tok->T=TOKEN_COMMENT;
    return 0;
}
int readPreprocessor(FILE*rfp,int*pch,token*tok){
    while(*pch!=EOF&&*pch!='\n'){
        if(*pch=='\r'){
            nextChar(rfp,pch);
            continue;
        }
        if(*pch=='\\'){
            if(pushToken(tok,'\\')!=0){
                tok->T=TOKEN_ERR;
                return 1;
            }
            nextChar(rfp,pch);
            if(*pch=='\r'){
                nextChar(rfp,pch);
            }
            if(*pch=='\n'){
                if(pushToken(tok,'\n')!=0){
                    tok->T=TOKEN_ERR;
                    return 2;
                }
                nextChar(rfp,pch);
            }
        }
        else{
            SAFE_PUSH_NEXT(tok,*pch);
        }
    }
    tok->T=TOKEN_PREPROC;
    return 0;
}
int readOperator(FILE*rfp,int*pch,token*tok){
    char first=pch[0];
    SAFE_PUSH_NEXT(tok,pch[0]);
    if((first=='='&&pch[0]=='=')||(first=='!'&&pch[0]=='=')||(first=='<'&&pch[0]=='=')||(first=='>'&&pch[0]=='=')||(first=='&'&&pch[0]=='&')||(first=='|'&&pch[0]=='|')||(first=='+'&&pch[0]=='+')||(first=='-'&&pch[0]=='-')||(first=='-'&&pch[0]=='>')||(first=='+'&&pch[0]=='=')||(first=='-'&&pch[0]=='=')||(first=='*'&&pch[0]=='=')||(first=='/'&&pch[0]=='=')||(first=='%'&&pch[0]=='=')||(first=='&'&&pch[0]=='=')||(first=='|'&&pch[0]=='=')||(first=='^'&&pch[0]=='=')||(first=='<'&&pch[0]=='<')||(first=='>'&&pch[0]=='>')){
        SAFE_PUSH_NEXT(tok,pch[0]);
    }
    if(strcmp(tok->text,".")==0||strcmp(tok->text,"->")==0){
        tok->T=TOKEN_DOT;
    }
    else{
        tok->T=TOKEN_OPERATOR;
    }
    return 0;
}
int getToken(FILE*rfp,int*pch,token*tok){
    skipWhiteSpace(rfp,pch);
    if(pch[0]==EOF){
        tok->T=TOKEN_EOF;
        return 0;
    }
    if(pch[0]=='#'){
        int ret=readPreprocessor(rfp,pch,tok);
        return ret;
    }
    if(isalpha(pch[0])||pch[0]=='_'){
        int ret=readIdentifier(rfp,pch,tok);
        return ret;
    }
    if(isdigit(pch[0])){
        int ret=readNumber(rfp,pch,tok);
        return ret;
    }
    if(pch[0]=='"'){
        int ret=readString(rfp,pch,tok);
        return ret;
    }
    if(pch[0]=='\''){
        int ret=readChar(rfp,pch,tok);
        return ret;
    }
    if(pch[0]=='/'){
        nextChar(rfp,pch);
        if(pch[0]=='/'){
            int ret=readLineComment(rfp,pch,tok);
            return ret;
        }
        if(pch[0]=='*'){
            int ret=readBlockComment(rfp,pch,tok);
            return ret;
        }
        ungetc(pch[0],rfp);
        pch[0]='/';
        int ret=readOperator(rfp,pch,tok);
        return ret;
    }
    int ret;
    switch(pch[0]){
        case'{':tok->T=TOKEN_LBRACE;
        ret=pushToken(tok,'{');
        nextChar(rfp,pch);
        return ret;
        case'}':tok->T=TOKEN_RBRACE;
        ret=pushToken(tok,'}');
        nextChar(rfp,pch);
        return ret;
        case'[':tok->T=TOKEN_LBRACKET;
        ret=pushToken(tok,'[');
        nextChar(rfp,pch);
        return ret;
        case']':tok->T=TOKEN_RBRACKET;
        ret=pushToken(tok,']');
        nextChar(rfp,pch);
        return ret;
        case';':tok->T=TOKEN_SEMICOLON;
        ret=pushToken(tok,';');
        nextChar(rfp,pch);
        return ret;
        case',':tok->T=TOKEN_COMMA;
        ret=pushToken(tok,',');
        nextChar(rfp,pch);
        return ret;
        case':':tok->T=TOKEN_COLON;
        ret=pushToken(tok,':');
        nextChar(rfp,pch);
        return ret;
        case'(':tok->T=TOKEN_LPAREN;
        ret=pushToken(tok,'(');
        nextChar(rfp,pch);
        return ret;
        case')':tok->T=TOKEN_RPAREN;
        ret=pushToken(tok,')');
        nextChar(rfp,pch);
        return ret;
        case'.':nextChar(rfp,pch);
        if(isdigit(pch[0])){
            ungetc(pch[0],rfp);
            pch[0]='.';
            int ret=readNumber(rfp,pch,tok);
            return ret;
        }
        ungetc(pch[0],rfp);
        pch[0]='.';
        tok->T=TOKEN_DOT;
        ret=pushToken(tok,'.');
        nextChar(rfp,pch);
        return ret;
        default:if(ispunct(pch[0])){
            ret=readOperator(rfp,pch,tok);
            return ret;
        }
        else{
            tok->T=TOKEN_ERR;
            nextChar(rfp,pch);
            return 5;
        }
    }
    return 0;
}
 /*int needSpaceBefore(token* prev,token* cur){
	if(prev==NULL||cur==NULL)return 0;
	if(prev->T==TOKEN_LPAREN||prev->T==TOKEN_LBRACKET)
		return 0;
	if(cur->T==TOKEN_RPAREN||cur->T==TOKEN_RBRACKET)
		return 0;
	if(prev->T==TOKEN_OPERATOR&&
	    (strcmp(cur->text,"!")==0||
	    strcmp(cur->text,"~")==0||
	    strcmp(cur->text,"++")==0||
	    strcmp(cur->text,"--")==0))
	    return 0;
	if(cur->T==TOKEN_OPERATOR&&
	    (strcmp(cur->text,"!")==0||
	    strcmp(cur->text,"~")==0||
	    strcmp(cur->text,"++")==0||
	    strcmp(cur->text,"--")==0))
	    return 0;
	if(prev->T==TOKEN_LPAREN||prev->T==TOKEN_LBRACKET)
	    return 0;
	if(cur->T==TOKEN_LPAREN){
		if(prev->T==TOKEN_WORD&&
			(strcmp(prev->text,"if")== 0||
		        strcmp(prev->text,"while")==0||
		        strcmp(prev->text,"for") == 0||
		        strcmp(prev->text,"switch")== 0||
		        strcmp(prev->text,"return")==0))
		    return 1;
		return 0;
	}
    if(cur->T==TOKEN_COMMENT)return 0;
    if(cur->T==TOKEN_PREPROC)return 0;
    if(prev->T==TOKEN_COMMENT)return 0;
    if(prev->T==TOKEN_PREPROC)return 0;
    if(prev->T==TOKEN_COMMA)return 1;
    if(prev->T==TOKEN_RPAREN&&
        (cur->T==TOKEN_LBRACE||cur->T==TOKEN_WORD))
        return 1;
    if(prev->T==TOKEN_LPAREN||prev->T==TOKEN_LBRACKET)
	    return 0;
	if(cur->T==TOKEN_RPAREN||cur->T==TOKEN_RBRACKET)
	    return 0;
    if(cur->T==TOKEN_OPERATOR||prev->T==TOKEN_OPERATOR)
        return 1;
    if(prev->T==TOKEN_WORD&&cur->T==TOKEN_WORD)
	    return 1;
	if(prev->T==TOKEN_WORD&&cur->T==TOKEN_NUMBER)
	return 1;
	if(prev->T==TOKEN_NUMBER&&cur->T==TOKEN_WORD)
	return 1;
    return 0;
}*/
int needSpaceBefore(token*prev,token*cur,int compact){
    if(!prev||!cur)return 0;
    if(compact){
         // 【紧凑模式】仅在相邻的标识符/数字之间加空格
        if(prev->T==TOKEN_WORD&&cur->T==TOKEN_WORD)return 1;
        if(prev->T==TOKEN_WORD&&cur->T==TOKEN_NUMBER)return 1;
        if(prev->T==TOKEN_NUMBER&&cur->T==TOKEN_WORD)return 1;
         // 行内注释前面加一个空格，保持可读性（如 TOKEN_LBRACE, //{）
        if(cur->T==TOKEN_COMMENT)return 1;
         // 其他任何地方都不加空格
        return 0;
    }
     // 1. 左括号之后永远不要空格
    if(prev->T==TOKEN_LPAREN||prev->T==TOKEN_LBRACKET)return 0;
     // 2. 右括号之前永远不要空格
    if(cur->T==TOKEN_RPAREN||cur->T==TOKEN_RBRACKET)return 0;
     // 3. 分号、逗号、冒号之前不要空格（紧跟前面内容）
    if(cur->T==TOKEN_SEMICOLON||cur->T==TOKEN_COMMA||cur->T==TOKEN_COLON)return 0;
     // 4. 一元运算符与其操作数之间不加空格
    if(prev->T==TOKEN_OPERATOR&&(strcmp(prev->text,"!")==0||strcmp(prev->text,"~")==0||strcmp(prev->text,"++")==0||strcmp(prev->text,"--")==0))return 0;
    if(cur->T==TOKEN_OPERATOR&&(strcmp(cur->text,"!")==0||strcmp(cur->text,"~")==0||strcmp(cur->text,"++")==0||strcmp(cur->text,"--")==0))return 0;
     // 5. 点运算符 . 和 -> 前后都不加空格
    if(cur->T==TOKEN_DOT||prev->T==TOKEN_DOT)return 0;
     // 6. 关键字后的 '(' 前加空格
    if(cur->T==TOKEN_LPAREN&&prev->T==TOKEN_WORD){
        if(strcmp(prev->text,"if")==0||strcmp(prev->text,"while")==0||strcmp(prev->text,"for")==0||strcmp(prev->text,"switch")==0||strcmp(prev->text,"return")==0)return 1;
        return 0;
         // 普通函数/宏调用 '(' 前不加空格
    }
     // 7. 逗号、右括号之后一般要加空格（例外：后跟 ';' 已在上面拦截）
    if(prev->T==TOKEN_COMMA)return 1;
    if(prev->T==TOKEN_RPAREN){
        if(cur->T==TOKEN_LBRACE||cur->T==TOKEN_WORD||cur->T==TOKEN_OPERATOR)return 1;
        return 0;
    }
     // 8. 预处理指令、注释附近不加多余空格（按原先逻辑保留）
    if(cur->T==TOKEN_COMMENT||cur->T==TOKEN_PREPROC)return 0;
    if(prev->T==TOKEN_COMMENT||prev->T==TOKEN_PREPROC)return 0;
     // 9. 二元/普通运算符两侧加空格
    if(cur->T==TOKEN_OPERATOR||prev->T==TOKEN_OPERATOR)return 1;
     // 10. 相邻标识符或标识符与数字之间加空格
    if(prev->T==TOKEN_WORD&&cur->T==TOKEN_WORD)return 1;
    if(prev->T==TOKEN_WORD&&cur->T==TOKEN_NUMBER)return 1;
    if(prev->T==TOKEN_NUMBER&&cur->T==TOKEN_WORD)return 1;
    return 0;
}
int formatting(char*rFileName,int braceOneNewLine,int compact){
    FILE*rfp=fopen(rFileName,"rb");
    if(rfp==NULL){
        fprintf(stderr,"打开文件失败\n");
        return 3;
    }
    char wFileName[512];
    strcpy(wFileName,rFileName);
    strcat(wFileName,"_f");
    FILE*wfp=fopen(wFileName,"wb");
    if(wfp==NULL){
        fprintf(stderr,"打开输出文件失败\n");
        fclose(rfp);
        return 4;
    }
    int currentChar='\0';
    nextChar(rfp,&currentChar);
    int nonBlockBraceCount=0;
    int indentLv=0;
    int parenDepth=0;
    int suppressNextSpace=0;
    int beginOfLine=1;
    token*prev=NULL;
    token*cur=newToken();
    if(cur==NULL)return 1;
    int*pch=&currentChar;
    getToken(rfp,pch,cur);
    while(cur->T!=TOKEN_EOF){
        if(cur->T==TOKEN_PREPROC){
            if(!beginOfLine)fputc('\n',wfp);
            fputs(cur->text,wfp);
            if(cur->text[cur->len-1]!='\n')fputc('\n',wfp);
            beginOfLine=1;
            freeToken(prev);
            prev=cur;
            cur=newToken();
            getToken(rfp,pch,cur);
            continue;
        }
        if(cur->T==TOKEN_COMMENT){
            if(beginOfLine){
                for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                beginOfLine=0;
            }
            if(!beginOfLine&&needSpaceBefore(prev,cur,compact))fputc(' ',wfp);
            fputs(cur->text,wfp);
            if(cur->len>0&&cur->text[cur->len-1]=='\n')beginOfLine=1;
            else{
                fputc('\n',wfp);
                beginOfLine=1;
            }
            freeToken(prev);
            prev=cur;
            cur=newToken();
            getToken(rfp,pch,cur);
            continue;
        }
        if(!beginOfLine&&needSpaceBefore(prev,cur,compact)&&!suppressNextSpace)fputc(' ',wfp);
        suppressNextSpace=0;
        if(cur->T==TOKEN_LBRACE){
            int isBlock=1;
            if(prev){
                if(prev->T==TOKEN_OPERATOR&&strcmp(prev->text,"=")==0)isBlock=0;
                else if(prev->T==TOKEN_COMMA)isBlock=0;
                else if(prev->T==TOKEN_LPAREN)isBlock=0;
            }
            if(isBlock){
                if(braceOneNewLine){
                    if(!beginOfLine)fputc('\n',wfp);
                    for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                    fputs("{",wfp);
                    indentLv++;
                    fputc('\n',wfp);
                    beginOfLine=1;
                }
                else{
                    if(beginOfLine){
                        for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                        beginOfLine=0;
                    }
                    fputs("{",wfp);
                    indentLv++;
                    fputc('\n',wfp);
                    beginOfLine=1;
                }
            }
            else{
                nonBlockBraceCount++;
                if(beginOfLine){
                    for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                    beginOfLine=0;
                }
                fputs("{",wfp);
            }
        }
        else if(cur->T==TOKEN_RBRACE){
            if(nonBlockBraceCount>0){
                nonBlockBraceCount--;
                if(beginOfLine){
                    for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                    beginOfLine=0;
                }
                fputs("}",wfp);
            }
            else{
                indentLv--;
                if(!beginOfLine)fputc('\n',wfp);
                for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                fputs("}",wfp);
                fputc('\n',wfp);
                beginOfLine=1;
            }
        }
        else if(cur->T==TOKEN_LPAREN){
            if(beginOfLine){
                for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                beginOfLine=0;
            }
            fputs("(",wfp);
            parenDepth++;
            suppressNextSpace=1;
        }
        else if(cur->T==TOKEN_RPAREN){
            if(beginOfLine){
                for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                beginOfLine=0;
            }
            fputs(")",wfp);
            parenDepth--;
        }
        else if(cur->T==TOKEN_SEMICOLON){
            fputs(";",wfp);
            if(parenDepth==0){
                fputc('\n',wfp);
                beginOfLine=1;
            }
        }
        else{
            if(beginOfLine){
                for(int i=0;i<indentLv*4;i++)fputc(' ',wfp);
                beginOfLine=0;
            }
            fputs(cur->text,wfp);
        }
        freeToken(prev);
        prev=cur;
        cur=newToken();
        getToken(rfp,pch,cur);
    }
    if(!beginOfLine){
        fputc('\n',wfp);
    }
    freeToken(prev);
    freeToken(cur);
    fclose(wfp);
    fclose(rfp);
#ifdef _WIN32
    char bakName[512];
    snprintf(bakName,sizeof(bakName),"%s.bak",rFileName);
    DeleteFileA(bakName);
    if(!MoveFileA(rFileName,bakName)){
        fprintf(stderr,"无法备份原文件\n");
        return 5;
    }
    if(!MoveFileExA(wFileName,rFileName,MOVEFILE_REPLACE_EXISTING)){
        fprintf(stderr,"替换失败，备份为 %s，新文件为 %s\n",bakName,wFileName);
        return 6;
    }
     // 成功，可以选择删除备份
     // DeleteFileA(bakName);
#else
    if(rename(wFileName,rFileName)!=0){
        perror("rename");
        return 5;
    }
#endif
    return 0;
}
#endif
