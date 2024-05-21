#include "head.h"

int main()
{
	char username[LINE_LEN]; //用户名
	GetWinUserName(username); //获取当前windwows账户用户名
	cout << "欢迎用户 "<< username <<" 使用本文件加密程序" << endl;

	OpenPGP pgp; //IPWorks OpenPGP组件
	MyKeyMgr keymgr(username, &pgp); //密钥管理
	
	MyFileMgr filemgr(&pgp, &keymgr);//文件操作管理
	filemgr.FileMenu();//显示文件菜单

	return 0;
}