#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define DATASIZE 1000000
struct point {
    double x;
    double y;
};
double euclid_dist(struct point v, struct point qp)        //distance btw two points
{
    // returns euclidean distance of two points
    double dist_x = (qp.x - v.x)*(qp.x - v.x);
    double dist_y = (qp.y - v.y)*(qp.y - v.y);
    return sqrt(dist_x + dist_y);
}
void rangeQuery(struct point *dp, struct point qp, double radius){	// bruteforce range Query
	int obj_cnt;
	for(int i=0;i<DATASIZE;i++){
		if(euclid_dist(dp[i],qp) < radius){
			obj_cnt++;
		}
	}
	printf("Query Object Count: %d\n", obj_cnt);
}
void kNNquery(struct point *dp, struct point qp, int k){	// bruteforce kNN Query
	
}
int main(void)
{
	int query = 0, dataset = 0;
    double rad;     // radius for range query
    int k;          // k value for kNN query
    struct point *input_data;
    input_data = (struct point*)malloc(sizeof(struct point)*DATASIZE);
    clock_t start_time, end_time;
    // Select Query Type. 1 : Range, 2 : KNN
    printf("Bruteforce Query!!!\n");
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
    // Select Dataset, load dataset
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
    // Load dataset
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
    int no =0;
    double x,y;
    while(fscanf(fp, "%lf, %lf", &x, &y)!= EOF){
        	input_data[no].x = x;
        	input_data[no].y = y;
        	no++;
    }
    struct point query_p;
    if(query == 1){                         // Range Query
        printf("[Range Query]\n");
        printf("Enter Query Point (x,y):\n");
        printf("ex)30 50\n");
        scanf("%lf %lf", &query_p.x, &query_p.y);
        printf("Enter Range Radius:\n");
        scanf("%lf", &rad);
        start_time = clock();
    	rangeQuery(input_data, query_p, rad);
        end_time = clock();
        printf("Query Time : %lf\n", (double)(end_time-start_time));
    }
    else if (query == 2){               // kNN Query
        printf("[KNN Query]\n");
        printf("Enter Query Point (x,y):\n");
        printf("ex)30 50\n");
        scanf("%lf %lf", &query_p.x, &query_p.y);
        printf("Enter K:\n");
        scanf("%d", &k);
        start_time = clock();
        kNNquery(input_data, query_p, k);
        end_time = clock();
        printf("Query Time : %lf\n", (double)(end_time-start_time));
    }

	return 0;
}
