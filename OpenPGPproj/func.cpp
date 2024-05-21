#include "head.h"
#include <conio.h>

void MyKeyMgr::SetKeyringDir()
{
	cout << "��������Կ����ŵ�Ŀ¼·��" << endl << "����ֱ�ӻس���Ĭ��Ϊ��ǰĿ¼�µ�keyring�ļ��У���";
	fgets(keyringDir, LINE_LEN, stdin); // �ӱ�׼�����ȡ�û��������Կ��Ŀ¼
	keyringDir[strlen(keyringDir) - 1] = '\0'; // �Ƴ����Ļ��з�

	if (strlen(keyringDir) == 0)
		strcpy(keyringDir, "keyring");
}

int MyKeyMgr::CheckUserExistence(char keyusername[])//����û��Ƿ������Կ
{
	for (int i = 0; i < allusers.size(); ++i)
	{
		if (strcmp(allusers[i], keyusername) == 0) {
			return 1;
		}
	}
	return 0;
}

void MyKeyMgr::CreateUserKey(char keyusername[])//Ϊ�û�������Կ
{
	// ����PGP��Կ��
	int ret_code = 0;
	ret_code = CreateKey(keyusername, passphrase); // ����CreateKey����������Կ��
	if (ret_code) ShowError(ret_code, mypgp); // ����������ת
	ret_code = SaveKeyring(keyringDir); // ����SaveKeyring����������Կ��
	if (ret_code) ShowError(ret_code, mypgp); // ����������ת

	char* temp = new char(LINE_LEN);
	strcpy(temp, keyusername);
	allusers.push_back(temp);//��ӵ�������
}

void ShowError(int ret_code, OpenPGP* mypgp)
{
	if (ret_code)     // ����������󣬴�ӡ������Ϣ
	{
		printf("\nError: %d", ret_code);
		if (mypgp->GetLastError())
			printf(" \"%s\"\n", mypgp->GetLastError());
	}
	system("pause");
	exit(-1);//��ֹ
}

void GetWinUserName(char username[])
{
	unsigned long readsize = 255;//��ȡ����
	WCHAR winusername[LINE_LEN]; //�û���
	GetUserName(winusername, &readsize);
	sprintf(username, "%ws", winusername); //��WCHAR[]תΪchar[]
}

void MyFileMgr::FileMenu()//��ʾ�ļ��˵�
{
	while (1) {
		system("cls");
		cout << "�����ļ�---------------�� 1" << endl;
		cout << "�����ļ�---------------�� 2" << endl;
		cout << "ǩ���ļ�---------------�� 3" << endl;
		cout << "��֤�ļ�---------------�� 4" << endl;
		cout << "ǩ���������ļ�---------�� 5" << endl;
		cout << "���ܲ���֤�ļ�---------�� 6" << endl;
		cout << "�˳�����---------------�� 0" << endl;
		unsigned char opt;//ѡ��
		opt = _getch();
		if (opt == '0') {
			break;
		}
		else if (opt >= '1'&& opt <= '6'){
			system("cls");
			cout<<"��ǰ�û���" << mykeymgr->myusername << endl;
			FileOperation(opt - '0');
			system("pause");
		}
		else {
			cout << "�����������ڣ�����������" << endl;
			system("pause");
		}
	}
}

void MyFileMgr::SetFileName(char inputfile[], char outputfile[])
{
	printf("�����ò����������ļ�·��: ");
	cin >> inputfile;
	mypgp->SetInputFile(inputfile); // ���������ļ�

	printf("�����ò���������ļ�·��: ");
	cin >> outputfile;
	mypgp->SetOutputFile(outputfile); // ��������ļ�

	cout << "�Ƿ񸲸�ԭ�ļ�����y/n��";
	char temp;
	cin >> temp;
	mypgp->SetOverwrite(temp == 'y'); // �����Ƿ񸲸������ļ�
}

