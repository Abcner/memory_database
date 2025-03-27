/**
 * @file test_project_option.cpp
 * @author Han Ruichen (hanruichen@ruc.edu.cn)
 * @brief test of projection algorithms
 *
 * @version 0.1
 * @date 2023-04-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "../include/metadata.h"
#include "../include/gendata_util.hpp"
#include "../include/statistical_analysis_util.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cmath>
row_store_min row_min[67108864];
row_store_max row_max[67108864];

/**
 * @brief projection calculation
 *        calculate a row per iteration
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param result
 * @return int the result count.
 */
int proalgo_rowwise(int condition, const idx &size_R,
                    row_store_min *row_min,
                    std::vector<std::pair<int, int>> &result)
{
  int count = 0;
  idx i;
  idx result_size = size_R;
  for (i = 0; i != result_size; ++i)
  {
    if (row_min[i].Ra <= condition && row_min[i].Rc <= condition)
    {
      count++;
      result.emplace_back(row_min[i].Ra, row_min[i].Rc);
    }
  }
  return count;
}
/**
 * @brief projection calculation
 *        calculate a row per iteration
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param result
 * @return int the result count.
 */
int proalgo_rowwise(int condition, const idx &size_R,
                    row_store_min *row_min,
                    struct fixed_arrays &result)
{
  int count = 0;
  idx i;
  idx result_size = size_R;
  for (i = 0; i != result_size; ++i)
  {
    if (row_min[i].Ra <= condition && row_min[i].Rc <= condition)
    {

      result.pos_value1[count] = row_min[i].Ra; // value 1
      result.value2[count] = row_min[i].Rc;
      count++;
    }
  }
  return count;
}
/**
 * @brief projection calculation
 *        calculate a row per iteration
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param result
 * @return int the result count.
 */
int proalgo_rowwise(int condition, const idx &size_R,
                    row_store_max *row_max,
                    std::vector<std::pair<int, int>> &result)
{
  int count = 0;
  idx i;
  idx result_size = size_R;
  for (i = 0; i != result_size; ++i)
  {
    if (row_max[i].Ra <= condition && row_max[i].Rc <= condition)
    {
      count++;
      result.emplace_back(row_max[i].Ra, row_max[i].Rc);
    }
  }
  return count;
}
/**
 * @brief projection calculation
 *        calculate a row per iteration
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param result
 * @return int the result count.
 */
