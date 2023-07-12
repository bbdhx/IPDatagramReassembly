#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

//unsigned short int IP_data_len;                               //IP���ݱ����ݲ��ֳ���
unsigned short int *IP_data_len;
unsigned short int IP_head_len = 20;                          //IP���ݱ�ͷ������

//unsigned short int F_NUM;                                     //IP���ݱ���Ƭ����
unsigned short int F_data_len;                                //IP���ݱ���Ƭ���ݲ��ֳ���
unsigned short int F_head_len = 20;                           //IP���ݱ���Ƭͷ������

unsigned short int F_offset_multiple = 8;                     //IP���ݱ�Ƭƫ����Ϊ8�ı���

//bool date[1000] = { 0 };
bool **DATA;

struct hole                              //���ṹ��
{
	unsigned short int h_first;          //��ͷ�����ĵ�һ���ֽ�λ��
	unsigned short int h_last;           //��β���������һ���ֽ�λ��
	struct hole *h_next;                 //ָ����һ������ָ��
	struct hole *h_prev;                 //ָ��ǰһ������ָ��
};

struct fragment                          //��Ƭ�ṹ��
{
	unsigned short int f_first;          //��Ƭͷ
	unsigned short int f_last;           //��Ƭβ
	unsigned short int f_len;            //��Ƭ���ݲ��ֳ���
	unsigned short int f_offset;         //Ƭƫ����
	unsigned short int f_flag;           //��ʶ��ͬIP���ݱ�
	char f_more_fragment;                //��ʶ����Ƭ�Ƿ�Ϊ���һ����Ƭ
	char f_dont_fragment;                //��ʶIP���ݱ��Ƿ���Է�Ƭ
	string f_data;                       //��Ƭ�����ݲ���
};

class hole_fragment_reassembly                                                   //��Ƭ������
{
public:
	hole_fragment_reassembly();                                                  //�޲ι��캯����Ĭ��ԭʼ������β7000�ֽ�
	hole_fragment_reassembly(unsigned short int h_len);                          //�вι��캯��������Ϊָ��ԭʼ������h_len
	~hole_fragment_reassembly();                                              //��������
	void SET_fragment(fragment *frag, unsigned short int F_NUM, int flag, unsigned short int IP_data_len);                 //��IP���ݱ����з�Ƭ����������ֵ
	void GET_fragment(fragment *frag, unsigned short int F_NUM);                 //�����IP���ݱ�������ֵ
	void SET_hole(unsigned short int f_len, unsigned short int f_offset, unsigned short int f_flag, char f_more_fragment);
	unsigned short int GET_hole_len(int i) { return hole_len[i]; }


private:
	unsigned short int hole_len[10];                                                  //ԭʼ������
	hole *h_head[10];                                                                 //��ͷָ��
	int fragment_len;
};

hole_fragment_reassembly::hole_fragment_reassembly()
{
	for (int i = 0; i < 10; i++)
	{
		hole_len[i] = 1000;
		h_head[i] = new hole;
		h_head[i]->h_first = 0;
		h_head[i]->h_last = hole_len[i] - 1;
		h_head[i]->h_next = NULL;
		h_head[i]->h_prev = h_head[i];
	}
}

hole_fragment_reassembly::~hole_fragment_reassembly()
{
	for (int i = 0; i < 10; i++)
	{
		delete h_head[i];
	}
}

hole_fragment_reassembly::hole_fragment_reassembly(unsigned short int h_len)
{
	for (int i = 0; i < 10; i++)
	{
		hole_len[i] = h_len;
		h_head[i] = new hole;
		h_head[i]->h_first = 0;
		h_head[i]->h_last = hole_len[i] - 1;
		h_head[i]->h_next = NULL;
		h_head[i]->h_prev = h_head[i];
	}
}

