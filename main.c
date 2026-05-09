#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<string.h>
#include<direct.h>
#include"myToken.h"
int main(){
	/*int weight[256]={0};
	char s[120];
	scanf("%s",s);
	countChar(s,weight);
	char* code[256]={0};
	RETcode(weight,code);
	for(int i=0;i<256;i++){
		if(code[i])printf("%d ",i);
		puts(code[i]);
	}
	writeFile("out.huf",code,s,weight);*/
	char path[MAX_PATH];
	char cmd[100];
	#define CMDLINE_MAX 4096
	char in[CMDLINE_MAX+100];
	char arg[CMDLINE_MAX];
	if (_getcwd(path, MAX_PATH)==NULL){
		puts("获取当前目录失败");
		return -1;
	}
	printf("输入?获取指令大全。\n");
	printf("注意是半角的?。\n");
	printf("注意输入文件名要完整，带后缀。\n");
	while(1){
		printf("%s> ",path);
		if(fgets(in,sizeof(in),stdin)==NULL){
			break;
		}
		in[strcspn(in,"\n")]='\0';
		char* p=in;
		while(*p==' '||*p=='\t'){//\t->tab
			p++;
		}
		if(*p=='\0')continue;//写的时候漏了个=，导致总是输出原目录
		int n=sscanf(p,"%99s %4095[^\n]",cmd,arg);
		if(n<1){//[^\n]读取直到遇到\n
			continue;
		}
		if(_stricmp(cmd,"exit")==0||_stricmp(cmd,"quit")==0){
			break;
		}
		if(_stricmp(cmd,"cd")==0){
			if(n<2)continue;
			if(SetCurrentDirectoryA(arg)){//设置当前工作目录
				if(_getcwd(path,MAX_PATH)==NULL){
					puts("无法获取当前目录");
				}
			}else{
				DWORD error=GetLastError();//错误处理，错误处理这部分是AI写的
				printf("无法切换到目录 \"%s\"。错误码: %lu\n", arg, error);
				if(error==ERROR_FILE_NOT_FOUND){
					printf("原因：目录不存在。\n");
				}
				else if(error==ERROR_ACCESS_DENIED){
					printf("原因：访问被拒绝。\n");
				}
			}
		}else if(_stricmp(cmd,"cls")==0){
			HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
			if(hConsole==INVALID_HANDLE_VALUE){
				continue;
			}
			CONSOLE_SCREEN_BUFFER_INFO CSBI;
			if(!GetConsoleScreenBufferInfo(hConsole,&CSBI)){
				continue;
			}
			DWORD cells=CSBI.dwSize.X*CSBI.dwSize.Y;//unsigned int
			COORD startCoord={0,0};
			DWORD written;
			FillConsoleOutputCharacter(hConsole,' ',cells,startCoord,&written);
			SetConsoleCursorPosition(hConsole,startCoord);
		}else if(_stricmp(cmd,"dir")==0){
			char pathp[MAX_PATH];
			snprintf(pathp,MAX_PATH,"%s\\*",path);
			WIN32_FIND_DATA findData;
			HANDLE hFind;
			hFind=FindFirstFile(pathp,&findData);
			if(hFind==INVALID_HANDLE_VALUE){
				printf("无法访问当前目录\n");
				continue;
			}
			do{
				printf("%s",findData.cFileName);
				if(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){
					printf("(目录)");
				}
				puts("");
			}while(FindNextFile(hFind,&findData));
			FindClose(hFind);
		}else if(_stricmp(cmd,"lzpack")==0){
			char outfile[MAX_PATH];
			char remaining[CMDLINE_MAX];
			if(sscanf(arg,"%s %[^\n]",outfile,remaining)<1){
				printf("错误，无法解析输出文件名\n");
				continue;
			}
			FILE* f=fopen(outfile,"wb");
			if(f){
				fclose(f);
			}else{
				printf("错误，无法创建输出文件\n");
				continue;
			}
			char* token=strtok(remaining,";");
			int success=0,fail=0;
			while(token!=NULL){
				while(*token==' '||*token=='\t')token++;
				char* end=token+strlen(token)-1;
				while(end>token&&(*end==' '||*end=='\t'))end--;
				1[end]='\0';//整活
				if(strlen(token)==0){
					token=strtok(NULL,";");
					continue;
				}
				token=strtok(NULL,";");
			}
			printf("打包完成，成功%d个，失败%d个\n",success,fail);
		}
	}
}