int proalgo_rowwise(int condition, const idx &size_R,
                    row_store_max *row_max,
                    struct fixed_arrays &result)
{
  int count = 0;
  idx i;
  idx result_size = size_R;
  for (i = 0; i != result_size; ++i)
  {
    if (row_max[i].Ra <= condition && row_max[i].Rc <= condition)
    {
      result.pos_value1[count] = row_max[i].Ra; // value 1
      result.value2[count] = row_max[i].Rc;
      count++;
    }
  }
  return count;
}
/**
 * @brief projection calculation
 *        calculate one column in one run with early materialization strategy and dynamic vector result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_em(int condition, const idx &size_R,
                   const int *Ra,
                   const int *Rc,
                   std::vector<std::pair<int, int>> &result,
                   int pre_size)
{
  idx read_idx, cur_size, write_idx;
  for (read_idx = 0, write_idx = pre_size; read_idx != size_R; ++read_idx)
  {
    if (Ra[read_idx] <= condition)
    {
      result[write_idx].first = read_idx;      // pos,
      result[write_idx].second = Ra[read_idx]; // value1
      ++write_idx;
    }
  }
  cur_size = write_idx - pre_size;
  for (read_idx = 0, write_idx = pre_size; read_idx != cur_size; ++read_idx)
  {
    auto cur_pos = result[read_idx + pre_size].first;
    if (Rc[cur_pos] <= condition)
    {
      result[write_idx].first = result[cur_pos + pre_size].second; // value 1
      result[write_idx].second = Rc[cur_pos];                      // value 2
      ++write_idx;
    }
  }

  return write_idx;
}
/**
 * @brief projection calculation
 *        calculate one column in one run with early materialization strategy and fixed vector result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_em(int condition, const idx &size_R,
                   const int *Ra,
                   const int *Rc,
                   struct fixed_arrays &result,
                   int pre_size)
{

  idx read_idx, cur_size, write_idx;
  for (read_idx = 0, write_idx = pre_size; read_idx != size_R; ++read_idx)
  {
    if (Ra[read_idx] <= condition)
    {
      result.pos_value1[write_idx] = read_idx; // (pos, value1)
      result.value2[write_idx] = Ra[read_idx];
      // result.emplace_back(read_idx, Ra[read_idx]);
      ++write_idx;
    }
  }
  cur_size = write_idx - pre_size;
  for (read_idx = 0, write_idx = pre_size; read_idx != cur_size; ++read_idx)
  {
    auto cur_pos = result.pos_value1[read_idx + pre_size];
    if (Rc[cur_pos] <= condition)
    {
      result.pos_value1[write_idx] = result.value2[cur_pos + pre_size]; // value 1
      result.value2[write_idx] = Rc[cur_pos];                           // value 2
      ++write_idx;
    }
  }

  return write_idx;
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and independent dynamic vector intermediate results as well as dynamic vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param pos1
 * @param pos2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_idv(int condition, const idx &size_R,
                       const int *Ra,
                       const int *Rc,
                       std::vector<int> &pos1, std::vector<int> &pos2,
                       std::vector<std::pair<int, int>> &result)
{

  idx i, j;
  for (i = 0; i < size_R; ++i)
  {
    if (Ra[i] <= condition)
    {
      pos1.emplace_back(i);
    }
  }

  for (i = 0; i < size_R; ++i)
  {
    if (Rc[i] <= condition)
    {
      pos2.emplace_back(i);
    }
  }

  idx merge_idx = 0;
  for (i = 0, j = 0; i < pos1.size() && j < pos2.size();)
  {
    if (pos1[i] == pos2[j])
    {
      pos1[merge_idx] = pos1[i];
      ++i;
      ++j;
      ++merge_idx;
    }
    else if (pos1[i] > pos2[j])
    {
      ++j;
    }
    else
    {
      // if pos1[i] < pos2[j]
      ++i;
    }
  }

  for (i = 0; i != merge_idx; ++i)
  {
    auto cur_pos = pos1[i];
    result.emplace_back(Ra[cur_pos], Rc[cur_pos]);
  }

  return result.size();
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and independent dynamic vector intermediate results as well as dynamic vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param pos1
 * @param pos2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_idv(int condition, const idx &size_R,
                       const int *Ra,
                       const int *Rc,
                       std::vector<int> &pos1, std::vector<int> &pos2,
                       struct fixed_arrays &result,
                       int pre_size)
{
  idx i, j;
  for (i = 0; i < size_R; ++i)
  {
    if (Ra[i] <= condition)
    {
      pos1.emplace_back(i);
    }
  }

  for (i = 0; i < size_R; ++i)
  {
    if (Rc[i] <= condition)
    {
      pos2.emplace_back(i);
    }
  }

  idx merge_idx = 0;
  for (i = 0, j = 0; i < pos1.size() && j < pos2.size();)
  {
    if (pos1[i] == pos2[j])
    {
      pos1[merge_idx] = pos1[i];
      ++i;
      ++j;
      ++merge_idx;
    }
    else if (pos1[i] > pos2[j])
    {
      ++j;
    }
    else
    {
      // if pos1[i] < pos2[j]
      ++i;
    }
  }

  for (i = 0; i != merge_idx; ++i)
  {
    auto cur_pos = pos1[i];
    int cur_idx = pre_size + i;
    result.pos_value1[cur_idx] = Ra[cur_pos]; // value 1
    result.value2[cur_idx] = Rc[cur_pos];
  }

  return merge_idx;
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and shared dynamic vector intermediate results as well as dynamic vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param pos1
 * @param pos2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_sdv(int condition, const idx &size_R,
                       const int *Ra,
                       const int *Rc,
                       std::vector<int> &pos,
                       std::vector<std::pair<int, int>> &result)
{
  idx i, cur_size = 0;
  for (i = 0; i < size_R; ++i)
  {
    if (Ra[i] <= condition)
    {
      pos.emplace_back(i);
    }
  }

  for (i = 0; i < pos.size(); ++i)
  {
    if (Rc[pos[i]] <= condition)
    {
      pos[cur_size++] = pos[i];
    }
  }

  for (i = 0; i != cur_size; ++i)
  {
    auto cur_pos = pos[i];
    result.emplace_back(Ra[cur_pos], Rc[cur_pos]);
  }

  return result.size();
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and shared dynamic vector intermediate results as well as fixed vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param pos1
 * @param pos2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_sdv(int condition, const idx &size_R,
                       const int *Ra,
                       const int *Rc,
                       std::vector<int> &pos,
                       struct fixed_arrays &result,
                       int pre_size)
{
  idx i, cur_size = 0;
  for (i = 0; i < size_R; ++i)
  {
    if (Ra[i] <= condition)
    {
      pos.emplace_back(i);
    }
  }

  for (i = 0; i < pos.size(); ++i)
  {
    if (Rc[pos[i]] <= condition)
    {
      pos[cur_size++] = pos[i];
    }
  }

  for (i = 0; i != cur_size; ++i)
  {
    auto cur_pos = pos[i];
    int cur_idx = i + pre_size;
    result.pos_value1[i] = Ra[cur_pos]; // value 1
    result.value2[i] = Rc[cur_pos];
  }

  return cur_size;
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and independent fixed vector intermediate results as well as dynamic vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param pos1
 * @param pos2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_ifv(int condition, const idx &size_R,
                       const int *Ra,
                       const int *Rc,
                       int *pos1, int *pos2,
                       std::vector<std::pair<int, int>> &result)
{
  idx pos1_idx, pos2_idx, i, j;
  for (i = 0, pos1_idx = 0; i < size_R; ++i)
  {
    if (Ra[i] <= condition)
    {
      pos1[pos1_idx] = i;
      ++pos1_idx;
    }
  }

  for (i = 0, pos2_idx = 0; i < size_R; ++i)
  {
    if (Rc[i] <= condition)
    {
      pos2[pos2_idx] = i;
      ++pos2_idx;
    }
  }

  idx merge_idx = 0;
  for (i = 0, j = 0; i < pos1_idx && j < pos2_idx;)
  {
    if (pos1[i] == pos2[j])
    {
      pos1[merge_idx] = pos1[i];
      ++i;
      ++j;
      ++merge_idx;
    }
    else if (pos1[i] > pos2[j])
    {
      ++j;
    }
    else
    {
      // if pos1[i] < pos2[j]
      ++i;
    }
  }

  for (i = 0; i != merge_idx; ++i)
  {
    auto cur_pos = pos1[i];
    result.emplace_back(Ra[cur_pos], Rc[cur_pos]);
  }

  return result.size();
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and independent fixed vector intermediate results as well as fixed vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param pos1
 * @param pos2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_ifv(int condition, const idx &size_R,
                       const int *Ra,
                       const int *Rc,
                       int *pos1, int *pos2,
                       struct fixed_arrays &result,
                       int pre_size)
{
  idx pos1_idx, pos2_idx, i, j;
  for (i = 0, pos1_idx = 0; i < size_R; ++i)
  {
    if (Ra[i] <= condition)
    {
      pos1[pos1_idx] = i;
      ++pos1_idx;
    }
  }

  for (i = 0, pos2_idx = 0; i < size_R; ++i)
  {
    if (Rc[i] <= condition)
    {
      pos2[pos2_idx] = i;
      ++pos2_idx;
    }
  }

  idx merge_idx = 0;
  for (i = 0, j = 0; i < pos1_idx && j < pos2_idx;)
  {
    if (pos1[i] == pos2[j])
    {
      pos1[merge_idx] = pos1[i];
      ++i;
      ++j;
      ++merge_idx;
    }
    else if (pos1[i] > pos2[j])
    {
      ++j;
    }
    else
    {
      // if pos1[i] < pos2[j]
      ++i;
    }
  }

  for (i = 0; i != merge_idx; ++i)
  {
    auto cur_pos = pos1[i];
    int cur_idx = i + pre_size;
    result.pos_value1[cur_idx] = Ra[cur_pos]; // value 1
    result.value2[cur_idx] = Rc[cur_pos];
  }

  return merge_idx;
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and shared fixed vector intermediate results as well as dynamic vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param pos1
 * @param pos2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_sfv(int condition, const idx &size_R,
                       const int *Ra,
                       const int *Rc,
                       int *pos,
                       std::vector<std::pair<int, int>> &result)
{
  idx pos1_idx, pos2_idx, i, j;
  for (i = 0, pos1_idx = 0; i < size_R; ++i)
  {
    if (Ra[i] <= condition)
    {
      pos[pos1_idx] = i;
      ++pos1_idx;
    }
  }

  for (i = 0, pos2_idx = 0; i < pos1_idx; ++i)
  {
    if (Rc[pos[i]] <= condition)
    {
      pos[pos2_idx] = pos[i];
      ++pos2_idx;
    }
  }

  for (i = 0; i != pos2_idx; ++i)
  {
    auto cur_pos = pos[i];
    result.emplace_back(Ra[cur_pos], Rc[cur_pos]);
  }

  return result.size();
}

/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and shared fixed vector intermediate results as well as fixed vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param pos1
 * @param pos2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_sfv(int condition, const idx &size_R,
                       const int *Ra,
                       const int *Rc,
                       int *pos,
                       struct fixed_arrays &result,
                       int pre_size)
{
  idx pos1_idx, pos2_idx, i, j;
  for (i = 0, pos1_idx = 0; i < size_R; ++i)
  {
    if (Ra[i] <= condition)
    {
      pos[pos1_idx] = i;
      ++pos1_idx;
    }
  }

  for (i = 0, pos2_idx = 0; i < pos1_idx; ++i)
  {
    if (Rc[pos[i]] <= condition)
    {
      pos[pos2_idx] = pos[i];
      ++pos2_idx;
    }
  }

  for (i = 0; i != pos2_idx; ++i)
  {
    auto cur_pos = pos[i];
    int cur_idx = i + pre_size;
    result.pos_value1[cur_idx] = Ra[cur_pos]; // value 1
    result.value2[cur_idx] = Rc[cur_pos];
  }

  return pos2_idx;
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and independent bitmap intermediate results as well as dynamic vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param bitmap1
 * @param bitmap2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_ibmp(int condition, const idx &size_R,
                        const int *Ra,
                        const int *Rc,
                        bool *bitmap1, bool *bitmap2,
                        std::vector<std::pair<int, int>> &result)
{
  idx i;
  for (i = 0; i != size_R; ++i)
  {
    bitmap1[i] = (Ra[i] <= condition);
  }

  for (i = 0; i != size_R; ++i)
  {
    bitmap2[i] = (Rc[i] <= condition);
  }

  for (i = 0; i != size_R; ++i)
  {
    bitmap1[i] = (bitmap1[i] & bitmap2[i]);
  }

  for (i = 0; i != size_R; ++i)
  {
    if (bitmap1[i])
    {
      result.emplace_back(Ra[i], Rc[i]);
    }
  }

  return result.size();
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and independent bitmap intermediate results as well as fixed vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param bitmap1
 * @param bitmap2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_ibmp(int condition, const idx &size_R,
                        const int *Ra,
                        const int *Rc,
                        bool *bitmap1, bool *bitmap2,
                        struct fixed_arrays &result,
                        int pre_size)
{
  idx i, cur_size = pre_size;
  for (i = 0; i != size_R; ++i)
  {
    bitmap1[i] = (Ra[i] <= condition);
  }

  for (i = 0; i != size_R; ++i)
  {
    bitmap2[i] = (Rc[i] <= condition);
  }

  for (i = 0; i != size_R; ++i)
  {
    bitmap1[i] = (bitmap1[i] & bitmap2[i]);
  }

  for (i = 0; i != size_R; ++i)
  {
    if (bitmap1[i])
    {
      result.pos_value1[cur_size] = Ra[i]; // value 1
      result.value2[cur_size] = Rc[i];
      cur_size++;
    }
  }

  return cur_size;
}
/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and shared bitmap intermediate results as well as dynamic vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param bitmap1
 * @param bitmap2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_sbmp(int condition, const idx &size_R,
                        const int *Ra,
                        const int *Rc,
                        bool *bitmap,
                        std::vector<std::pair<int, int>> &result)
{
  idx i;
  for (i = 0; i != size_R; ++i)
  {
    bitmap[i] = (Ra[i] <= condition);
  }

  for (i = 0; i != size_R; ++i)
  {
    bitmap[i] = (bitmap[i]) && (Rc[i] <= condition);
  }

  for (i = 0; i != size_R; ++i)
  {
    if (bitmap[i])
    {
      result.emplace_back(Ra[i], Rc[i]);
    }
  }

  return result.size();
}

/**
 * @brief projection calculation
 *        calculate one column in one run with late materialization strategy and shared bitmap intermediate results as well as fixed vector final result
 *
 * @param condition
 * @param size_R
 * @param Ra
 * @param Rc
 * @param bitmap1
 * @param bitmap2
 * @param result
 * @return int the result count.
 */
