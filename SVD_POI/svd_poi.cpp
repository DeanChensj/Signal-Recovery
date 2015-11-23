/*
Author: Shangjie Chen
Email:deancsj@gmail.com
*/
//Header file of the class NEWSVD.
#include <vector>
#include <cstring>
#include <utility>
#include <cmath>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include <ctime>

using namespace std;

struct Data{  
    int locationIndex; 
    int timeIndex; 
    double intensity;   
    int type; 
    Data(int li, int ti, double it, int tp): locationIndex(li), timeIndex(ti), intensity(it), type(tp){}
 };

enum Type {Downtown, LivingQuarter, Square, Greenbelt};

/*
    Data format: locationIndex timeIndex intensity type
*/
vector<vector<Data> > train_data; 
vector<Data> cross_data; 

const double gama = 0.0055; // 步长
const double lambda = 0.01889; // 0.01889
const int factors = 15;
const int locationSlots = 400;
const int timeSlots = 2;
const int typeNums = 3;
double globalAverage = 0.0; //全局平均值
vector<double> timeSlotsBias(timeSlots, 0); //用于存储各个timeSlot的平均强度
vector<double> locationSlotsBias(locationSlots, 0); //用于存储各个网格的平均强度
vector<vector<double> > Q(timeSlots, vector<double>(factors, 0)); //关于时间的factor矩阵
vector<vector<double> > P(locationSlots, vector<double>(factors, 0)); //关于位置的factor矩阵
vector<vector<double> > Y(locationSlots, vector<double>(factors, 0));
vector<vector<double> > sumY(typeNums, vector<double>(factors, 0));

double predictIntensity(int locationIndex, int timeIndex, int type){
    double tmp = 0.0;
    int size = train_data[type].size();
    double sqrtNum = 0.0;
    if (size > 1) sqrtNum = 1/(sqrt(size));
    for(size_t i = 0; i < factors; i++){
        tmp += (P[locationIndex][i] + sumY[type][i] * sqrtNum) * Q[timeIndex][i];
    }
    double intensity = globalAverage + locationSlotsBias[locationIndex] + timeSlotsBias[timeIndex] + tmp;

    if (intensity >= -45) {
        intensity = -45;
    } else if (intensity <= -99) {
        intensity = -99;
    }
    return intensity;
}

double getRmse() {
    int timeIndex, locationIndex, type;
    int n = 0;
    double intensity, rmse(0);
    for (const auto &ch:cross_data){
        timeIndex = ch.timeIndex;
        locationIndex = ch.locationIndex;
        intensity = ch.intensity;
        type = ch.type;
        ++n;
        double predict = predictIntensity(locationIndex, timeIndex, type);
        //double predict = -45;
        //cout << "predict " << predict << endl;
        rmse += (intensity - predict) * (intensity - predict);
        //cout << "rmse " << rmse << endl;
    }
    
    return sqrt(rmse / n);
}


void train(){
    int locationIndex, timeIndex;
    double intensity;
    int type;

    // 遍历每一种类型的地点
    for(int i = 0; i < typeNums; i++){

        int size = train_data[i].size(), cnt(0);
        double sqrtNum = 0;
         if (size > 1) sqrtNum = 1 / (sqrt(size));
     
         // 对于不同分类下的每一个数据点，计算出这个类别的sumY， 对每个用户更新他们的Y，加到sumY中
        for (const auto &ch: train_data[i]) {

            //如果要更新全部的Y的话，在这里更新sumY

            int timeIndex = ch.timeIndex;
            int locationIndex = ch.locationIndex;
            int type = ch.type;
            double intensity = ch.intensity;
            double predict = predictIntensity(locationIndex, timeIndex, type);
            double error = intensity - predict;
            
            // 更新 bias
            timeSlotsBias[timeIndex] += gama * (error - lambda * timeSlotsBias[timeIndex]);
            locationSlotsBias[locationIndex] += gama * (error - lambda * locationSlotsBias[locationIndex]);

            // 更新 Q P Y sumY
             for (int j = 0; j < factors; ++j) {
                double old_q = Q[timeIndex][j];
                double old_p = P[locationIndex][j];
                double old_y = Y[locationIndex][j];

                Q[timeIndex][j] += gama * (error * old_p - lambda * old_q);
                P[locationIndex][j] += gama * (error * (old_q + sqrtNum * sumY[i][j]) - lambda * old_p);
                
                 // 此处Y只更新了自己的，理论上应更新全局的
                if(cnt % 5 == 0){
                    for(int kk = 0; kk < size; kk++){
                        int locI = train_data[i][kk].locationIndex;
                        Y[locI][j] += gama * (error * (sqrtNum * old_p) - lambda * Y[locI][j]);
                        sumY[i][j] += Y[locI][j] - old_y;
                    }
                }else{
                    Y[locationIndex][j] += gama * (error * (sqrtNum * old_p) - lambda * old_y);
                    sumY[i][j] += Y[locationIndex][j] - old_y;
                }
                
             }
        }
    }
}



