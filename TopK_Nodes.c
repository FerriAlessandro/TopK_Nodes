#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define DIM 50     // Length of word "AddGraph"
#define LEN_NUM 10 // max num of digits of a number

typedef struct
{
    int distance;
    int present;
} distances_t; // Keep track of the various distances (dist[0] will contain the distance between 0 and 1, dist[1] the distance between 0 and 2 etc..)

typedef struct queue_t
{
    int node;
    int distance;
    struct queue_t *next;
    struct queue_t *prev;

} queue_t;

int custom_atoi(char *string)
{
    int dim, ris;
    ris = 0;
    dim = 0;
    while (string[dim] != '\0')
    {
        ris = ris * 10 + (string[dim] - '0');
        dim++;
    }
    return ris;
}

queue_t *insert_in_order(queue_t *head, int val, int index) // insertings in ascending order : 1->2->3.., takes as input the head of the list, value of the distance and index of the node
{
    queue_t *tmp, *curr;
    tmp = malloc(sizeof(queue_t));
    tmp->node = index;
    tmp->distance = val;
    tmp->next = NULL;
    tmp->prev = NULL;
    if (!head)
        return tmp;

    else
    {
        if (tmp->distance < head->distance)
        {
            tmp->next = head;
            head->prev = tmp;
            return tmp;
        }
        else
        {
            curr = head;

            while ((curr->next) && (curr->next->distance < tmp->distance))
                curr = curr->next;
            if (!curr->next)
            {
                curr->next = tmp;
                tmp->prev = curr;
            }
            else
            {
                tmp->next = curr->next;
                curr->next->prev = tmp;
                tmp->prev = curr;
                curr->next = tmp;
            }
            return head;
        }
    }
}
queue_t *delete_from_head(queue_t *head)
{
    queue_t *tmp;
    if (head->next)
    {
        tmp = head->next;
        tmp->prev = NULL;
    }
    else
        tmp = NULL;
    free(head);
    return tmp;
}

void cancella(queue_t *tmp)
{
    if (tmp->next)
    {
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
        free(tmp);
    }
    else
    {
        tmp->prev->next = NULL;
        free(tmp);
    }
}

queue_t *find_and_delete(queue_t *head, int index)
{
    queue_t *tmp, *prev;
    int found;
    tmp = head;
    prev = NULL;
    found = 0;
    while (!found)
    {
        if (tmp->node == index)
            found = 1;
        else
        {
            prev = tmp;
            tmp = tmp->next;
        }
    }
    if (prev)
    {
        prev->next = tmp->next;
        free(tmp);
        return head;
    }
    else
    {
        head = tmp->next;
        free(tmp);
        return head;
    }
}
void swap(distances_t *arr, int a, int b)
{
    distances_t tmp;
    tmp = arr[a];
    arr[a] = arr[b];
    arr[b] = tmp;
}
void max_heapify(distances_t *leaderboard, int n, int dim_heap)
{
    int left, right, max_pos;
    left = 2 * n + 1;
    right = 2 * n + 2;
    if (left < dim_heap && leaderboard[left].distance > leaderboard[n].distance)
        max_pos = left;
    else
        max_pos = n;
    if (right < dim_heap && leaderboard[right].distance > leaderboard[max_pos].distance)
        max_pos = right;
    if (max_pos != n)
    {
        swap(leaderboard, max_pos, n);
        max_heapify(leaderboard, max_pos, dim_heap);
    }
}
void fix_heap(distances_t *leaderboard, int dim_heap, int i)
{
    if (dim_heap >= 2 * i + 1)
    {
        if (leaderboard[i].distance == leaderboard[i * 2 + 1].distance)
        {
            fix_heap(leaderboard, dim_heap, i * 2 + 1);
            if (leaderboard[i].present < leaderboard[i * 2 + 1].present)
                swap(leaderboard, i, i * 2 + 1);
        }
    }
    if (dim_heap >= 2 * i + 2)
    {
        if (leaderboard[i].distance == leaderboard[i * 2 + 2].distance)
        {
            fix_heap(leaderboard, dim_heap, i * 2 + 2);
            if (leaderboard[i].present < leaderboard[i * 2 + 2].present)
                swap(leaderboard, i, i * 2 + 2);
        }
    }
}

void build_max_heap(distances_t *leaderboard, int dim_heap)
{
    int i;
    for (i = dim_heap / 2 - 1; i >= 0; i--)
        max_heapify(leaderboard, i, dim_heap);
}