int proalgo_cwm_lm_sbmp(int condition, const idx &size_R,
                        const int *Ra,
                        const int *Rc,
                        bool *bitmap,
                        struct fixed_arrays &result,
                        int pre_size)
{
  idx i, cur_size = pre_size;
  for (i = 0; i != size_R; ++i)
  {
    bitmap[i] = (Ra[i] <= condition);
  }

  for (i = 0; i != size_R; ++i)
  {
    bitmap[i] = (bitmap[i]) && (Rc[i] <= condition);
  }

  for (i = 0; i != size_R; ++i)
  {
    if (bitmap[i])
    {
      result.pos_value1[cur_size] = Ra[i]; // value 1
      result.value2[cur_size] = Rc[i];
      cur_size++;
    }
  }

  return cur_size;
}
/**
 * @brief projection calculation by Column-wise query processing model with early materialization strategy
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_cwm_em(const idx &size_R,
                         const T *Ra, const T *Rc,
                         const std::vector<idx> &conditions,
                         std::ofstream &proalgo_timefile)
{
  /*dynamic vector result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with early materialization strategy and dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_em(conditions[select_idx], size_R, Ra, Rc, result, count);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "early materialization strategy"
                     << "\t"
                     << ""
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_cwm_em_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*fixed vector result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with early materialization strategy and fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_em(conditions[select_idx], size_R, Ra, Rc, result, count);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "early materialization strategy"
                     << "\t"
                     << ""
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_cwm_em_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Vector-wise query processing model with early materialization strategy
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_vwm_em(const idx &size_R,
                         const T *Ra, const T *Rc,
                         const std::vector<idx> &conditions,
                         const std::vector<idx> &conditions_lsr,
                         std::ofstream &proalgo_timefile,
                         std::ofstream &proalgo_lsr_timefile)
{
  /*dynamic vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with early materialization strategy and dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_em(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, result, count);
    }

    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "early materialization strategy"
                     << "\t"
                     << ""
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_vwm_em_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with early materialization strategy and fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_em(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, result, count);
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "early materialization strategy"
                     << "\t"
                     << ""
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_em_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with early materialization strategy and fixed vector final result in low selection rate test" << std::endl;

  for (idx select_idx = 0; select_idx != conditions_lsr.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions_lsr[select_idx] / 10 << " %, total selection rate " << pow((double)conditions_lsr[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_em(conditions_lsr[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, result, count);
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << (double)pow(conditions_lsr[select_idx] / 10, 2) / pow(100, 2) << "% is " << count << "/" << DATA_NUM << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_lsr_timefile << "Vector-wise query processing"
                     << "\t"
                     << "early materialization strategy"
                     << "\t"
                     << ""
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "Vector-wise query processing with early materialization strategy and fixed vector final result"
                     << "\t"
                     << log10(LSR_BASE * pow(LSR_STRIDE, select_idx))
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Column-wise query processing model with late materialization strategy and dynamic vector Intermediate results
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_cwm_lm_dv(const idx &size_R,
                            const T *Ra, const T *Rc,
                            const std::vector<idx> &conditions,
                            std::ofstream &proalgo_timefile)
{
  /*independent dynamic vector intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and independent dynamic vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    std::vector<int> pos1, pos2;
    pos1.reserve(size_R);
    pos2.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_idv(conditions[select_idx], size_R, Ra, Rc, pos1, pos2, result);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent dynamic vector intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_idvi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*independent dynamic vector intermediate results as well as fixed vector result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and independent dynamic vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];

    std::vector<int> pos1, pos2;
    pos1.reserve(size_R);
    pos2.reserve(size_R);

    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_idv(conditions[select_idx], size_R, Ra, Rc, pos1, pos2, result, count);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent dynamic vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_idvi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*shared dynamic vector intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and shared dynamic vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<int> pos;
    std::vector<std::pair<int, int>> result;
    pos.reserve(size_R);
    result.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_sdv(conditions[select_idx], size_R, Ra, Rc, pos, result);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared dynamic vector intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_sdvi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*shared dynamic vector intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and shared dynamic vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<int> pos;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    pos.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_sdv(conditions[select_idx], size_R, Ra, Rc, pos, result, count);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared dynamic vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_sdvi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Vector-wise query processing model with late materialization strategy and dynamic vector Intermediate results
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_vwm_lm_dv(const idx &size_R,
                            const T *Ra, const T *Rc,
                            const std::vector<idx> &conditions,
                            std::ofstream &proalgo_timefile)
{
  /*independent dynamic vector intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and independent dynamic vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    std::vector<int> pos1, pos2;
    pos1.reserve(size_v);
    pos2.reserve(size_v);
    idx vec_num = DATA_NUM / size_v;
    timeval start, end;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_idv(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos1, pos2, result);
      pos1.clear();
      pos2.clear();
    }

    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent dynamic vector intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_idvi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*independent dynamic vector intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and independent dynamic vector intermediate results and fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];

    std::vector<int> pos1, pos2;
    pos1.reserve(size_v);
    pos2.reserve(size_v);

    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count += proalgo_cwm_lm_idv(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos1, pos2, result, count);
      pos1.clear();
      pos2.clear();
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent dynamic vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_idvi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*shared dynamic vector intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and shared dynamic vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<int> pos;
    std::vector<std::pair<int, int>> result;
    pos.reserve(size_v);
    result.reserve(size_R);
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_sdv(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos, result);
      pos.clear();
    }

    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared dynamic vector intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_sdvi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*shared dynamic vector intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and shared dynamic vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<int> pos;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    pos.reserve(size_v);
    idx vec_num = DATA_NUM / size_v;
    timeval start, end;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count += proalgo_cwm_lm_sdv(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos, result, count);
      pos.clear();
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared dynamic vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_sdvi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Column-wise query processing model with late materialization strategy and fixed vector intermediate results as well as dynamic vector final result
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_cwm_lm_fv(const idx &size_R,
                            const T *Ra, const T *Rc,
                            const std::vector<idx> &conditions,
                            std::ofstream &proalgo_timefile)
{
  /*independent fixed vector intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and independent fixed vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos1 = new int[size_R];
    int *pos2 = new int[size_R];
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_ifv(conditions[select_idx], size_R, Ra, Rc, pos1, pos2, result);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    delete[] pos1;
    delete[] pos2;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent fixed vector intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_ifvi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*independent fixed vector intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and independent fixed vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos1 = new int[size_R];
    int *pos2 = new int[size_R];
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_ifv(conditions[select_idx], size_R, Ra, Rc, pos1, pos2, result, count);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] pos1;
    delete[] pos2;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent fixed vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_ifvi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*shared fixed vector intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and shared fixed vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos = new int[size_R];
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_sfv(conditions[select_idx], size_R, Ra, Rc, pos, result);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    delete[] pos;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared fixed vector intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_sfvi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*shared fixed vector intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and shared fixed vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos = new int[size_R];
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_sfv(conditions[select_idx], size_R, Ra, Rc, pos, result, count);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] pos;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared fixed vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_sfvi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Vector-wise query processing model with late materialization strategy and fixed vector intermediate results as well as dynamic vector final result
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_vwm_lm_fv(const idx &size_R,
                            const T *Ra, const T *Rc,
                            const std::vector<idx> &conditions,
                            const std::vector<idx> &conditions_lsr,
                            std::ofstream &proalgo_timefile,
                            std::ofstream &proalgo_lsr_timefile)
{
  /*independent fixed vector intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and independent fixed vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos1 = new int[size_v];
    int *pos2 = new int[size_v];
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_ifv(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos1, pos2, result);
    }

    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    delete[] pos1;
    delete[] pos2;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent fixed vector intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_ifvi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*independent fixed vector intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and independent fixed vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos1 = new int[size_v];
    int *pos2 = new int[size_v];
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count += proalgo_cwm_lm_ifv(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos1, pos2, result, count);
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] pos1;
    delete[] pos2;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent fixed vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_ifvi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*shared fixed vector intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and shared fixed vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos = new int[size_v];
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    idx vec_num = DATA_NUM / size_v;
    timeval start, end;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_sfv(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos, result);
    }

    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    delete[] pos;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared fixed vector intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_sfvi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*shared fixed vector intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and shared fixed vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos = new int[size_v];
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count += proalgo_cwm_lm_sfv(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos, result, count);
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] pos;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared fixed vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_sfvi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*shared fixed vector intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and shared fixed vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions_lsr.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions_lsr[select_idx] / 10 << " %, total selection rate " << pow((double)conditions_lsr[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    int *pos = new int[size_v];
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count += proalgo_cwm_lm_sfv(conditions_lsr[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, pos, result, count);
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] pos;
    std::cout << "          Result count of selection rate " << (double)pow(conditions_lsr[select_idx] / 10, 2) / pow(100, 2) << "% is " << count << "/" << DATA_NUM << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_lsr_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared fixed vector intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_sfvi_fvf"
                     << "\t"
                     << log10(LSR_BASE * pow(LSR_STRIDE, select_idx))
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Column-wise query processing model with late materialization strategy and bitmap
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_cwm_lm_bmp(const idx &size_R,
                             const T *Ra, const T *Rc,
                             const std::vector<idx> &conditions,
                             std::ofstream &proalgo_timefile)
{
  /*independent bitmap intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and independent bitmap intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    bool *bitmap1 = new bool[size_R];
    bool *bitmap2 = new bool[size_R];
    result.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_ibmp(conditions[select_idx], size_R, Ra, Rc, bitmap1, bitmap2, result);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    delete[] bitmap1;
    delete[] bitmap2;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent bitmap intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_ibmpi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*independent bitmap intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and independent bitmap intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    bool *bitmap1 = new bool[size_R];
    bool *bitmap2 = new bool[size_R];
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_ibmp(conditions[select_idx], size_R, Ra, Rc, bitmap1, bitmap2, result, count);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] bitmap1;
    delete[] bitmap2;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent bitmap intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_ibmpi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }

  /*shared bitmap intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and shared dynamic vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    bool *bitmap = new bool[size_R];

    result.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_sbmp(conditions[select_idx], size_R, Ra, Rc, bitmap, result);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    delete[] bitmap;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared bitmap intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_sbmpi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*shared bitmap intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Column-wise query processing model with late materialization strategy and shared dynamic vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    bool *bitmap = new bool[size_R];
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_cwm_lm_sbmp(conditions[select_idx], size_R, Ra, Rc, bitmap, result, count);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] bitmap;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Column-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared bitmap intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_cwm_lm_sbmpi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Vector-wise query processing model with late materialization strategy and bitmap
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_vwm_lm_bmp(const idx &size_R,
                             const T *Ra, const T *Rc,
                             const std::vector<idx> &conditions,
                             const std::vector<idx> &conditions_lsr,
                             std::ofstream &proalgo_timefile,
                             std::ofstream &proalgo_lsr_timefile)
{
  /*independent bitmap intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and independent bitmap intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    bool *bitmap1 = new bool[size_v];
    bool *bitmap2 = new bool[size_v];
    result.reserve(size_R);
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_ibmp(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, bitmap1, bitmap2, result);
    }

    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    delete[] bitmap1;
    delete[] bitmap2;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent bitmap intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_ibmpi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*independent bitmap intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and independent bitmap intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    bool *bitmap1 = new bool[size_v];
    bool *bitmap2 = new bool[size_v];
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_ibmp(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, bitmap1, bitmap2, result, count);
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] bitmap1;
    delete[] bitmap2;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent bitmap intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_ibmpi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*independent bitmap intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and independent bitmap intermediate results as well as fixed vector final result in low selection rate test" << std::endl;

  for (idx select_idx = 0; select_idx != conditions_lsr.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions_lsr[select_idx] / 10 << " %, total selection rate " << pow((double)conditions_lsr[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    bool *bitmap1 = new bool[size_v];
    bool *bitmap2 = new bool[size_v];
    timeval start, end;
    idx vec_num = DATA_NUM / size_v;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_ibmp(conditions_lsr[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, bitmap1, bitmap2, result, count);
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] bitmap1;
    delete[] bitmap2;
    std::cout << "          Result count of selection rate " << (double)pow(conditions_lsr[select_idx] / 10, 2) / pow(100, 2) << "% is " << count << "/" << DATA_NUM << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_lsr_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "independent bitmap intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_ibmpi_fvf"
                     << "\t"
                     << log10(LSR_BASE * pow(LSR_STRIDE, select_idx))
                     << "\t"
                     << ms << std::endl;
  }
  /*shared bitmap intermediate results as well as dynamic vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and shared dynamic vector intermediate results as well as dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    bool *bitmap = new bool[size_v];
    idx vec_num = DATA_NUM / size_v;
    result.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_sbmp(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, bitmap, result);
    }

    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    delete[] bitmap;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared bitmap intermediate results"
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_sbmpi_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  /*shared bitmap intermediate results as well as fixed vector final result*/
  std::cout << ">>> start projection calculation by Vector-wise query processing model with late materialization strategy and shared dynamic vector intermediate results as well as fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    bool *bitmap = new bool[size_v];
    idx vec_num = DATA_NUM / size_v;
    timeval start, end;
    gettimeofday(&start, NULL);
    for (idx i = 0; i != vec_num; ++i)
    {
      count = proalgo_cwm_lm_sbmp(conditions[select_idx], size_v, Ra + i * size_v, Rc + i * size_v, bitmap, result, count);
    }

    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    delete[] bitmap;
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Vector-wise query processing"
                     << "\t"
                     << "late materialization strategy"
                     << "\t"
                     << "shared bitmap intermediate results"
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_vwm_lm_sbmpi_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Column-wise query processing model with late materialization strategy
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_cwm_lm(const idx &size_R,
                         const T *Ra, const T *Rc,
                         const std::vector<idx> &conditions,
                         std::ofstream &proalgo_timefile)
{
  /*1. dynamic vector*/
  test_proalgo_cwm_lm_dv(DATA_NUM, Ra, Rc, conditions, proalgo_timefile);
  /*2. fixed vector*/
  test_proalgo_cwm_lm_fv(DATA_NUM, Ra, Rc, conditions, proalgo_timefile);
  /*2. bitmap*/
  test_proalgo_cwm_lm_bmp(DATA_NUM, Ra, Rc, conditions, proalgo_timefile);
}
/**
 * @brief projection calculation by Vector-wise query processing model with late materialization strategy
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_vwm_lm(const idx &size_R,
                         const T *Ra, const T *Rc,
                         const std::vector<idx> &conditions,
                         const std::vector<idx> &conditions_lsr,
                         std::ofstream &proalgo_timefile,
                         std::ofstream &proalgo_lsr_timefile)
{
  /*1. dynamic vector*/
  test_proalgo_vwm_lm_dv(DATA_NUM, Ra, Rc, conditions, proalgo_timefile);
  /*2. fixed vector*/
  test_proalgo_vwm_lm_fv(DATA_NUM, Ra, Rc, conditions, conditions_lsr, proalgo_timefile, proalgo_lsr_timefile);
  /*2. bitmap*/
  test_proalgo_vwm_lm_bmp(DATA_NUM, Ra, Rc, conditions, conditions_lsr, proalgo_timefile, proalgo_lsr_timefile);
}
/**
 * @brief projection calculation by Row-wise query processing model
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_rowwise_model(const idx &size_R,
                                row_store_min *row_min,
                                row_store_max *row_max,
                                const std::vector<idx> &conditions,
                                const std::vector<idx> &conditions_lsr,
                                std::ofstream &proalgo_timefile,
                                std::ofstream &proalgo_lsr_timefile)
{
  std::cout << ">>> start projection calculation by Row-wise query processing model in cache with dynamic vector final result" << std::endl;
  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    timeval start, end;
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    gettimeofday(&start, NULL);
    count = proalgo_rowwise(conditions[select_idx], size_R, row_min, result);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Row-wise query processing model in cache"
                     << "\t"
                     << ""
                     << "\t"
                     << ""
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_rowwise_rwmic_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  std::cout << ">>> start projection calculation by Row-wise query processing model in cache with fixed vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_rowwise(conditions[select_idx], size_R, row_min, result);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Row-wise query processing model in cache"
                     << "\t"
                     << ""
                     << "\t"
                     << ""
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_rowwise_rwmic_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  std::cout << ">>> start projection calculation by Row-wise query processing model in cache with fixed vector final result in low selection rate test" << std::endl;

  for (idx select_idx = 0; select_idx != conditions_lsr.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions_lsr[select_idx] / 10 << " %, total selection rate " << pow((double)conditions_lsr[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_rowwise(conditions_lsr[select_idx], size_R, row_min, result);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << (double)pow(conditions_lsr[select_idx] / 10, 2) / pow(100, 2) << "% is " << count << "/" << DATA_NUM << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_lsr_timefile << "Row-wise query processing model in cache"
                         << "\t"
                         << ""
                         << "\t"
                         << ""
                         << "\t"
                         << "fixed vector final result"
                         << "\t"
                         << "proalgo_rowwise_rwmic_fvf"
                         << "\t"
                         << log10(LSR_BASE * pow(LSR_STRIDE, select_idx))
                         << "\t"
                         << ms << std::endl;
  }
  std::cout << ">>> start projection calculation by Row-wise query processing model out of cache with dynamic vector final result" << std::endl;

  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    std::vector<std::pair<int, int>> result;
    result.reserve(size_R);
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_rowwise(conditions[select_idx], size_R, row_max, result);
    gettimeofday(&end, NULL);
    result.clear();
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Row-wise query processing model out of cache"
                     << "\t"
                     << ""
                     << "\t"
                     << ""
                     << "\t"
                     << "dynamic vector final result"
                     << "\t"
                     << "proalgo_rowwise_rwmoc_dvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
  std::cout << ">>> start projection calculation by Row-wise query processing model out of cache with fixed vector final result" << std::endl;
  for (idx select_idx = 0; select_idx != conditions.size(); ++select_idx)
  {
    std::cout << "      column selection rate " << (double)conditions[select_idx] / 10 << "%, total selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "%" << std::endl;
    int count = 0;
    struct fixed_arrays result;
    result.pos_value1 = new idx[size_R];
    result.value2 = new idx[size_R];
    timeval start, end;
    gettimeofday(&start, NULL);
    count = proalgo_rowwise(conditions[select_idx], size_R, row_max, result);
    gettimeofday(&end, NULL);
    double ms = calc_ms(end, start);
    std::cout << "          Result count of selection rate " << pow((double)conditions[select_idx] / 10, 2) / pow(100, 2) * 100 << "% is " << count << "/" << size_R << std::endl;
    std::cout << "          Time: " << ms << "ms" << std::endl;
    proalgo_timefile << "Row-wise query processing model out of cache"
                     << "\t"
                     << ""
                     << "\t"
                     << ""
                     << "\t"
                     << "fixed vector final result"
                     << "\t"
                     << "proalgo_rowwise_rwmoc_fvf"
                     << "\t"
                     << 0.1 * (select_idx + 1)
                     << "\t"
                     << ms << std::endl;
  }
}
/**
 * @brief projection calculation by Column-wise query processing model
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_columnwise_model(const idx &size_R,
                                   const T *Ra, const T *Rc,
                                   const std::vector<idx> &conditions,
                                   std::ofstream &proalgo_timefile)
{
  /*1. early materialization strategy*/
  test_proalgo_cwm_em(DATA_NUM, Ra, Rc, conditions, proalgo_timefile);
  /*1. late materialization strategy*/
  test_proalgo_cwm_lm(DATA_NUM, Ra, Rc, conditions, proalgo_timefile);
}
/**
 * @brief projection calculation by Vector-wise query processing model
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo_vectorwise_model(const idx &size_R,
                                   const T *Ra, const T *Rc,
                                   const std::vector<idx> &conditions,
                                   const std::vector<idx> &conditions_lsr,
                                   std::ofstream &proalgo_timefile,
                                   std::ofstream &proalgo_lsr_timefile)
{
  /*1. early materialization strategy*/
  test_proalgo_vwm_em(DATA_NUM, Ra, Rc, conditions, conditions_lsr, proalgo_timefile, proalgo_lsr_timefile);
  /*1. late materialization strategy*/
  test_proalgo_vwm_lm(DATA_NUM, Ra, Rc, conditions, conditions_lsr, proalgo_timefile, proalgo_lsr_timefile);
}
/**
 * @brief projection algorithm implementation and testing
 * @param size_R
 * @param Ra
 * @param Rc
 * @param conditions
 * @param proalgo_timefile
 * @return void
 */
