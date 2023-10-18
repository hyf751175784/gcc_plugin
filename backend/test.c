#include <stdio.h>
#include <stdbool.h>

int GG = 1;

static int fn_test_1(int a, int b)
{
	bool c = a > b;
	bool d = c == b;
	int e = a + b;
	int f = -b;
    b = f + b;
	if (f == 0)
	{
		return 1;
	}

		switch (f) 
	{
	case 1: // 代码块 
			break;
	case 2: // 代码块 
			break;
	default: // 代码块
	} 


	return 0;
}

static int fn_test_2(int a, int b)
{
	bool c = a == b;
	bool d = c == b;
	int e = a + b;
	int f = b + e;

	if (c)
	{
		return 1;
	}
	return 0;
}

static int fn_test_3(int a, int b)
{
	bool c = a == b;
	bool d = c == b;
	int e = a - b;
	int f = b / e;

	if (c)
	{
		return 1;
	}
	return 0;
}

int main(void)
{
	FILE *ff;
	ff = fopen("1.txt", "r");
	// GG = 4;
	GG + 5;
	int x = 0 + 3;
	x = x + GG;
	int f = 0;
	x = fn_test_1(f, 2);
	printf("%d \n", x);
	printf("%d \n", GG);
	return 0;
}
