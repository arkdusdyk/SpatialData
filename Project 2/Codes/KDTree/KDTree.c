#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Set Data Value Range < 10000 (INF : 10001)
#define DATASIZE 1000001
#define INF 10001
#define MAX_DIM 2
#define COMPARE(a, b) ((a > b) ? a : b)

// kdtree의 node 자료구조 정의.
struct kd_node_t
{
    double x[MAX_DIM];
    struct kd_node_t *left, *right;
};

struct point {
    double x;
    double y;
};

struct Rect {
    double min_x, min_y;
    double max_x, max_y;
};

struct candidate_node {
    struct kd_node_t current_node;
    struct Rect rec;
    int level;
};
// 거리함수 정의.
inline double dist(struct kd_node_t *a, struct kd_node_t *b, int dim)
{
    double t, d = 0;
    while (dim--)
    {
        t = a->x[dim] - b->x[dim];
        d = d + t * t;
    }
    
    return d;
}

// swap 함수 정의.
void swap(struct kd_node_t *x, struct kd_node_t *y)
{
    double tmp[MAX_DIM];
    memcpy(tmp, x->x, sizeof(tmp));
    memcpy(x->x, y->x, sizeof(tmp));
    memcpy(y->x, tmp, sizeof(tmp));
}

// 중앙값 찾는 함수 정의. 중앙값은 kdtree의 node split point를 찾을 때 사용된다.
struct kd_node_t* find_median(struct kd_node_t *start, struct kd_node_t *end, int idx)
{
    if (end <= start) return NULL;
    if (end == start + 1)
        return start;
    
    struct kd_node_t *p, *store, *med;
    med = start + (end - start) / 2;
    double pivot;
    
    while (1)
    {
        pivot = med->x[idx];
        
        swap(med, end-1);
        for(store = p = start; p < end; p++)
            if(p->x[idx] < pivot)
            {
                if(p != store)
                    swap(p, store);
                store++;
            }
        swap(store, end - 1);
        
        if(store->x[idx] == med->x[idx])
            return med;
        if (store > med) end = store;
        else start = store;
    }
}

// recursion으로 kdtree를 build하는 함수.
struct kd_node_t* make_kdtree(struct kd_node_t *t, int len, int i, int dim)
{
    struct kd_node_t *n;
    
    if(!len) return NULL;
    
    if((n = find_median(t, t+len, i)))
    {
        i = (i + 1) % dim;
        n->left = make_kdtree(t, n-t, i, dim);
        n->right = make_kdtree(n+1, t+len-(n+1), i, dim);
    }
    
    return n;
}

double euclid_dist(struct candidate_node v, struct point qp)
{
    // returns euclidean distance of candidate node point v and query point qp
    double dist_x = (qp.x - v.current_node.x[0])*(qp.x - v.current_node.x[0]);
    double dist_y = (qp.y - v.current_node.x[1])*(qp.y - v.current_node.x[1]);
    return sqrt(dist_x + dist_y);
}