void test_proalgo(const idx &size_R,
                  const T *Ra, const T *Rc,
                  const std::vector<idx> &conditions,
                  const std::vector<idx> &conditions_lsr,
                  row_store_min *row_min,
                  row_store_max *row_max,
                  std::ofstream &proalgo_timefile,
                  std::ofstream &proalgo_lsr_timefile)
{
  /*1.Row-wise query processing model*/
  test_proalgo_rowwise_model(DATA_NUM, row_min, row_max, conditions, conditions_lsr, proalgo_timefile, proalgo_lsr_timefile);
  /*2.Column-wise query processing model*/
  test_proalgo_columnwise_model(DATA_NUM, Ra, Rc, conditions, proalgo_timefile);
  /*3.Vector-wise query processing model*/
  test_proalgo_vectorwise_model(DATA_NUM, Ra, Rc, conditions, conditions_lsr, proalgo_timefile, proalgo_lsr_timefile);
}
int main()
{

  std::ofstream proalgo_timefile, proalgo_lsr_timefile;

  proalgo_lsr_timefile.open(PROALGO_LSR_TIME_FILE, std::ios::out | std::ios::trunc);
  proalgo_lsr_timefile << "Query Processing Model"
                       << "\t"
                       << "Materialization strategy"
                       << "\t"
                       << "Intermediate Result Type"
                       << "\t"
                       << "Final Result Type"
                       << "\t"
                       << "Query Processing Model with different Materialization strategy、Intermediate Result Type and Final Result Type"
                       << "\t"
                       << "Lg(Selection Rate)"
                       << "\t"
                       << "Runtimes(ms)" << std::endl;

  proalgo_timefile.open(PROALGO_TIME_FILE, std::ios::out | std::ios::trunc);
  proalgo_timefile << "Query Processing Model"
                   << "\t"
                   << "Materialization strategy"
                   << "\t"
                   << "Intermediate Result Type"
                   << "\t"
                   << "Final Result Type"
                   << "\t"
                   << "Query Processing Model with different Materialization strategy、Intermediate Result Type and Final Result Type"
                   << "\t"
                   << "Selection Rate"
                   << "\t"
                   << "Runtimes(ms)" << std::endl;

  T *Ra = new T[DATA_NUM];
  T *Rc = new T[DATA_NUM];
  std::vector<int> conditions;
  std::vector<int> conditions_lsr;
  gen_data(DATA_NUM, Ra, Rc, row_min, row_max);
  gen_conditions(conditions, conditions_lsr);
  /*Projection algorithm based on different query models*/
  test_proalgo(DATA_NUM, Ra, Rc, conditions, conditions_lsr, row_min, row_max, proalgo_timefile, proalgo_lsr_timefile);
  delete[] Ra;
  delete[] Rc;
  conditions.clear();
  proalgo_timefile.close();
}