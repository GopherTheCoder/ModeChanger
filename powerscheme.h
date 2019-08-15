#ifndef __powerscheme_h__
#define __powerscheme_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <shellapi.h>

typedef struct powerscheme {
	char alias[10];
	char guid[37];
	int  activation;
}powerscheme;
typedef struct cfgchain {
	powerscheme scheme;
	struct cfgchain* next;
}cfgchain;

class PowerScheme {
private:
	cfgchain* cfg = getcfg();

	cfgchain* getcfg();
public:
	void changecfg(int choice);
	int getActiveCfg();
	//void getCfgNames();
};

cfgchain* PowerScheme::getcfg() {
	FILE* fp;
	char t;
	int i = 0, j = 0;
	cfgchain* cfg = (cfgchain*)malloc(sizeof(cfgchain));
	cfgchain* temp = cfg;

	fp = _popen("powercfg /l", "r");

	while (1) {
		/*---获取GUID---*/
		while (getc(fp) != ':');
		getc(fp);
		fgets(temp->scheme.guid, sizeof(temp->scheme.guid), fp);

		/*---获取名称---*/
		getc(fp); getc(fp); getc(fp);
		i = 0;
		while ((t = getc(fp)) != ')')
		{
			temp->scheme.alias[i] = t;
			i++;
		}
		temp->scheme.alias[i] = '\0';

		/*---获取激活状态---*/
		getc(fp);
		if ((t = getc(fp)) == '*') {
			temp->scheme.activation = 1;
			t = getc(fp);
			t = getc(fp);
		}
		else temp->scheme.activation = 0;

		/*---链表操作---*/
		if (feof(fp)) break;

		cfgchain* p = (cfgchain*)malloc(sizeof(cfgchain));
		p->next = NULL;
		temp->next = p;
		temp = p;
	}

	_pclose(fp);

	return cfg;
}

void PowerScheme::changecfg(int choice) {
	char command[50];

	while (choice - 1) {
		cfg = cfg->next;
		choice--;
	}

	strcpy_s(command, "powercfg /s ");
	strcat_s(command, cfg->scheme.guid);
	system(command);
	/*
	int cchWideChar = MultiByteToWideChar(CP_ACP, 0, command, -1, NULL, 0);
	wchar_t* lpParameters = new wchar_t[cchWideChar];
	MultiByteToWideChar(CP_ACP, 0, command, -1, lpParameters, cchWideChar);
	ShellExecute(NULL, NULL, L"powercfg", lpParameters, NULL, SW_HIDE);
	*/
	cfg = getcfg();
}

inline int PowerScheme::getActiveCfg() {
	cfgchain* head = cfg;
	int i = 1;

	while (head) {
		if (head->scheme.activation)
			return i;
		else {
			i++;
			head = head->next;
		}
	}
	exit(-1);
}

#endif // !__powerscheme_h__