void MyFileMgr::FileOperation(int opt)//�����ļ�����
{
	// ����PGP���״̬
	mypgp->Reset();

	//�趨�ļ�·��
	char inputfile[LINE_LEN]; //�����ļ�·��
	char outputfile[LINE_LEN]; //����ļ�·��
	SetFileName(inputfile, outputfile);

	//�趨������
	char receivername[LINE_LEN];//�������û���
	if (opt == 1 || opt == 5) {
		cout << "������������ĸ��ļ����û���: ";
		cin >> receivername;

		int IfExist = mykeymgr->CheckUserExistence(receivername);
		if (IfExist == 0)//����������Կ���򴴽�һ��
			mykeymgr->CreateUserKey(receivername);
	}

	//�趨�ļ�������
	char signername[LINE_LEN];//ǩ�����û���
	if (opt == 4 || opt == 6) {
		cout << "������׼����֤���ļ������ߵ��û���: ";
		cin >> signername;
	}

	//��Բ�ͬѡ����в���
	int ret_code = 0;
	if (opt == 1) {
		mypgp->SetASCIIArmor(true); // �������ΪASCII��ʽ
		mypgp->SetRecipientKeyCount(1); // ���ý����߹�Կ����Ϊ1
		mypgp->SetRecipientKeyKeyring(0, mykeymgr->keyringDir); // ���ý����߹�Կ���ڵ���Կ��Ŀ¼
		mypgp->SetRecipientKeyUserId(0, receivername); // ���ý����߹�Կ���û�ID
		ret_code = mypgp->Encrypt(); // ����Encrypt�������м���
		if (ret_code) ShowError(ret_code, mypgp); // ����������ת
		cout << "�ļ��������" << endl;
	}
	else if (opt == 2) {
		mypgp->SetKeyCount(1); // ����˽Կ����Ϊ1
		mypgp->SetKeyKeyring(0, mykeymgr->keyringDir); // ����˽Կ���ڵ���Կ��Ŀ¼
		mypgp->SetKeyUserId(0, mykeymgr->myusername); // ����˽Կ���û�ID
		mypgp->SetKeyPassphrase(0, passphrase); // ����˽Կ������
		ret_code = mypgp->Decrypt(); // ����Decrypt�������н���
		if (ret_code) ShowError(ret_code, mypgp); // ����������ת
		cout << "�ļ��������" << endl;
	}
	else if (opt == 3) {
		//mypgp->SetSigningAlgorithm("MD5");
		mypgp->SetASCIIArmor(true); // ����ASCIIװ�ף���ǩ�����ת��Ϊ�ɶ���ASCII��ʽ
		mypgp->SetKeyCount(1); // ����ʹ�õ���Կ����Ϊ1
		mypgp->SetKeyKeyring(0, mykeymgr->keyringDir); // ������Կ���ڵ���Կ��Ŀ¼
		mypgp->SetKeyUserId(0, mykeymgr->myusername); // ��������ǩ����˽Կ�û�ID
		mypgp->SetKeyPassphrase(0, passphrase); // ����˽Կ������
		ret_code = mypgp->Sign(); // ����Sign��������Ϣ����ǩ��
		if (ret_code) ShowError(ret_code, mypgp); // ����������ת
		cout << "�ļ�ǩ�����" << endl;
	}
	else if (opt == 4) {
		mypgp->SetSignerKeyCount(1); // ����ʹ�õ�ǩ����Կ����Ϊ1
		mypgp->SetSignerKeyKeyring(0, mykeymgr->keyringDir); // ����ǩ����Կ���ڵ���Կ��Ŀ¼
		mypgp->SetSignerKeyUserId(0, signername); // ����������֤��ǩ����Կ�û�ID
		ret_code = mypgp->VerifySignature(); // ����VerifySignature��������Ϣ������֤
		if (ret_code) ShowError(ret_code, mypgp); // ����������ת
		cout << "�ļ���֤���" << endl;
	}
	else if (opt == 5) {
		mypgp->SetASCIIArmor(true); // ����ASCIIװ�ף���ǩ���ͼ������ת��Ϊ�ɶ���ASCII��ʽ
		mypgp->SetKeyCount(1); // ����ʹ�õ���Կ����Ϊ1
		mypgp->SetKeyKeyring(0, mykeymgr->keyringDir); // ������Կ���ڵ���Կ��Ŀ¼
		mypgp->SetKeyUserId(0, mykeymgr->myusername); // ��������ǩ����˽Կ�û�ID
		mypgp->SetKeyPassphrase(0, passphrase); // ����˽Կ������
		mypgp->SetRecipientKeyCount(1); // ���ý����߹�Կ����Ϊ1
		mypgp->SetRecipientKeyKeyring(0, mykeymgr->keyringDir); // ���ý����߹�Կ���ڵ���Կ��Ŀ¼
		mypgp->SetRecipientKeyUserId(0, receivername); // ���ý����߹�Կ���û�ID
		ret_code = mypgp->SignAndEncrypt(); // ����SignAndEncrypt��������Ϣ����ǩ���ͼ���
		if (ret_code) ShowError(ret_code, mypgp); // ����������ת
		cout << "�ļ�ǩ�����������" << endl;
	}
	else if (opt == 6) {
		mypgp->SetKeyCount(1); // ����ʹ�õ���Կ����Ϊ1
		mypgp->SetKeyKeyring(0, mykeymgr->keyringDir); // ������Կ���ڵ���Կ��Ŀ¼
		mypgp->SetKeyUserId(0, mykeymgr->myusername); // �������ڽ��ܵ�˽Կ�û�ID
		mypgp->SetKeyPassphrase(0, passphrase); // ����˽Կ������
		mypgp->SetSignerKeyCount(1); // ����ʹ�õ�ǩ����Կ����Ϊ1
		mypgp->SetSignerKeyKeyring(0, mykeymgr->keyringDir); // ����ǩ����Կ���ڵ���Կ��Ŀ¼
		mypgp->SetSignerKeyUserId(0, signername); // ����������֤��ǩ����Կ�û�ID
		ret_code = mypgp->DecryptAndVerifySignature(); // ����DecryptAndVerifySignature��������Ϣ���н��ܺ���֤
		if (ret_code) ShowError(ret_code, mypgp); // ����������ת
		cout << "�ļ����ܲ���֤���" << endl;
	}
}

