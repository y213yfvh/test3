#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<string.h>
#include<direct.h>
#include"myToken.h"
int main(){
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
		if(*p=='\0')continue;
		int n=sscanf(p,"%99s %4095[^\n]",cmd,arg);
		if(n<1){//[^\n]读取直到遇到\n
			continue;
		}
		if(_stricmp(cmd,"exit")==0||_stricmp(cmd,"quit")==0){
			break;
		}
		if(_stricmp(cmd,"cd")==0){
			if(n<2)continue;
			if(SetCurrentDirectoryA(arg)){
				if(_getcwd(path,MAX_PATH)==NULL){
					puts("无法获取当前目录");
				}
			}else{
				DWORD error=GetLastError();
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
			DWORD cells=CSBI.dwSize.X*CSBI.dwSize.Y;
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
		}else if(_stricmp(cmd,"format0")==0){
			if(n<2)continue;
			char path1[MAX_PATH];
			sscanf(arg,"%s",path1);
			formatting(path1,0,0);
		}else if(_stricmp(cmd,"format1")==0){
			if(n<2)continue;
			char path1[MAX_PATH];
			sscanf(arg,"%s",path1);
			formatting(path1,1,0);
		}else if(_stricmp(cmd,"format2")==0){
			if(n<2)continue;
			char path1[MAX_PATH];
			sscanf(arg,"%s",path1);
			formatting(path1,0,1);
		}else if(strcmp(cmd,"?")==0){
			printf("cd <目录>                      切换工作目录\n");
			printf("exit / quit                    退出程序\n");
			printf("cls                            清空屏幕\n");
			printf("dir                            列出当前目录内容，.是自身，..是上一级目录\n");
			printf("format0                        格式化代码，{不独占一行\n");
			printf("format1                        格式化代码，{独占一行\n");
			printf("format2                        格式化代码，紧凑\n");
		}
	}
}