void hole_fragment_reassembly::SET_fragment(fragment *frag, unsigned short int F_NUM, int flag, unsigned short int IP_data_len)
{
	int i;
	for (i = 0; i < F_NUM - 1; i++)
	{
		frag[i].f_first = i * F_data_len;
		frag[i].f_last = (i + 1) * F_data_len - 1;
		frag[i].f_len = frag[i].f_last - frag[i].f_first + 1;
		frag[i].f_offset = i * (F_data_len / 8);
		frag[i].f_flag = flag;
		frag[i].f_more_fragment = '1';
		frag[i].f_dont_fragment = '0';
		frag[i].f_data = "|��";
		/*stringstream s;
		s << i + 1;
		string ss;
		s >> ss;
		frag[i].f_data += ss;*/
		frag[i].f_data += to_string(flag);
		frag[i].f_data += "IP���ݱ��ĵ�";
		frag[i].f_data += to_string(i + 1);
		frag[i].f_data += "��Ƭ����|";
	}

	if (IP_data_len % F_data_len == 0)
	{
		frag[i].f_first = i * F_data_len;
		frag[i].f_last = (i + 1) * F_data_len - 1;
		frag[i].f_len = frag[i].f_last - frag[i].f_first + 1;
		frag[i].f_offset = i * (F_data_len / 8);
		frag[i].f_flag = flag;
		frag[i].f_more_fragment = '0';
		frag[i].f_dont_fragment = '0';
		frag[i].f_data = "|��";
		/*stringstream s;
		s << i + 1;
		string ss;
		s >> ss;
		frag[i].f_data += ss;*/
		frag[i].f_data += to_string(flag);
		frag[i].f_data += "IP���ݱ��ĵ�";
		frag[i].f_data += to_string(i + 1);
		frag[i].f_data += "��Ƭ����|";
	}
	else
	{
		frag[i].f_first = i * F_data_len;
		frag[i].f_last = (i * F_data_len) - 1 + (IP_data_len % F_data_len);
		frag[i].f_len = frag[i].f_last - frag[i].f_first + 1;
		frag[i].f_offset = i * (F_data_len / 8);
		frag[i].f_flag = flag;
		frag[i].f_more_fragment = '0';
		frag[i].f_dont_fragment = '0';
		frag[i].f_data = "|��";
		/*stringstream s;
		s << i + 1;
		string ss;
		s >> ss;
		frag[i].f_data += ss;*/
		frag[i].f_data += to_string(flag);
		frag[i].f_data += "IP���ݱ��ĵ�";
		frag[i].f_data += to_string(i + 1);
		frag[i].f_data += "��Ƭ����|";
	}
}

void hole_fragment_reassembly::GET_fragment(fragment *frag, unsigned short int F_NUM)
{
	cout << "first\tlast\tlength\toffset\tDF\tMF\tdata" << endl;
	for (int i = 0; i < F_NUM; i++)
	{
		cout << frag[i].f_first << ",\t"
			<< frag[i].f_last << ",\t"
			<< frag[i].f_len << ",\t"
			<< frag[i].f_offset << ",\t"
			<< frag[i].f_dont_fragment << ",\t"
			<< frag[i].f_more_fragment << ",\t"
			<< frag[i].f_data << endl;
	}
}

