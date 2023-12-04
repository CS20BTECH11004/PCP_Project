#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <limits.h>
#include <cmath>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>

namespace DeltaStepping
{
    std::mutex mutx;

    typedef std::chrono::milliseconds MSEC;
    typedef std::chrono::microseconds MCROSEC;
    typedef std::chrono::nanoseconds NSEC;
    typedef std::chrono::high_resolution_clock HRC;

    void relax(int v, int x, std::vector<int> &d, std::vector<std::set<int>> &B, float delta)
    {
        mutx.lock();
        if (x < d[v])
        {
            if (d[v] != INT_MAX)
                B[floor(d[v] / delta)].erase(v);
            B[floor(x / delta)].insert(v);
            d[v] = x;
        }
        mutx.unlock();
    }

    bool isEmpty(std::vector<std::set<int>> &B)
    {
        for (std::set<int> s : B)
        {
            if (!s.empty())
                return false;
        }
        return true;
    }

    std::vector<int> run(std::vector<std::vector<int>> &l, float delta = 3.0, int s = 0)
    {
        int n = l.size();
        std::vector<std::set<std::pair<int, int>>> heavy(n), light(n);
        std::vector<std::set<int>> B(n);
        std::vector<int> d(n, INT_MAX);
        d[s] = 0;

        for (int v = 0; v < n; v++)
        {
            for (int w = 0; w < n; w++)
            {
                if (w == v || l[v][w] == 0)
                    continue;

                std::pair<int, int> vw = {v, w};
                if (l[v][w] > delta)
                    heavy[v].insert(vw);
                else
                    light[v].insert(vw);
            }
        }
        B[0].insert(s);
        int i = 0;

        while (!isEmpty(B))
        {
            std::set<int> S;
            std::set<std::pair<int, int>> Req;
            while (!B[i].empty())
            {
                for (int v : B[i])
                {
                    for (std::pair<int, int> vw : light[v])
                    {
                        Req.insert(std::pair<int, int>{vw.second, d[v] + l[v][vw.second]});
                    }
                    S.insert(v);
                }
                B[i] = std::set<int>{};

                std::vector<std::thread> threads(Req.size());
                int k = 0;

                for (std::pair<int, int> vx : Req)
                {
                    threads[k++] = std::thread(&relax, vx.first, vx.second, ref(d), ref(B), delta);
                }

                for (auto &t : threads)
                    t.join();
            }

            for (int v : S)
            {
                for (std::pair<int, int> vw : heavy[v])
                {
                    Req.insert(std::pair<int, int>{vw.second, d[v] + l[v][vw.second]});
                }
            }

            for (std::pair<int, int> vx : Req)
            {
                relax(vx.first, vx.second, d, B, delta);
            }

            i++;
        }

        std::vector<int> sh(n, 0);
        for (int v = 0; v < n; v++)
        {
            sh[v] = d[v];
        }

        return sh;
    }

};
