//
// Created by trotfunky on 24/05/18.
//

#include "PointToPointTrajectory.h"

PointToPointTrajectory::PointToPointTrajectory()
{
    counter = 0;

    xTable = nullptr;
    yTable = nullptr;
    size = 0;
}

PointToPointTrajectory::PointToPointTrajectory(const double* xTable, const double* yTable, const int size)
{
    counter = 0;

    this->xTable = xTable;
    this->yTable = yTable;
    this->size = size;
}

std::pair<double,double> PointToPointTrajectory::query()
{
    std::pair<double,double> temp = std::make_pair(*(xTable+counter),*(yTable+counter));
    if(counter < size)
    {
        counter ++;
    }
    else
    {
        return(std::make_pair(0,0));
    }
    return(temp);
}

PointToPointTrajectory::operator bool()
{
    return(counter < size);
}