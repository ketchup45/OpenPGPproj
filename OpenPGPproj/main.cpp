#include "head.h"

int main()
{
	char username[LINE_LEN]; //�û���
	GetWinUserName(username); //��ȡ��ǰwindwows�˻��û���
	cout << "��ӭ�û� "<< username <<" ʹ�ñ��ļ����ܳ���" << endl;

	OpenPGP pgp; //IPWorks OpenPGP���
	MyKeyMgr keymgr(username, &pgp); //��Կ����
	
	MyFileMgr filemgr(&pgp, &keymgr);//�ļ���������
	filemgr.FileMenu();//��ʾ�ļ��˵�

	return 0;
}