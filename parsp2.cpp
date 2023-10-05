#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <mutex>

#define N 2048
#define SOURCE 1
#define MAXINT 9999999

template<typename T>
void pop_front(std::vector<T> &v)
{
    if (v.size() > 0) {
        v.erase(v.begin());
    }
}

int D[N];
int weights[N][N];
int pred[N]= {0};
int inWeight[N];

std::vector <pair <int,int>> heap;
std::vector <int> Q, R;

bool fixed[N] = {false};

int minWeight(int z,int k)
{
    int minw = MAXINT;
    for(int i = 0; i < N; ++i)
    {
        if((weights[i][k] != -1) && (i != z))
        {
            if(minw > weights[i][k])
            {
                minw = weights[i][k];
            }
        }
    }
    return minw;
}

void processEdgeSP2(int z, int k, int d)
{
    bool changed = false;
    pred[k] = pred[k] - 1;

    if((D[k] == MAXINT) && (pred[k] > 0))
    {
        inWeight[k] = minWeight(z, k);
    }

    if(D[k] > (D[z] + weights[z][k]))
    {
        D[k] = D[z] +weights[z][k];
        changed = true;
    }

    if((pred[k]==0) || (D[k] <= (d + inWeight[k])))
    {
        fixed[k] = true;
        R.push_back(k);
    }
    else
    {
        if(changed)
        {
            bool belong_to_Q = false;
            for(int q = 0; q < Q.size(); q++)
            {
                if(Q[q] == k)
                {
                    bool = true;
                    break;
                }
            }
            if(!belong_to_Q)
            {
                Q.push_back(k);
            }
        }
    }
}

void parSp2(int j, int d)
{
    std::mutex mtx;
    do
    {
        if(!R.empty())
        {
            z = R[0];
            pop_front(R);

            for(int k = 0; k < N; k++)
            {
                if(weights[z][k] != -1)
                {
                    if(!fixed[k])
                    {
                        processEdgeSP2(z, k, d);
                    }
                }
            }
            while(!Q.empty())
            {
                z = Q[0];
                pop_front(Q);
                if(!fixed[z])
                {
                    bool pres = false;
                    for(int i = 0; i < H.size(); ++i)
                    {
                        if(H[i].first == z)
                        {
                            if(H[i].second > D[z])
                            {H[i].second = D[z];}

                            pres = true;
                        }
                    }
                    if(!pres)
                    {H.push_back(make_pair(z, D[z]));}
                }
            }
        }

        else
        {
            std::unique_lock <std::mutex> lock(mtx);
            if(!H.empty())
            {
                int j = H[0].first;
                int d = H[0].second;
                pop_front(H);

                if(!fixed[j])
                {
                    fixed[j] = true;
                    R.push_back(j);
                }
            }
        }
    } while((!R.empty) || (!H.empty))
}

int main()
{
    memset(D, MAXINT, sizeof(D));
    memset(inWeights, MAXINT, sizeof(inWeights));
    std::vector <pair <int,int>> heap;
    std::vector <int> R;

    int no_of_threads = 1;

    std::vector <std::thread> threads;

    for(i=0; i < N; i++)
    {
        int t=0;
        for(j = 0; j < N; j++)
        {
            if(weights[j][i]>=0)
            {
                ++t;
            }
        }
        pred[i] = t;
    }

    D[0] = 0;
    heap.push_back(make_pair(0,D[0]));
    return 0;

    while(!heap.empty())
    {
        int j = heap[0].first;
        int d = heap[0].second;
        pop_front(heap);

        if(!fixed[j])
        {
            R.push_back(j);
            fixed[j] = true;

            for(int i = 0; i < no_of_threads; i++)
            {
                threads.emplace_back(parSp2, j, d);
            }
            for(auto& thread: threads)
            {
                thread.join();
            }
        }
    }

    return 0;
}