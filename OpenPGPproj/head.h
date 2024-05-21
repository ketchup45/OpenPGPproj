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

// �Զ����KeyMgr�࣬�̳���KeyMgr��
class MyKeyMgr : public KeyMgr
{
private:
    OpenPGP* mypgp;
public:
    vector<char*> allusers;//��¼����ӵ����Կ�û���
    char keyringDir[LINE_LEN]; // ��Կ����Ŀ¼
    char myusername[LINE_LEN]; //��ǰ�û���

    virtual int FireKeyList(KeyMgrKeyListEventParams* e)
    {
        //cout << e->UserId << "  ";
        char* temp = new char(LINE_LEN);
        strcpy(temp, e->UserId);
        allusers.push_back(temp);//��ӵ��û���¼��
        return 0;
    }

    void SetKeyringDir();//�趨��Կ��Ŀ¼·��
    int CheckUserExistence(char keyusername[]);//����û��Ƿ������Կ
    void CreateUserKey(char keyusername[]);//Ϊ�û�������Կ

    MyKeyMgr(char curusername[], OpenPGP* pgp) {
        allusers.clear();

        //�趨����
        strcpy(myusername, curusername);
        mypgp = pgp;

        SetKeyringDir();//�趨��Կ��Ŀ¼
        LoadKeyring(keyringDir); //������Կ��

        ListKeys();//������ȡ��Կ

        int IfExist= CheckUserExistence(myusername);
        if (IfExist == 0)//����������Կ���򴴽�һ��
            CreateUserKey(myusername);
    }
    ~MyKeyMgr() {
        //�ͷ��û����ж�̬�ڴ�����
        for (int i = allusers.size() - 1; i >= 0; i--)
        {
            char* temp = allusers[i];    
            allusers.pop_back();
        }
    }
};

class MyFileMgr //�����ļ�����
{
    OpenPGP* mypgp;
    MyKeyMgr* mykeymgr;

public:
    void FileMenu();//��ʾ�ļ��˵�
    void FileOperation(int opt);//�����ļ�����
    void SetFileName(char inputfile[], char outputfile[]);

    MyFileMgr(OpenPGP* inmypgp, MyKeyMgr* inmykeymgr) {
        mypgp = inmypgp;
        mykeymgr = inmykeymgr;
    }
};

void ShowError(int ret_code, OpenPGP* mypgp);//��ʾ�����˳�����
void GetWinUserName(char username[]);//��ȡ��ǰwindwows�˻��û���
//