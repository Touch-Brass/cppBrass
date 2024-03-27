#include "global.h"

namespace global
{
    std::vector<double> linspace(double start, double finish, int N)
    {
        std::vector<double> res(N, 0);
        for (int i = 0; i < N; ++i)
        {
            res[i] = start + i * (finish - start) / static_cast<double>(N - 1);
        }
        return res;
    }

    double linspace(double start, double finish, int N, int idx)
    {
        if (idx >= N)
        {
            std::cout << "Idx is outside of range" << std::endl;
            return -1;
        }
        return start + idx * (finish - start) / static_cast<double>(N - 1);
    }

    double limit(double val, double min, double max)
    {
        if (val < min)
        {
            val = min;
            return val;
        }
        else if (val > max)
        {
            val = max;
            return val;
        }
        return val;
    }

    double outputClamp(double val)
    {
        return limit(val, -1.0, 1.0);
    }
}