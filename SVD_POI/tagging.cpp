#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
struct POINT
{
    double x;
    double y;
    POINT(double xx, double yy): x(xx), y(yy){}
};

bool PtInAnyRect(POINT pCur, POINT pLT, POINT pLB, POINT pRB, POINT pRT )   
{   
    //任意四边形有4个顶点  
    int nCount = 4;  
    POINT RectPoints[4] = { pLT, pLB, pRB, pRT };  
    int nCross = 0;  
    for (int i = 0; i < nCount; i++)   
    {   
        //依次取相邻的两个点  
        POINT pStart = RectPoints[i];   
        POINT pEnd = RectPoints[(i + 1) % nCount];  
  
        //相邻的两个点是平行于x轴的，肯定不相交，忽略  
        if ( pStart.y == pEnd.y )   
            continue;  
  
        //交点在pStart,pEnd的延长线上，pCur肯定不会与pStart.pEnd相交，忽略  
        if ( pCur.y < min(pStart.y, pEnd.y) || pCur.y > max( pStart.y, pEnd.y ) )   
            continue;  
  
        //求当前点和x轴的平行线与pStart,pEnd直线的交点的x坐标  
        double x = (double)(pCur.y - pStart.y) * (double)(pEnd.x - pStart.x) / (double)(pEnd.y - pStart.y) + pStart.x;  
  
        //若x坐标大于当前点的坐标，则有交点  
        if ( x > pCur.x )   
            nCross++;   
    }  
  
    // 单边交点为偶数，点在多边形之外   
    return (nCross % 2 == 1);   
}  


int main(){
    FILE *ft = fopen("types.txt", "r");
    FILE *fd = fopen("raw.txt", "r");
    FILE *fo = fopen("in.txt", "w");

    vector<int> types;
    vector<vector<POINT> > points;
    points.resize(27);

    double *p = new double[8];
    double longi, lati, intensity;
    int type, count(0), time;

    while((fscanf(ft, "%lf,%lf %lf,%lf %lf,%lf %lf,%lf %d", &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7],&type )) != EOF){
        for (int i = 0; i < 4; ++i){
            POINT tmp(p[2*i], p[2*i+1]);
            points[count].push_back(tmp);
        }
        types.push_back(type);
        count++;
    }

    while((fscanf(fd, "%lf %lf %d %lf", &longi, &lati, &time, &intensity) != EOF)){
        POINT pCur(longi, lati);
        int mytype = 1;
        for(int i = 0; i < points.size(); i++){
            if(PtInAnyRect(pCur, points[i][0], points[i][1], points[i][2], points[i][3])){
                mytype = types[i] - 1;
                break;
            }
        }
        fprintf(fo, "%lf %lf %d %f %d\n", pCur.x, pCur.y, time, intensity, mytype);
    }

    return 0;
}