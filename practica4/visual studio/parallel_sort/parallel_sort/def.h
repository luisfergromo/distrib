#define N 20000000
#define GRANULARITY 20000

struct LR {
    int     L;
    int     R;
};

struct Stack {
    struct LR data[N];
    int     size;
};

void Stack_Init(Stack *S)
{
    S->size = 0;
}

int Stack_Size(Stack *S)
{
	return S->size;
}

void Stack_Push(Stack *S, int L, int R)
{
	struct LR data;
	data.L=L;
	data.R=R;

    if (S->size < N)
        S->data[S->size++] = data;
    else
        fprintf(stderr, "Error: stack full\n");
}

struct LR Stack_Pop(Stack *S)
{
	if (S->size == 0)
        fprintf(stderr, "Error: stack empty\n");
    else
    {
		S->size--;
		return S->data[S->size];
	}
    
}