void rangeQuery(struct kd_node_t *p, struct point qp, double radius)
{
    //range query의 질의 조건인 질의 포인트와 질의 반경
    // p : kdtree, qp : query point, radius : range radius
    // 앞으로 할 것 : 
    // candidate_node 스택 만들기
    // kd tree root node부터 스택에 삽입 (level 1부터 DFS)
    int obj_cnt = 0;
    struct candidate_node *cand_stack = (struct candidate_node*)malloc(sizeof(struct candidate_node)*DATASIZE);
    struct candidate_node *root = (struct candidate_node*)malloc(sizeof(struct candidate_node));
    int tree_level = 1;                         // Odd : cut median of x, Even : cut median of y
    root->current_node = p[0];
    root->rec.min_x = 0;
    root->rec.min_y = 0;
    root->rec.max_x = INF;
    root->rec.max_y = INF;
    root->level = tree_level;

    int top = 0;
    cand_stack[top] = *root;
    while (top >= 0) {                                  // Depth First Search 로 트리 탐색
        struct candidate_node v;
        v = cand_stack[top];
        top--;
        if (euclid_dist(v,qp) <= radius) {              // Check if current node fits range query.
            printf("(%lf, %lf)\n", v.current_node.x[0], v.current_node.x[1]);
            obj_cnt++;
        }
        
        if (v.level % 2 == 1) {                         // Level Odd : cut median of x
            if (v.current_node.x[0] + radius >= qp.x )  // Check left
            {
                if (v.current_node.left != NULL){
                    struct candidate_node lc;
                    lc.current_node = *v.current_node.left;
                    lc.rec.min_x = v.rec.min_x;
                    lc.rec.min_y = v.rec.min_y;
                    lc.rec.max_x = v.current_node.x[0];
                    lc.rec.max_y = v.rec.max_y;
                    lc.level = v.level + 1;
                    cand_stack[top+1] = lc;               // push left child node to candidate stack     
                    top++;
                }
            }
            if (qp.x >= v.current_node.x[0] - radius) {      // Check right
                if(v.current_node.right != NULL){
                    struct candidate_node rc;
                    rc.current_node = *v.current_node.right;
                    rc.rec.min_x = v.current_node.x[0];
                    rc.rec.min_y = v.rec.min_y;
                    rc.rec.max_x = v.rec.max_x;
                    rc.rec.max_y = v.rec.max_y;
                    rc.level = v.level + 1;
                    cand_stack[top+1] = rc;                 // push right child node to candidate stack
                    top++;
                }
            }   
        }
        else {                                              // Level Even : cut median of y
            if (v.current_node.x[1] + radius >= qp.y) {     // check left
                if(v.current_node.left != NULL){
                    struct candidate_node lc;
                    lc.current_node = *v.current_node.left;
                    lc.rec.min_x = v.rec.min_x;
                    lc.rec.min_y = v.rec.min_y;
                    lc.rec.max_x = v.rec.max_x;
                    lc.rec.max_y = v.current_node.x[1];
                    lc.level = v.level+1;
                    cand_stack[top+1] = lc;
                    top++;
                }
            }
            if (qp.y >= v.current_node.x[1] - radius) {      // check right
                if(v.current_node.right != NULL){
                    struct candidate_node rc;
                    rc.current_node = *v.current_node.right;
                    rc.rec.min_x = v.rec.min_x;
                    rc.rec.min_y = v.current_node.x[1];
                    rc.rec.max_x = v.rec.max_x;
                    rc.rec.max_y = v.rec.max_y;
                    rc.level = v.level+1;
                    cand_stack[top+1] = rc;                // push right child node to candidate stack
                    top++;
                }
            }
        }
    }
    printf("count: %d\n", obj_cnt);
}

void kNNquery(struct kd_node_t *p, int K)
{
    //kNN query의 질의조건인 질의 포인트와 최근접이웃 개수
    
}


int main(void)
{
    struct kd_node_t *kd = (struct kd_node_t*)malloc(sizeof(struct kd_node_t)*DATASIZE);
    int query = 0, dataset = 0;
    double rad;     // radius for range query
    int k;          // k value for kNN query
    clock_t start_time, end_time;
    // Select Query Type. 1 : Range, 2 : KNN
    while(1){
        printf("Select Query Type\n");
        printf("1. Range Query\n");
        printf("2. kNN Query\n");
        scanf("%d", &query);
        if(query==1 || query == 2){
            break;
        }
        printf("Invalid Query Type!\n");
    }
    // Select Dataset, load dataset & construct KDtree
    while(1){
        printf("Select Dataset\n");
        printf("1. clustered_dataset.txt\n");
        printf("2. gaussian_dataset.txt\n");
        printf("3. uniformed_dataset.txt\n");
        scanf("%d", &dataset);
        if(dataset == 1 || dataset == 2 || dataset == 3){
            break;
        }
        printf("Invalid Dataset!\n");
    }
    // Load dataset, construct KD tree
    FILE *fp;
    if(dataset == 1){
        fp = fopen("../clustered_dataset.txt", "r");
    }
    else if(dataset == 2){
        fp = fopen("../gaussian_dataset.txt", "r");
    }
    else{
        fp = fopen("../uniformed_dataset.txt", "r");
    }
    int no = 0;
    double x,y;
    // Construct KD Tree
    while(fscanf(fp, "%lf, %lf", &x, &y)!= EOF){
        kd[no].x[0] = x;
        kd[no].x[1] = y;
        kd[no].left = NULL;
        kd[no].right = NULL;
        no++;
    }
    kd = make_kdtree(kd, no, 0, MAX_DIM);
    struct point query_p;
    if(query == 1){                         // Range Query
        printf("[Range Query]\n");
        printf("Enter Query Point (x,y):\n");
        printf("ex)30 50\n");
        scanf("%lf %lf", &query_p.x, &query_p.y);
        printf("Enter Range Radius:\n");
        scanf("%lf", &rad);
        start_time = clock();
        rangeQuery(kd, query_p, rad);
        end_time = clock();
        printf("Query Time : %lf\n", (double)(end_time-start_time)/CLOCKS_PER_SEC);
    }
    else if (query == 2){               // kNN Query
        printf("[KNN Query]\n");
    }
    return 0;
}
