#include "head.h"
#include <conio.h>

void MyKeyMgr::SetKeyringDir()
{
	cout << "请输入密钥环存放的目录路径" << endl << "（若直接回车，默认为当前目录下的keyring文件夹）：";
	fgets(keyringDir, LINE_LEN, stdin); // 从标准输入读取用户输入的密钥环目录
	keyringDir[strlen(keyringDir) - 1] = '\0'; // 移除最后的换行符

	if (strlen(keyringDir) == 0)
		strcpy(keyringDir, "keyring");
}

int MyKeyMgr::CheckUserExistence(char keyusername[])//检查用户是否存在密钥
{
	for (int i = 0; i < allusers.size(); ++i)
	{
		if (strcmp(allusers[i], keyusername) == 0) {
			return 1;
		}
	}
	return 0;
}

void MyKeyMgr::CreateUserKey(char keyusername[])//为用户创建密钥
{
	// 创建PGP密钥对
	int ret_code = 0;
	ret_code = CreateKey(keyusername, passphrase); // 调用CreateKey函数创建密钥对
	if (ret_code) ShowError(ret_code, mypgp); // 发生错误，跳转
	ret_code = SaveKeyring(keyringDir); // 调用SaveKeyring函数保存密钥对
	if (ret_code) ShowError(ret_code, mypgp); // 发生错误，跳转

	char* temp = new char(LINE_LEN);
	strcpy(temp, keyusername);
	allusers.push_back(temp);//添加到名单中
}

void ShowError(int ret_code, OpenPGP* mypgp)
{
	if (ret_code)     // 如果发生错误，打印错误信息
	{
		printf("\nError: %d", ret_code);
		if (mypgp->GetLastError())
			printf(" \"%s\"\n", mypgp->GetLastError());
	}
	system("pause");
	exit(-1);//终止
}

void GetWinUserName(char username[])
{
	unsigned long readsize = 255;//读取长度
	WCHAR winusername[LINE_LEN]; //用户名
	GetUserName(winusername, &readsize);
	sprintf(username, "%ws", winusername); //把WCHAR[]转为char[]
}

void MyFileMgr::FileMenu()//显示文件菜单
{
	while (1) {
		system("cls");
		cout << "加密文件---------------按 1" << endl;
		cout << "解密文件---------------按 2" << endl;
		cout << "签名文件---------------按 3" << endl;
		cout << "验证文件---------------按 4" << endl;
		cout << "签名并加密文件---------按 5" << endl;
		cout << "解密并验证文件---------按 6" << endl;
		cout << "退出程序---------------按 0" << endl;
		unsigned char opt;//选项
		opt = _getch();
		if (opt == '0') {
			break;
		}
		else if (opt >= '1'&& opt <= '6'){
			system("cls");
			cout<<"当前用户：" << mykeymgr->myusername << endl;
			FileOperation(opt - '0');
			system("pause");
		}
		else {
			cout << "输入的命令不存在，请重新输入" << endl;
			system("pause");
		}
	}
}

void MyFileMgr::SetFileName(char inputfile[], char outputfile[])
{
	printf("请设置操作的输入文件路径: ");
	cin >> inputfile;
	mypgp->SetInputFile(inputfile); // 设置输入文件

	printf("请设置操作的输出文件路径: ");
	cin >> outputfile;
	mypgp->SetOutputFile(outputfile); // 设置输出文件

	cout << "是否覆盖原文件？（y/n）";
	char temp;
	cin >> temp;
	mypgp->SetOverwrite(temp == 'y'); // 设置是否覆盖已有文件
}