void init(){
    // 分开train 和 cross
    //输入数据， 计算几个平均值
    
    // ifstream fdata("data.txt");
    // ofstream ftrain("train.txt");
    // ofstream fcross("cross.txt");

    // string s;
    // srand(time(NULL));
    // while (getline(fdata, s)) {
    //     if (rand() % 100 < 15) {
    //         fcross << s << endl;
    //     } else {
    //         ftrain << s << endl;
    //     }
    // }

    // fdata.close();
    // ftrain.close();
    // fcross.close();

    FILE *ft = fopen("train.txt", "r");
    FILE *fc = fopen("cross.txt", "r");

    int loc, time, count(0), tp;
    double intensity, sum(0);
    vector<int> locationCnt(locationSlots, 0);
    vector<int> timeCnt(timeSlots, 0);
    train_data.resize(typeNums);

    while((fscanf(ft, "%d %d %lf %d", &loc, &time, &intensity, &tp)) != EOF){
        count++;
        sum += intensity;
        locationCnt[loc]++;
        locationSlotsBias[loc] += intensity;
        timeCnt[time]++;
        timeSlotsBias[time] += intensity;

        Data tmp(loc, time, intensity, tp);
        train_data[tp].push_back(tmp);
    }

    while((fscanf(fc, "%d %d %lf %d", &loc, &time, &intensity, &tp)) != EOF){
        Data tmp(loc, time, intensity, tp);
        cross_data.push_back(tmp);
    }

     globalAverage = sum/count;
     //cout << "globalAverage = " << globalAverage << endl;

    for(size_t i = 0; i < timeSlots; i++){
        if(timeCnt[i] != 0) timeSlotsBias[i] /= timeCnt[i];
        else timeSlotsBias[i] = globalAverage;

        for(size_t j = 0; j < factors; j++){
            Q[i][j] =  20 * (rand() / (RAND_MAX + 1.0)) / sqrt(factors);
        }
    }

    for(size_t i = 0; i < locationSlots; i++){
        if(locationCnt[i] != 0) locationSlotsBias[i] /= locationCnt[i];
        else locationSlotsBias[i] = globalAverage;
        for(size_t j = 0; j < factors; j++){
            P[i][j] = 20 * (rand() / (RAND_MAX + 1.0)) / sqrt(factors);
            Y[i][j] = 0;
        }
    }
    for(size_t i = 0; i < typeNums; i++){
        for(size_t j = 0; j < factors; j++){
            sumY[i][j] = 5 * (rand() / (RAND_MAX + 1.0)) / sqrt(factors);
        }
    }

    fclose(ft);
    fclose(fc);
}

int main()
{
    init();
     double curRmse, preRmse = 100;
     for (int i = 0; i < 200; ++i) {
        train();
        curRmse = getRmse();
        cout << "Rmse in the step " << i << " is: " << curRmse << endl;
        preRmse = curRmse;
    }
    // for(int i = 0; i < locationSlots; i++){
    //     cout << locationSlotsBias[i] << endl;
    // }
    return 0;
}



