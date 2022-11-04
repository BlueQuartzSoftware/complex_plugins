/*
 * Your License or Copyright Information can go here
 */

#pragma once

#include <iostream>
#include <memory>
#include <string>

/**
 * @brief The DistanceTemplate class contains a templated function getDistance to find the distance, via a variety of
 * metrics, between two vectors of arbitrary dimensions.  The developer should ensure that the pointers passed to
 * getDistance do indeed contain vectors of the same component dimensions and start at the desired tuples.
 */
namespace DistanceTemplate
{
const int32_t k_EuclideanIndex = 0;
const int32_t k_SquaredEuclideanIndex = 1;
const int32_t k_ManhattanIndex = 2;
const int32_t k_CosineIndex = 3;
const int32_t k_PearsonIndex = 4;
const int32_t k_SquaredPearsonIndex = 5;

/**
 * @brief
 * @return
 */
std::vector<std::string> GetDistanceMetricsOptions()
{
  return {"Euclidean", "Squared Euclidean", "Manhattan", "Cosine", "Pearson", "Squared Pearson"};
}

/**
 * @brief
 * @tparam leftDataType
 * @tparam rightDataType
 * @tparam outDataType
 * @param leftVector
 * @param rightVector
 * @param compDims
 * @param distMetric
 * @return
 */
template <typename leftDataType, typename rightDataType, typename outDataType>
outDataType GetDistance(leftDataType* leftVector, rightDataType* rightVector, size_t compDims, int distMetric)
{
  double dist = 0.0;
  double lVal = 0.0;
  double rVal = 0.0;

  double epsilon = std::numeric_limits<double>::min();

  // Euclidean
  if(distMetric == k_EuclideanIndex)
  {
    for(size_t i = 0; i < compDims; i++)
    {
      lVal = static_cast<double>(leftVector[i]);
      rVal = static_cast<double>(rightVector[i]);
      dist += (lVal - rVal) * (lVal - rVal);
    }

    dist = sqrt(dist);
  }
  // Squared Euclidean
  else if(distMetric == k_SquaredEuclideanIndex)
  {
    for(size_t i = 0; i < compDims; i++)
    {
      lVal = static_cast<double>(leftVector[i]);
      rVal = static_cast<double>(rightVector[i]);
      dist += (lVal - rVal) * (lVal - rVal);
    }
  }
  // Manhattan
  else if(distMetric == k_ManhattanIndex)
  {
    for(size_t i = 0; i < compDims; i++)
    {
      lVal = static_cast<double>(leftVector[i]);
      rVal = static_cast<double>(rightVector[i]);
      dist += fabs(lVal - rVal);
    }
  }
  // Cosine
  else if(distMetric == k_CosineIndex)
  {
    double r = 0;
    double x = 0;
    double y = 0;
    for(size_t i = 0; i < compDims; i++)
    {
      lVal = static_cast<double>(leftVector[i]);
      rVal = static_cast<double>(rightVector[i]);
      r += lVal * rVal;
      x += lVal * lVal;
      y += rVal * rVal;
    }
    dist = 1 - (r / (sqrt(x * y) + epsilon));
  }
  // Pearson
  else if(distMetric == k_PearsonIndex)
  {
    double r = 0;
    double x = 0;
    double y = 0;
    double xAvg = 0;
    double yAvg = 0;
    for(size_t i = 0; i < compDims; i++)
    {
      lVal = static_cast<double>(leftVector[i]);
      rVal = static_cast<double>(rightVector[i]);
      xAvg += lVal;
      yAvg += rVal;
    }
    xAvg /= static_cast<double>(compDims);
    yAvg /= static_cast<double>(compDims);
    for(size_t i = 0; i < compDims; i++)
    {
      lVal = static_cast<double>(leftVector[i]);
      rVal = static_cast<double>(rightVector[i]);
      r += (lVal - xAvg) * (rVal - yAvg);
      x += (lVal - xAvg) * (lVal - xAvg);
      y += (rVal - yAvg) * (rVal - yAvg);
    }
    dist = 1 - (r / (sqrt(x * y) + epsilon));
  }
  // Squared Pearson
  else if(distMetric == k_SquaredPearsonIndex)
  {
    double r = 0;
    double x = 0;
    double y = 0;
    double xAvg = 0;
    double yAvg = 0;
    for(size_t i = 0; i < compDims; i++)
    {
      lVal = static_cast<double>(leftVector[i]);
      rVal = static_cast<double>(rightVector[i]);
      xAvg += lVal;
      yAvg += rVal;
    }
    xAvg /= static_cast<double>(compDims);
    yAvg /= static_cast<double>(compDims);
    for(size_t i = 0; i < compDims; i++)
    {
      lVal = static_cast<double>(leftVector[i]);
      rVal = static_cast<double>(rightVector[i]);
      r += (lVal - xAvg) * (rVal - yAvg);
      x += (lVal - xAvg) * (lVal - xAvg);
      y += (rVal - yAvg) * (rVal - yAvg);
    }
    dist = 1 - ((r * r) / ((x * y) + epsilon));
  }

  // Return the correct primitive type for distance
  return static_cast<outDataType>(dist);
}

}; // namespace DistanceTemplate