void AddGraph(int d, distances_t *leaderboard, int *dim_heap, int k, int graph_number)
{

    int i, j, p, q, mat[d][d], not_degenerate = 0, sum, last = 0; // i, j for matrix; p to go through the string and acquire the numbers; not_degenerate to recognize the case in which 0 has no connections
    char per_atoi[LEN_NUM + 1], per_parsing[10 * d + 100];
    distances_t arr_distances[d - 1]; // array of distances from node 0
    queue_t *head = NULL, *tmp = NULL;
    for (i = 0; i < d; i++) // get the strings row by row and insert them in the matrix
    {
        if (fgets(per_parsing, 10 * d + d, stdin) != NULL)
        {
            for (j = 0, p = 0; j < d; j++) // insert the columns
            {
                q = 0;
                while (per_parsing[p] >= '0' && per_parsing[p] <= '9')
                {
                    per_atoi[q] = per_parsing[p];
                    p++;
                    q++;
                }
                per_atoi[q] = '\0';
                p++;
                mat[i][j] = custom_atoi(per_atoi);
            }
        }
    }
    not_degenerate = 0;
    for (i = 1; i < d; i++)
    {
        if (mat[0][i] != 0)
        {
            arr_distances[i - 1].distance = mat[0][i]; // place in the array the direct distances from node 0
            arr_distances[i - 1].present = 1;
            head = insert_in_order(head, arr_distances[i - 1].distance, i);
            not_degenerate = 1;
        }
        else
        {
            arr_distances[i - 1].distance = INT_MAX; // if the distance to node 0 is 0, the node cannot reach node 0 directly, hence the distance is infinite
            arr_distances[i - 1].present = 0;
        }
    }

    if (not_degenerate)
    {
        while (head) // while there's something in the queue
        {
            i = head->node;
            tmp = head;             // extract from the queue the element whose adjacents i want to analyze
            for (j = 1; j < d; j++) // cycle through the adjacents
            {
                if (j != i && mat[i][j] != 0) // if i'm not on a self loop and there's a link between i and j
                {
                    if (mat[i][j] + arr_distances[i - 1].distance < arr_distances[j - 1].distance) // dijkstra
                    {
                        arr_distances[j - 1].distance = mat[i][j] + arr_distances[i - 1].distance;
                        if (!(arr_distances[j - 1].present)) // if not already present, add it
                        {
                            head = insert_in_order(head, arr_distances[j - 1].distance, j);
                            arr_distances[j - 1].present = 1;
                        }
                        else
                        {
                            head = find_and_delete(head, j);
                            head = insert_in_order(head, arr_distances[j - 1].distance, j);
                        }
                    }
                }
            }
            arr_distances[i - 1].present = 1;
            if (head->node != i)
                cancella(tmp);
            else
                head = delete_from_head(head);
        }
        for (i = 0, sum = 0; i < d - 1; i++) // sum the distances
            if (arr_distances[i].distance != INT_MAX)
                sum = sum + arr_distances[i].distance;
    }
    else
        sum = 0;

    if (*(dim_heap) + 1 == k)
        last = 1;
    if (*(dim_heap) < k)
    {
        leaderboard[*dim_heap].present = graph_number;
        leaderboard[*dim_heap].distance = sum;
        (*dim_heap) = (*dim_heap) + 1;
    }
    else
    {
        if (sum < leaderboard[0].distance)
        {
            leaderboard[0].distance = sum;
            leaderboard[0].present = graph_number;
            max_heapify(leaderboard, 0, k);
            fix_heap(leaderboard, k, 0);
        }
    }

    if (last)
    {
        last = 0;
        build_max_heap(leaderboard, k);
    }
}

int main()
{
    int d, k, aggiungi = 0, topk = 0, dim_heap = 0, graph_number = -1, i; // d=number of nodes, k= leaderboard length, buffer=max dimension of the row to read
    char comando[DIM + 2], foo[5];                                        // AddGraph or TopK
    if (scanf("%d%d", &d, &k) == 2)
        ;
    if (fgets(foo, 5, stdin) != NULL)
        ;
    distances_t leaderboard[k]; // leaderboards array
    if (fgets(comando, DIM + 1, stdin) != NULL)
    {

        if (comando[0] == 'A')
            aggiungi = 1;
        if (comando[0] == 'T')
            topk = 1;
        dim_heap = 0;
        while (aggiungi || topk)
        {
            if (aggiungi)
            {
                aggiungi = 0;
                graph_number++;
                AddGraph(d, leaderboard, &dim_heap, k, graph_number); // number of nodes, leaderboard,  leaderboard current dimension, leaderboard max dimension and graph number
            }
            else if (topk)
            {
                topk = 0;
                if (dim_heap > 0)
                {
                    for (i = 0; i < dim_heap - 1; i++)
                        printf("%d ", leaderboard[i].present);
                    printf("%d", leaderboard[dim_heap - 1].present);
                    printf("\n");
                }
                else
                    printf("\n");
            }
            if (fgets(comando, DIM + 1, stdin) != NULL)
            {

                if (comando[0] == 'A')
                    aggiungi = 1;
                else if (comando[0] == 'T')
                    topk = 1;
            }
        }
    }
    return 0;
}