void hole_fragment_reassembly::SET_hole(unsigned short int f_len, unsigned short int f_offset, unsigned short int f_flag, char f_more_fragment)
{
	unsigned short int f_first = 8 * f_offset;
	unsigned short int f_last = f_first + f_len - 1;

	hole *p = h_head[f_flag];

	if (f_more_fragment == '0')
	{
		p->h_prev->h_last = f_last;
		hole_len[f_flag] = f_last;
	}

	do
	{
		if (f_first >= p->h_first && f_last <= p->h_last)
		{
			if (f_first == p->h_first && f_last < p->h_last)
			{
				for (int i = f_first; i <= f_last; i++)
					DATA[f_flag][i] = 1;
				p->h_first = f_last + 1;
				break;
			}
			if (f_first > p->h_first && f_last < p->h_last)
			{
				for (int i = f_first; i <= f_last; i++)
					DATA[f_flag][i] = 1;
				hole *t = new hole;                                             //����һ���¶�
				t->h_first = f_last + 1;                                        //��ͷΪ����Ƭ��β�ֽڵ���һ���ֽ�
				t->h_last = p->h_last;                                          //��βΪԭ���Ķ�β
				t->h_next = p->h_next;                                          //ָ��ԭ������һ����
				t->h_prev = p;                                                  //ָ��ԭ��

				p->h_last = f_first - 1;                                        //ԭ���Ķ�βΪ����Ƭ��ͷ�ֽڵ�ǰһ���ֽ�
				p->h_next = t;                                                  //ָ���½����Ķ�

				if (t->h_next != NULL)                                          //if�¶����滹�ж�������һ������ǰָ��ָ���¶�
					t->h_next->h_prev = t;
				if (t->h_next == NULL)                                          //if�¶�����û�ж�������һ������ǰָ��ָ���¶�
					h_head[f_flag]->h_prev = t;
				break;
			}
			if (f_first > p->h_first && f_last == p->h_last)
			{
				for (int i = f_first; i <= f_last; i++)
					DATA[f_flag][i] = 1;
				p->h_last = f_first - 1;
				break;
			}
			if (f_first == p->h_first && f_last == p->h_last)                   //һ�����պ�����
			{
				for (int i = f_first; i <= f_last; i++)
					DATA[f_flag][i] = 1;
				if (p->h_next == NULL)                                          //if������β��������һ������ǰָ��ָ�򱾶���ǰһ����
				{                                                               //���������ÿ�
					h_head[f_flag]->h_prev = p->h_prev;
					//delete p;
					p = NULL;
				}
				else if (p != h_head[f_flag])                                           //if�������м䶴
				{
					p->h_prev->h_next = p->h_next;
					p->h_next->h_prev = p->h_prev;
					//delete p;
				}
				else                                                            //if������ͷ��
				{
					p->h_next->h_prev = p->h_prev;
					p = p->h_next;
					//delete p;
				}
				break;
			}
		}
		else
		{
			p = p->h_next;
			continue;
		}
	} while (p != NULL);
}

