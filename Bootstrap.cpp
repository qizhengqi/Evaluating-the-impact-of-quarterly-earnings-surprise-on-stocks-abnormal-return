//
//  Bootstrap.cpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/11.
//

#include "Bootstrap.hpp"
#include "stdio.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <iterator>
#include <cmath>
#include <chrono> // std::chrono::system_clock


// divide_group beat/meet/miss, create group vector [[],[],[]]
Group DivideGroup(map<string, Stock*> &stocks)
{
    Group grouplist(3);
    map<string, Stock*>::iterator itr;
    for (itr = stocks.begin(); itr != stocks.end(); itr++)
    {
        if (itr->second->GetGroup() == 1)
            grouplist[0].push_back(itr->second->GetTicker());
        else if (itr->second->GetGroup() == 2)
            grouplist[1].push_back(itr->second->GetTicker());
        else
            grouplist[2].push_back(itr->second->GetTicker());
    }
    return grouplist;
}



// Operators of Vectors and Matrix
// V, W are vectors of floating numbers with type vector<double>.
// M, N are matrices of floating numbers with type vector<vector<double>>.

// 1. Addition of 2 vectors
Vector operator+(Vector& V, Vector& W)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++)
    {
        U[j] = V[j] + W[j];
    }
    return U;
}

// 2. Addition between matrix and vector
Vector operator+(vector<vector<double>>& M, Vector& V)
{
    int c = (int)M.size();
    int d = (int)V.size();
    Vector W(d);
    for (int j = 0; j < d; j++)
    {
        for (int i = 0; i < c; i++)
        {
            W[j] = M[i][j] + V[j];
        }
    }
    return W;
}

// 3. Subtraction between vector and vector
Vector operator-(Vector& V, Vector& W)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++)
    {
        U[j] = V[j] - W[j];
    }
    return U;
}

// 4. Subtraction between matrix and vector
Matrix operator-(Matrix& M, Vector& V)
{
    int c = (int)M.size();
    int d = (int)V.size();
    Matrix W(c, Vector(d, 0.0));
    for (int j = 0; j < c; j++)
    {
        for (int i = 0; i < d; i++)
        {
            W[j][i] = M[j][i] - V[i];
        }
    }
    return W;
}

// 5. Subtraction between vector and number
Vector operator-(Vector& V, double num)
{
    int d = (int)V.size();
    Vector W(d);
    for (int j = 0; j < d; j++)
        W[j] = V[j] - num;
    return W;
}

// 6. Division between vector and number
Vector operator/(Vector& V, int num)
{
    int d = (int)V.size();
    Vector W(d);
    for (int j = 0; j < d; j++)
        W[j] = V[j] / num;
    return W;
}

// 7. SumSquare of an matrix
Vector SumSquare(Matrix& M)
{
    int d = (int)M[0].size();
    Vector V(d, 0.0);
    for (int i = 0; i < (int)M.size(); i++)
    {
        for (int j = 0; j < d; j++)
        {
            V[j] = V[j] + pow(M[i][j], 2);
        }
    }
    return V;
}

// 8. Square root of a vector
Vector SquareRoot(Vector& V)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++)
    {
        U[j] = sqrt(V[j]);
    }
    return U;
}


//---------------------------------------------------------------------------------------------------------

Group Bootstrap::RandomSelect(Group grouplist)
{
    Group randomlist(3);
    for (int i = 0; i < 3; i++)
    {
        //random_shuffle(grouplist[i].begin(), grouplist[i].end());
        unsigned seed = (int)std::chrono::system_clock::now ().time_since_epoch ().count ();
        shuffle(grouplist[i].begin(), grouplist[i].end(), std::default_random_engine (seed));
        for (int j = 0; j < SampleNum; j++)
        {
            randomlist[i].push_back(grouplist[i][j]);
        }
    }
    return randomlist;
}

void Bootstrap::SetN(int N_)
{
    N = N_;
}

// retrieve abnormal data, vector of 3 matrix, each is 80 * 2N
vector<AbnormalReturns> Bootstrap::RetrieveRandomList(map<string, Stock*>& stocks, const Group& randomlist)
{
    vector<AbnormalReturns> ARmap(3);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < SampleNum; j++)
        {
            Vector abnormalreturns = stocks[randomlist[i][j]]->GetAR();
            int day0index = stocks[randomlist[i][j]]->GetDay0Index() - 1;
            std::vector<double> ab_slice(abnormalreturns.begin() + day0index - N + 1, abnormalreturns.begin() + day0index + N + 1);
            ARmap[i].insert({ randomlist[i][j], ab_slice });
        }
    }
    return ARmap;
}


