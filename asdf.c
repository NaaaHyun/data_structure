#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node *node;
typedef struct PriorityQueue *PriorityQueue;
typedef struct ElementType ElementType;

node vertex_name[100];
int **adjacency;
int num_vertex;

struct Node{
	int vertex_no;
	node *pred;
	int d;
	int heap_index;
};

struct ElementType{
	int vertex;
	int priority;
};

struct PriorityQueue{
	int Capacity;
	int Size;
	ElementType *Elements;
};

node CreateNode(int vertex_num){
	node n;

	n = malloc(sizeof(struct Node));
	if (n == NULL){
		printf("Out of space!");
	}

	n->pred = malloc(sizeof(node));
	if (n->pred == NULL){
		printf("Out of space!");
	}

	n->vertex_no = vertex_num;
	n->pred = NULL;
	n->d = 1000;
	n->heap_index = 0;

	return n;
}

PriorityQueue CreateQueue(int queuesize){
	PriorityQueue Q;

	Q = malloc(sizeof(struct PriorityQueue));
	if (Q == NULL){
		printf("Out of space!");
	}

	Q->Elements = (ElementType *)malloc(sizeof(ElementType)* (queuesize + 1));
	if (Q->Elements == NULL){
		printf("Out of space!");
	}

	Q->Capacity = queuesize;
	Q->Size = 0;

	return Q;
}

void SetIndex(int num, int index, PriorityQueue Q){
	int i;

	for (i = 1; i <= num_vertex; i++){
		if (vertex_name[i]->vertex_no == Q->Elements[num].vertex){
			vertex_name[i]->heap_index = index;
			break;
		}
	}
}

void Insert(node n, PriorityQueue Q){
	int i;
	ElementType E;

	if (Q->Size == Q->Capacity){
		printf("Priority queue is full\n");
		return;
	}

	E.vertex = n->vertex_no;
	E.priority = n->d;

	if (Q->Size == 0){
		Q->Elements[1] = E;
		n->heap_index = 1;
		Q->Size++;
		return;
	}

	for (i = ++Q->Size; Q->Elements[i / 2].priority > E.priority; i /= 2){
		Q->Elements[i] = Q->Elements[i / 2];
		SetIndex(i / 2, i, Q);
	}
	Q->Elements[i] = E;
	n->heap_index = i;
}

ElementType DeleteMin(PriorityQueue Q){
	int i, Child;
	ElementType MinElement, LastElement;

	MinElement = Q->Elements[1];
	LastElement = Q->Elements[Q->Size--];

	for (i = 1; i * 2 <= Q->Size; i = Child){
		Child = i * 2;
		if (Child != Q->Size && Q->Elements[Child + 1].priority < Q->Elements[Child].priority){
			Child++;
		}
		if (LastElement.priority > Q->Elements[Child].priority){
			Q->Elements[i] = Q->Elements[Child];
			SetIndex(Child, i, Q);
		}
		else{
			break;
		}
	}

	Q->Elements[i] = LastElement;
	SetIndex(Q->Size + 1, i, Q);

	return MinElement;
}

void DecresePriority(node n, PriorityQueue Q){
	int i;
	ElementType E;

	E.vertex = n->vertex_no;
	E.priority = n->d;

	for (i = 2; i <= n->heap_index; i++){
		if (Q->Elements[i].priority > E.priority){
			if (Q->Elements[i].vertex == E.vertex){
				Q->Elements[i].priority = E.priority;
				break;
			}
			else{
				Q->Elements[n->heap_index] = Q->Elements[i];
				SetIndex(n->heap_index, n->heap_index, Q);

				Q->Elements[i] = E;
				n->heap_index = i;
				break;
			}
		}
	}
}

void Dijkstra(PriorityQueue Q, node start_node, node end_node){
	node temp;
	ElementType E;
	int i, result[10];

	Q->Size = 0;

	for (i = 1; i <= num_vertex; i++){
		vertex_name[i]->pred = NULL;
		vertex_name[i]->d = 1000;
		vertex_name[i]->heap_index = 0;
	}
	start_node->d = 0;

	for (i = 1; i <= num_vertex; i++){
		if (adjacency[start_node->vertex_no][i] != 1000){
			vertex_name[i]->d = adjacency[start_node->vertex_no][i];
			vertex_name[i]->pred = &vertex_name[start_node->vertex_no];
		}
	} 

	for (i = 1; i <= num_vertex; i++){
		Insert(vertex_name[i], Q);
	} 

	while (Q->Size != 0){
		E = DeleteMin(Q);

		if (E.vertex == end_node->vertex_no){
			break;
		}

		for (i = 1; i <= num_vertex; i++){
			if (E.priority + adjacency[E.vertex][i] < vertex_name[i]->d){
				vertex_name[i]->d = E.priority + adjacency[E.vertex][i];
				vertex_name[i]->pred = &vertex_name[E.vertex];
				DecresePriority(vertex_name[i], Q);
			}
		}
	}

	if (end_node->d == 1000){
		printf("nowhere to go %d\n", end_node->vertex_no);
	}
	else{
		i = 0;
		temp = end_node;
		while (temp->pred != NULL){
			result[i++] = temp->vertex_no;
			temp = *temp->pred;
		}
		result[i++] = temp->vertex_no;
		while (result[--i] != end_node->vertex_no){
			printf("%d ", result[i]);
		}
		printf("%d\n", result[i]);
	}
}

int main(int argc, char *argv[]){

	char buf[200], pair[20], restart;
	int start_vertex, end_vertex, weight, i, j;
	char* vertex = NULL;
	PriorityQueue Q = NULL;
	num_vertex = 0;

	FILE *file = fopen(argv[1], "rt");

	fgets(buf, sizeof(buf), file);

	vertex = strtok(buf, " \n");
	vertex_name[++num_vertex] = CreateNode(atoi(vertex));

	while (vertex != NULL){
		vertex = strtok(NULL, " \n");
		if (vertex == NULL){
			break;
		}
		vertex_name[++num_vertex] = CreateNode(atoi(vertex));
	}

	adjacency = (int **)malloc(sizeof (int *)* (num_vertex + 1));
	for (i = 0; i <= num_vertex; i++) {
		adjacency[i] = (int *)malloc(sizeof (int)* (num_vertex + 1));
		for (j = 0; j <= num_vertex; j++){
			adjacency[i][j] = 1000;
		}
	}

	while (fscanf(file, "%s", pair) != EOF){
		start_vertex = atoi(strtok(pair, "-"));
		end_vertex = atoi(strtok(NULL, "-"));
		weight = atoi(strtok(NULL, "\0"));
		adjacency[start_vertex][end_vertex] = weight;
	}

	fclose(file);

	Q = CreateQueue(num_vertex);

	while (1){
		printf("Start : ");
		scanf("%d", &start_vertex);
		printf("End : ");
		scanf("%d", &end_vertex);

		Dijkstra(Q, vertex_name[start_vertex], vertex_name[end_vertex]);

		printf("restart?(Y/N) : ");
		scanf(" %c", &restart);

		if (restart == 'Y'){
			continue;
		}
		else{
			break;
		}
	}

	for (i = 1; i <= num_vertex; i++){
		vertex_name[i]->pred = NULL;
		free(vertex_name[i]->pred);
		free(vertex_name[i]);
	}
	for (i = 0; i <= num_vertex; i++){
		free(adjacency[i]);
	}
	free(adjacency);
	free(Q->Elements);
	free(Q);

	return 0;
}