int main()
{
	unsigned short int MTU = 0;                                       //����䵥Ԫ
	//unsigned short int IP_len;                                    //IP���ݱ�����

	int num = 0;
	cout << "������IP���ݱ���������";
	cin >> num;
	unsigned short int *IP_len = new unsigned short int[num];
	do
	{
		cout << "��������̫������䵥Ԫ(MTU)��";
		cin >> MTU;
	} while ((MTU - 20) % 8 != 0);

	cout << "�������IP���ݱ��ĳ���(����20�ֽڵ����ݱ��ײ�)\n";
	for (int i = 0; i < num; i++)
	{
		cout << "��" << i + 1 << "��IP���ݱ��ĳ��ȣ�";
		cin >> IP_len[i];
	}

	IP_data_len = new unsigned short int[num];
	for (int i = 0; i < num; i++)
		IP_data_len[i] = IP_len[i] - IP_head_len;
	//IP_data_len = IP_len - IP_head_len;

	unsigned short int *F_NUM = new unsigned short int[num];
	F_data_len = MTU - F_head_len;
	unsigned short int max_f_num = 0;
	for (int i = 0; i < num; i++)
	{
		if (IP_data_len[i] % F_data_len == 0)
		{
			F_NUM[i] = IP_data_len[i] / F_data_len;
		}
		else
		{
			F_NUM[i] = IP_data_len[i] / F_data_len + 1;
		}
		if (max_f_num < F_NUM[i])
			max_f_num = F_NUM[i];
		//if (IP_data_len % F_data_len == 0)                            //�����Ƭ����
		//{
		//	F_NUM = IP_data_len / F_data_len;
		//}
		//else
		//{
		//	F_NUM = IP_data_len / F_data_len + 1;
		//}
	}
	//const int MAX_F_NUM = max_f_num;
	for (int i = 0; i < num; i++)
	{
		cout << "i: " << F_NUM[i] << endl;
	}


	fragment **FRAG = new fragment*[num];
	hole_fragment_reassembly A;                                    //����IP���ݱ�����������
	for (int i = 0; i < num; i++)
	{
		fragment *frag = new fragment[F_NUM[i]];                          //������Ƭ�ṹ��
		FRAG[i] = frag;
		A.SET_fragment(frag, F_NUM[i], i, IP_data_len[i]);                                   //��IP���ݱ����з�Ƭ�����÷�Ƭ�ṹ������
		A.GET_fragment(frag, F_NUM[i]);                                   //�������Ƭ������
		cout << endl << endl;
	}

	DATA = new bool*[num];
	for (int i = 0; i < num; i++)
	{
		DATA[i] = new bool[1000]{ 0 };
		cout << "��ʼ��" << i << "�ն���\n";
		for (int j = 0; j < 1000; j++)
		{
			cout << DATA[i][j];
		}
		cout << endl << endl;

	}

	srand((unsigned int)time(NULL));

	int **RAND = new int*[num];
	for (int i = 0; i < num; i++)
	{
		RAND[i] = new int[F_NUM[i]];
	}

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < F_NUM[i]; j++)
		{
			RAND[i][j] = rand() % F_NUM[i];
			for (int k = 0; k < j; k++)
			{
				if (RAND[i][j] == RAND[i][k])
				{
					j--;
					break;
				}
			}
			//cout << RAND[i][j] << " ";
		}
		//cout << endl;
	}

	for (int i = 0; i < num; i++)
	{
		cout << "��" << i << "��IP���ݱ���Ƭ����˳��";
		for (int j = 0; j < F_NUM[i]; j++)
			cout << RAND[i][j];
		cout << endl;
	}

	int TOTAL_FRAG_NUM = 0;
	for (int i = 0; i < num; i++)
	{
		TOTAL_FRAG_NUM += F_NUM[i];
	}

	int rand_IP = 0;
	for (int i = 0; i < TOTAL_FRAG_NUM; i++)
	{
		rand_IP = rand() % num;
		if (F_NUM[rand_IP] > 0)
		{
			cout << endl << endl << "��" << rand_IP << "��IP���ݱ��ĵ�"
				<< RAND[rand_IP][F_NUM[rand_IP] - 1] << "��Ƭ����" << endl;
			A.SET_hole(FRAG[rand_IP][RAND[rand_IP][F_NUM[rand_IP] - 1]].f_len,
				FRAG[rand_IP][RAND[rand_IP][F_NUM[rand_IP] - 1]].f_offset,
				FRAG[rand_IP][RAND[rand_IP][F_NUM[rand_IP] - 1]].f_flag,
				FRAG[rand_IP][RAND[rand_IP][F_NUM[rand_IP] - 1]].f_more_fragment);
			F_NUM[rand_IP]--;

			for (int i = 0; i < A.GET_hole_len(rand_IP); i++)
			{
				cout << DATA[rand_IP][i];
			}
		}
		else
		{
			i--;
		}
	}

	cout << endl << endl;
	cout << "������ɶ�ȫ��������\n";

	for (int i = 0; i < num; i++)
	{
		cout << "��" << i << "��IP���ݱ��Ķ���������";
		for (int j = 0; j < A.GET_hole_len(i); j++)
		{
			cout << DATA[i][j];
		}
		cout << endl << endl;
	}

	for (int i = 0; i < num; i++)
	{
		delete[] RAND[i];
	}
	delete[] RAND;

	for (int i = 0; i < num; i++)
	{
		delete[] DATA[i];
	}
	delete[] DATA;

	for (int i = 0; i < num; i++)
	{
		delete[] FRAG[i];
	}
	delete[] FRAG;

	delete[] F_NUM;
	delete[] IP_len;

	system("pause");
	return 0;
}