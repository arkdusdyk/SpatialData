#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define DATASIZE 1000000
struct point {
    double x;
    double y;
};
struct knn {
	struct point p;
	double dist;
};
double euclid_dist(struct point v, struct point qp)        //distance btw two points
{
    // returns euclidean distance of two points
    double dist_x = (qp.x - v.x)*(qp.x - v.x);
    double dist_y = (qp.y - v.y)*(qp.y - v.y);
    return sqrt(dist_x + dist_y);
}
void rangeQuery(struct point *dp, struct point qp, double radius){	// bruteforce range Query
	int obj_cnt = 0;
	for(int i=0;i<DATASIZE;i++){
		if(euclid_dist(dp[i],qp) < radius){
			printf("(%lf, %lf)\n", dp[i].x, dp[i].y);
			obj_cnt++;
		}
	}
	printf("Query Object Count: %d\n", obj_cnt);
}
void kNNquery(struct point *dp, struct point qp, int k){	// bruteforce kNN Query
	int obj_cnt = 0;
	struct knn *top_k;
	top_k = (struct knn*)malloc(sizeof(struct knn)*k);
	for(int i=0;i<DATASIZE;i++){
		double dist = euclid_dist(dp[i],qp);
		if(obj_cnt==0){
			top_k[0].p = dp[i];
			top_k[0].dist = dist;
			obj_cnt++;
		}
		else{
			if(dist<top_k[obj_cnt-1].dist){
				int j;
				for(j=0;j<obj_cnt;j++){
					if(dist < top_k[j].dist){
						break;
					}
				}
				if(obj_cnt<k){
					for(int l=obj_cnt;l>j;l--){
						top_k[l].p = top_k[l-1].p;
						top_k[l].dist = top_k[l-1].dist;
					}
					top_k[j].p = dp[i];
					top_k[j].dist = dist;
					obj_cnt++;
				}
				else{
					for(int l=k-1;l>j;l--){
						top_k[l].p = top_k[l-1].p;
						top_k[l].dist = top_k[l-1].dist;
					}
					top_k[j].p = dp[i];
					top_k[j].dist = dist;
				}
			}
		}
	}

	for(int i=0;i<k;i++){
		printf("(%lf, %lf) : %lf\n", top_k[i].p.x, top_k[i].p.y, top_k[i].dist);
	}
	printf("Query Object Count: %d\n", obj_cnt);
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
