#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

//unsigned short int IP_data_len;                               //IP数据报数据部分长度
unsigned short int *IP_data_len;
unsigned short int IP_head_len = 20;                          //IP数据报头部长度

//unsigned short int F_NUM;                                     //IP数据报分片数量
unsigned short int F_data_len;                                //IP数据报分片数据部分长度
unsigned short int F_head_len = 20;                           //IP数据报分片头部长度

unsigned short int F_offset_multiple = 8;                     //IP数据报片偏移量为8的倍数

//bool date[1000] = { 0 };
bool **DATA;

struct hole                              //洞结构体
{
	unsigned short int h_first;          //洞头：洞的第一个字节位置
	unsigned short int h_last;           //洞尾：洞的最后一个字节位置
	struct hole *h_next;                 //指向下一个洞的指针
	struct hole *h_prev;                 //指向前一个洞的指针
};

struct fragment                          //分片结构体
{
	unsigned short int f_first;          //分片头
	unsigned short int f_last;           //分片尾
	unsigned short int f_len;            //分片数据部分长度
	unsigned short int f_offset;         //片偏移量
	unsigned short int f_flag;           //标识不同IP数据报
	char f_more_fragment;                //标识本分片是否为最后一个分片
	char f_dont_fragment;                //标识IP数据报是否可以分片
	string f_data;                       //分片的数据部分
};

class hole_fragment_reassembly                                                   //分片重组类
{
public:
	hole_fragment_reassembly();                                                  //无参构造函数，默认原始洞长度尾7000字节
	hole_fragment_reassembly(unsigned short int h_len);                          //有参构造函数，参数为指定原始洞长度h_len
	~hole_fragment_reassembly();                                              //析构函数
	void SET_fragment(fragment *frag, unsigned short int F_NUM, int flag, unsigned short int IP_data_len);                 //对IP数据报进行分片并设置属性值
	void GET_fragment(fragment *frag, unsigned short int F_NUM);                 //输出各IP数据报的属性值
	void SET_hole(unsigned short int f_len, unsigned short int f_offset, unsigned short int f_flag, char f_more_fragment);
	unsigned short int GET_hole_len(int i) { return hole_len[i]; }


private:
	unsigned short int hole_len[10];                                                  //原始洞长度
	hole *h_head[10];                                                                 //洞头指针
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
		frag[i].f_data = "|第";
		/*stringstream s;
		s << i + 1;
		string ss;
		s >> ss;
		frag[i].f_data += ss;*/
		frag[i].f_data += to_string(flag);
		frag[i].f_data += "IP数据报的第";
		frag[i].f_data += to_string(i + 1);
		frag[i].f_data += "分片内容|";
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
		frag[i].f_data = "|第";
		/*stringstream s;
		s << i + 1;
		string ss;
		s >> ss;
		frag[i].f_data += ss;*/
		frag[i].f_data += to_string(flag);
		frag[i].f_data += "IP数据报的第";
		frag[i].f_data += to_string(i + 1);
		frag[i].f_data += "分片内容|";
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
		frag[i].f_data = "|第";
		/*stringstream s;
		s << i + 1;
		string ss;
		s >> ss;
		frag[i].f_data += ss;*/
		frag[i].f_data += to_string(flag);
		frag[i].f_data += "IP数据报的第";
		frag[i].f_data += to_string(i + 1);
		frag[i].f_data += "分片内容|";
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
				hole *t = new hole;                                             //建立一个新洞
				t->h_first = f_last + 1;                                        //洞头为本分片的尾字节的下一个字节
				t->h_last = p->h_last;                                          //洞尾为原洞的洞尾
				t->h_next = p->h_next;                                          //指向原洞的下一个洞
				t->h_prev = p;                                                  //指向原洞

				p->h_last = f_first - 1;                                        //原洞的洞尾为本分片的头字节的前一个字节
				p->h_next = t;                                                  //指向新建立的洞

				if (t->h_next != NULL)                                          //if新洞后面还有洞：将下一个洞的前指针指向新洞
					t->h_next->h_prev = t;
				if (t->h_next == NULL)                                          //if新洞后面没有洞：将第一个洞的前指针指向新洞
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
			if (f_first == p->h_first && f_last == p->h_last)                   //一个洞刚好填满
			{
				for (int i = f_first; i <= f_last; i++)
					DATA[f_flag][i] = 1;
				if (p->h_next == NULL)                                          //if本洞是尾洞：将第一个洞的前指针指向本洞的前一个洞
				{                                                               //并将本洞置空
					h_head[f_flag]->h_prev = p->h_prev;
					//delete p;
					p = NULL;
				}
				else if (p != h_head[f_flag])                                           //if本洞是中间洞
				{
					p->h_prev->h_next = p->h_next;
					p->h_next->h_prev = p->h_prev;
					//delete p;
				}
				else                                                            //if本洞是头洞
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
	unsigned short int MTU = 0;                                       //最大传输单元
	//unsigned short int IP_len;                                    //IP数据报长度

	int num = 0;
	cout << "请输入IP数据报的数量：";
	cin >> num;
	unsigned short int *IP_len = new unsigned short int[num];
	do
	{
		cout << "请输入以太网最大传输单元(MTU)：";
		cin >> MTU;
	} while ((MTU - 20) % 8 != 0);

	cout << "请输入各IP数据报的长度(包括20字节的数据报首部)\n";
	for (int i = 0; i < num; i++)
	{
		cout << "第" << i + 1 << "个IP数据报的长度：";
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
		//if (IP_data_len % F_data_len == 0)                            //计算分片数量
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
	hole_fragment_reassembly A;                                    //创建IP数据报重组的类对象
	for (int i = 0; i < num; i++)
	{
		fragment *frag = new fragment[F_NUM[i]];                          //创建分片结构体
		FRAG[i] = frag;
		A.SET_fragment(frag, F_NUM[i], i, IP_data_len[i]);                                   //对IP数据报进行分片并设置分片结构体属性
		A.GET_fragment(frag, F_NUM[i]);                                   //输出各分片的属性
		cout << endl << endl;
	}

	DATA = new bool*[num];
	for (int i = 0; i < num; i++)
	{
		DATA[i] = new bool[1000]{ 0 };
		cout << "初始第" << i << "空洞：\n";
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
		cout << "第" << i << "个IP数据报分片发送顺序：";
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
			cout << endl << endl << "第" << rand_IP << "个IP数据报的第"
				<< RAND[rand_IP][F_NUM[rand_IP] - 1] << "分片到达" << endl;
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
	cout << "重组完成洞全部填满：\n";

	for (int i = 0; i < num; i++)
	{
		cout << "第" << i << "个IP数据报的洞填充情况：";
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