void MyFileMgr::FileOperation(int opt)//进行文件操作
{
	// 重置PGP组件状态
	mypgp->Reset();

	//设定文件路径
	char inputfile[LINE_LEN]; //输入文件路径
	char outputfile[LINE_LEN]; //输出文件路径
	SetFileName(inputfile, outputfile);

	//设定接收者
	char receivername[LINE_LEN];//接收者用户名
	if (opt == 1 || opt == 5) {
		cout << "请输入允许调阅该文件的用户名: ";
		cin >> receivername;

		int IfExist = mykeymgr->CheckUserExistence(receivername);
		if (IfExist == 0)//若不存在密钥，则创建一个
			mykeymgr->CreateUserKey(receivername);
	}

	//设定文件发布者
	char signername[LINE_LEN];//签名者用户名
	if (opt == 4 || opt == 6) {
		cout << "请输入准备验证的文件发布者的用户名: ";
		cin >> signername;
	}

	//针对不同选项，进行操作
	int ret_code = 0;
	if (opt == 1) {
		mypgp->SetASCIIArmor(true); // 设置输出为ASCII格式
		mypgp->SetRecipientKeyCount(1); // 设置接收者公钥数量为1
		mypgp->SetRecipientKeyKeyring(0, mykeymgr->keyringDir); // 设置接收者公钥所在的密钥环目录
		mypgp->SetRecipientKeyUserId(0, receivername); // 设置接收者公钥的用户ID
		ret_code = mypgp->Encrypt(); // 调用Encrypt函数进行加密
		if (ret_code) ShowError(ret_code, mypgp); // 发生错误，跳转
		cout << "文件加密完成" << endl;
	}
	else if (opt == 2) {
		mypgp->SetKeyCount(1); // 设置私钥数量为1
		mypgp->SetKeyKeyring(0, mykeymgr->keyringDir); // 设置私钥所在的密钥环目录
		mypgp->SetKeyUserId(0, mykeymgr->myusername); // 设置私钥的用户ID
		mypgp->SetKeyPassphrase(0, passphrase); // 设置私钥的密码
		ret_code = mypgp->Decrypt(); // 调用Decrypt函数进行解密
		if (ret_code) ShowError(ret_code, mypgp); // 发生错误，跳转
		cout << "文件解密完成" << endl;
	}
	else if (opt == 3) {
		//mypgp->SetSigningAlgorithm("MD5");
		mypgp->SetASCIIArmor(true); // 启用ASCII装甲，将签名输出转换为可读的ASCII格式
		mypgp->SetKeyCount(1); // 设置使用的密钥数量为1
		mypgp->SetKeyKeyring(0, mykeymgr->keyringDir); // 设置密钥所在的密钥环目录
		mypgp->SetKeyUserId(0, mykeymgr->myusername); // 设置用于签名的私钥用户ID
		mypgp->SetKeyPassphrase(0, passphrase); // 设置私钥的密码
		ret_code = mypgp->Sign(); // 调用Sign方法对消息进行签名
		if (ret_code) ShowError(ret_code, mypgp); // 发生错误，跳转
		cout << "文件签名完成" << endl;
	}
	else if (opt == 4) {
		mypgp->SetSignerKeyCount(1); // 设置使用的签名密钥数量为1
		mypgp->SetSignerKeyKeyring(0, mykeymgr->keyringDir); // 设置签名密钥所在的密钥环目录
		mypgp->SetSignerKeyUserId(0, signername); // 设置用于验证的签名密钥用户ID
		ret_code = mypgp->VerifySignature(); // 调用VerifySignature方法对消息进行验证
		if (ret_code) ShowError(ret_code, mypgp); // 发生错误，跳转
		cout << "文件验证完成" << endl;
	}
	else if (opt == 5) {
		mypgp->SetASCIIArmor(true); // 启用ASCII装甲，将签名和加密输出转换为可读的ASCII格式
		mypgp->SetKeyCount(1); // 设置使用的密钥数量为1
		mypgp->SetKeyKeyring(0, mykeymgr->keyringDir); // 设置密钥所在的密钥环目录
		mypgp->SetKeyUserId(0, mykeymgr->myusername); // 设置用于签名的私钥用户ID
		mypgp->SetKeyPassphrase(0, passphrase); // 设置私钥的密码
		mypgp->SetRecipientKeyCount(1); // 设置接收者公钥数量为1
		mypgp->SetRecipientKeyKeyring(0, mykeymgr->keyringDir); // 设置接收者公钥所在的密钥环目录
		mypgp->SetRecipientKeyUserId(0, receivername); // 设置接收者公钥的用户ID
		ret_code = mypgp->SignAndEncrypt(); // 调用SignAndEncrypt方法对消息进行签名和加密
		if (ret_code) ShowError(ret_code, mypgp); // 发生错误，跳转
		cout << "文件签名并加密完成" << endl;
	}
	else if (opt == 6) {
		mypgp->SetKeyCount(1); // 设置使用的密钥数量为1
		mypgp->SetKeyKeyring(0, mykeymgr->keyringDir); // 设置密钥所在的密钥环目录
		mypgp->SetKeyUserId(0, mykeymgr->myusername); // 设置用于解密的私钥用户ID
		mypgp->SetKeyPassphrase(0, passphrase); // 设置私钥的密码
		mypgp->SetSignerKeyCount(1); // 设置使用的签名密钥数量为1
		mypgp->SetSignerKeyKeyring(0, mykeymgr->keyringDir); // 设置签名密钥所在的密钥环目录
		mypgp->SetSignerKeyUserId(0, signername); // 设置用于验证的签名密钥用户ID
		ret_code = mypgp->DecryptAndVerifySignature(); // 调用DecryptAndVerifySignature方法对消息进行解密和验证
		if (ret_code) ShowError(ret_code, mypgp); // 发生错误，跳转
		cout << "文件解密并验证完成" << endl;
	}
}

