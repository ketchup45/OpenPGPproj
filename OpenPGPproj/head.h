#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <ctype.h>
#include <windows.h>
#include <ipworksopenpgp.h>
using namespace std;

#define LINE_LEN 100
#define passphrase "myOpenPGPproj"

// 自定义的KeyMgr类，继承自KeyMgr类
class MyKeyMgr : public KeyMgr
{
private:
    OpenPGP* mypgp;
public:
    vector<char*> allusers;//记录所有拥有密钥用户名
    char keyringDir[LINE_LEN]; // 密钥环的目录
    char myusername[LINE_LEN]; //当前用户名

    virtual int FireKeyList(KeyMgrKeyListEventParams* e)
    {
        //cout << e->UserId << "  ";
        char* temp = new char(LINE_LEN);
        strcpy(temp, e->UserId);
        allusers.push_back(temp);//添加到用户记录中
        return 0;
    }

    void SetKeyringDir();//设定密钥环目录路径
    int CheckUserExistence(char keyusername[]);//检查用户是否存在密钥
    void CreateUserKey(char keyusername[]);//为用户创建密钥

    MyKeyMgr(char curusername[], OpenPGP* pgp) {
        allusers.clear();

        //设定参数
        strcpy(myusername, curusername);
        mypgp = pgp;

        SetKeyringDir();//设定密钥环目录
        LoadKeyring(keyringDir); //加载密钥环

        ListKeys();//遍历读取密钥

        int IfExist= CheckUserExistence(myusername);
        if (IfExist == 0)//若不存在密钥，则创建一个
            CreateUserKey(myusername);
    }
    ~MyKeyMgr() {
        //释放用户表中动态内存申请
        for (int i = allusers.size() - 1; i >= 0; i--)
        {
            char* temp = allusers[i];    
            allusers.pop_back();
        }
    }
};

class MyFileMgr //管理文件操作
{
    OpenPGP* mypgp;
    MyKeyMgr* mykeymgr;

public:
    void FileMenu();//显示文件菜单
    void FileOperation(int opt);//进行文件操作
    void SetFileName(char inputfile[], char outputfile[]);

    MyFileMgr(OpenPGP* inmypgp, MyKeyMgr* inmykeymgr) {
        mypgp = inmypgp;
        mykeymgr = inmykeymgr;
    }
};

void ShowError(int ret_code, OpenPGP* mypgp);//显示错误，退出程序
void GetWinUserName(char username[]);//获取当前windwows账户用户名
//