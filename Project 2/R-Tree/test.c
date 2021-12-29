/**
   rtree lib usage example app.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "rtree.h"
#define DATASIZE 1000000

// RTREEMBR rects[] = {
//     { {0, 0, 0, 2, 2, 0} },  /* xmin, ymin, zmin, xmax, ymax, zmax (for 3 dimensional RTree) */
//     { {5, 5, 0, 7, 7, 0} },
//     { {8, 5, 0, 9, 6, 0} },
//     { {7, 1, 0, 9, 2, 0} }
// };

// RTREEMBR search_rect = {
//     {6, 4, 0, 10, 6, 0}    search will find above rects that this one overlaps 
// };

RTREEMBR rects[DATASIZE];
int nrects = sizeof(rects) / sizeof(rects[0]);
double rad;
int obj_cnt = 0;
struct point {
    double x;
    double y;
};

double euclid_dist(struct point v, struct point qp)        //distance btw candid_node and point
{
    // returns euclidean distance of candidate node point v and query point qp
    double dist_x = (qp.x - v.x)*(qp.x - v.x);
    double dist_y = (qp.y - v.y)*(qp.y - v.y);
    return sqrt(dist_x + dist_y);
}

double mbr_point_dist(RTREEMBR mbr, struct point qp)           //distance btw query point and RtreeMBR
{
    double dist_x = ((mbr.bound[1] - qp.x)*(mbr.bound[1] - qp.x));
    double dist_y = ((mbr.bound[2]-qp.y)*(mbr.bound[2]-qp.y));
    return sqrt(dist_x + dist_y);
}   

int MySearchCallback(int id, void* arg) 
{
    /* Note: -1 to make up for the +1 when data was inserted */
    //fprintf (stdout, "Hit data mbr %d ", id-1);
    return 1; /* keep going */
}
void rangeQuery(RTREENODE *r_node, struct point qp, double radius)
{
    int i;
    if(r_node->level > 0){
        for(i=0;i<MAXCARD;i++){
            if(r_node->branch[i].child != NULL){
                double min_x = r_node->branch[i].mbr.bound[0];
                double min_y = r_node->branch[i].mbr.bound[1];
                double max_x = r_node->branch[i].mbr.bound[2];
                double max_y = r_node->branch[i].mbr.bound[3];

                if((rad >= (min_x - qp.x)) && (rad>= (qp.x-max_x)) && (rad >= (min_y - qp.y)) && (rad >= (qp.y - min_y))){
                    rangeQuery(r_node->branch[i].child, qp, radius);
                }
            }
        }
    }
    else{
        for(i=0;i<MAXCARD;i++){
            if(r_node->branch[i].child){
                if(mbr_point_dist(r_node->branch[i].mbr, qp) <= sqrt(radius)){
                    obj_cnt++;
                }
            }
        }
    }
}
int main()
{
    RTREENODE* root = RTreeCreate();
    int query, dataset;
    int k;          // k value for kNN query
    int no = 0, i;
    double x,y;
    clock_t start_time, end_time;
    printf("RTree Query!!!\n");
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
    // Select Dataset, load dataset & construct Rtree
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
    // Load dataset, construct RTree
    FILE *fp;
    if(dataset == 1){
        fp = fopen("../datasets/clustered_dataset.txt", "r");
    }
    else if(dataset == 2){
        fp = fopen("../datasets/gaussian_dataset.txt", "r");
    }
    else{
        fp = fopen("../datasets/uniformed_dataset.txt", "r");
    }
    printf("Constructing MBR and R-Tree...\n");
    // Readline, make MBR
    while(fscanf(fp, "%lf, %lf", &x, &y)!= EOF){
        rects[no].bound[0] = x;
        rects[no].bound[1] = y;
        rects[no].bound[2] = x;
        rects[no].bound[3] = y;
        no++;
    }
    /* Insert all the testing data rects */
    for(i=0; i<nrects; i++){
        RTreeInsertRect(&rects[i],  /* the mbr being inserted */
                        i+10,        /* i+1 is mbr ID. ID MUST NEVER BE ZERO */
                        &root,        /* the address of rtree's root since root can change undernieth*/
                        0            /* always zero which means to add from the root */
            );
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
        rangeQuery(root, query_p,rad*rad);
        end_time = clock();
        printf("Query Object Count: %d\n", obj_cnt);
        printf("Query Time : %.4lf ms\n", (double)(end_time-start_time));
    }
    //else if (query == 2){               // kNN Query
    //     printf("[KNN Query]\n");
    //     printf("Enter Query Point (x,y):\n");
    //     printf("ex)30 50\n");
    //     scanf("%lf %lf", &query_p.x, &query_p.y);
    //     printf("Enter K:\n");
    //     scanf("%d", &k);
    //     start_time = clock();
    //     kNNquery(kd, query_p, k);
    //     end_time = clock();
    //     printf("Query Time : %lf\n", (double)(end_time-start_time)/CLOCKS_PER_SEC);
    // }
    // return 0;
    
    // int i, nhits;
    // fprintf (stdout, "nrects = %d ", nrects);   
    

    RTreeDestroy (root);

    return 0;
}