Vector Bootstrap::CalculateDailyAAR(AbnormalReturns& AbnormalReturn)
{
    vector<double> DailyAAR(2 * N, 0.0);
    for (int t = 0; t < 2 * N; t++)
    {
        for (map<string, vector<double>> ::iterator itr = AbnormalReturn.begin(); itr != AbnormalReturn.end(); itr++)
        {
            DailyAAR[t] += itr->second[t];
        }
        DailyAAR[t] = DailyAAR[t] / SampleNum;
    }
    return DailyAAR;
}

Vector Bootstrap::CalculateDailyCAAR(vector<double>& DailyAAR)
{
    vector <double> DailyCAAR(2 * N, 0.0);
    DailyCAAR[0] = DailyAAR[0];
    for (int t = 0; t < 2 * N - 1; t++)
    {
        DailyCAAR[t + 1] = DailyCAAR[t] + DailyAAR[t + 1];
    }
    return DailyCAAR;
}



//--------------------------------
vector<Matrix> Bootstrap::finalCalculation(map<string, Stock*>& stocks, Group& grouplist)
{
    Matrix sumAAR(3, Vector(2 * N, 0.0)); // store the summation of AAR
    Matrix sumCAAR(3, Vector(2 * N, 0.0)); // store the summation of CAAR
    vector<Matrix> wholeAAR(3); // store whole 40 times of AAR
    vector<Matrix> wholeCAAR(3); // store whole 40 times of CAAR

    // Initialization
    for (int i = 0; i < 3; i++)
    {
        for (int n = 0; n < RepeatNum; n++)
        {
            wholeAAR[i].push_back(Vector(2 * N, 0.0));
            wholeCAAR[i].push_back(Vector(2 * N, 0.0));
        }
    }

    // repeat for 40 times
    for (int n = 0; n < RepeatNum; n++)
    {
        Group randomlist = RandomSelect(grouplist);

        vector<AbnormalReturns> ARmap(3);
        ARmap = RetrieveRandomList(stocks, randomlist);

        for (int i = 0; i < 3; i++)
        {
            Vector AAR = CalculateDailyAAR(ARmap[i]);
            sumAAR[i] = sumAAR[i] + AAR;
            Vector CAAR = CalculateDailyCAAR(AAR);
            sumCAAR[i] = sumCAAR[i] + CAAR;
            wholeAAR[i][n] = AAR;
            wholeCAAR[i][n] = CAAR;
        }
    }

    //after 40 times simulation, we calculate Ave and S.D.
    Matrix AveAAR(3, Vector(2 * N, 0.0));
    Matrix AveCAAR(3, Vector(2 * N, 0.0));
    Matrix SDAAR(3, Vector(2 * N, 0.0));
    Matrix SDCAAR(3, Vector(2 * N, 0.0));

    for (int i = 0; i < 3; i++)
    {
        AveAAR[i] = sumAAR[i] / RepeatNum;
        AveCAAR[i] = sumCAAR[i] / RepeatNum;
    }

    for (int i = 0; i < 3; i++)
    {
        Matrix Res = wholeAAR[i] - AveAAR[i];
        Vector ResSquareSum = SumSquare(Res);
        Vector ResSquareAve = ResSquareSum / (RepeatNum - 1);
        SDAAR[i] = SquareRoot(ResSquareAve);

        Res = wholeCAAR[i] - AveCAAR[i];
        ResSquareSum = SumSquare(Res);
        ResSquareAve = ResSquareSum / (RepeatNum - 1);
        SDCAAR[i] = SquareRoot(ResSquareAve);
    }

    //build final result matrix
    vector<Matrix> FinalResult(3, Matrix(4, Vector(2 * N, 0.0)));
    //Put Beat's average AAR, standard deviation of AAR, average CAAR, and standard deviation of CAAR in final result matrix
    for (int i = 0; i < 3; i++)
    {
        FinalResult[i][0] = AveAAR[i];
        FinalResult[i][1] = SDAAR[i];
        FinalResult[i][2] = AveCAAR[i];
        FinalResult[i][3] = SDCAAR[i];
    }

    return FinalResult;
}
