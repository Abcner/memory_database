#ifndef Gendata_H
#define Gendata_H

#include "metadata.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <cstring>
#include <pthread.h>
#include <unistd.h> /* sysconf */
#include <numaif.h> /* get_mempolicy() */
#include <numa.h>
#define RAND_RANGE48(N,STATE) ((double)nrand48(STATE)/((double)RAND_MAX+1)*(N))


static int seeded = 0;
static unsigned int seedValue;
static int inited = 0;
static int max_cpus;
static int socket_num = 0;
static int numa[10][200]={{},{}};
static int numa_num[10]={0};
static int numa_count[4]={0};
/**
 * @brief Bind to corresponding NUMA node
 * 
 * @param[in] numaid
 * @return void
 */
void bind_numa(int numaid){
    int nr_nodes=socket_num;
    struct bitmask *new_nodes;
    new_nodes = numa_bitmask_alloc(nr_nodes); 
    numa_bitmask_setbit(new_nodes, numaid);
    numa_bind(new_nodes);
}
/**
 * @brief Get hardware information about NUMA
 * 
 * @return int the generated random number 
 */
static void
get_numa_info(){
    socket_num = numa_max_node()+1;
    int i;
    max_cpus  = sysconf(_SC_NPROCESSORS_ONLN);
    for(i=0;i<max_cpus;i++){
        numa[numa_node_of_cpu(i)][numa_num[numa_node_of_cpu(i)]++]=i;

    }
    
   
}
/**
 * @brief Obtain the cpuid on each NUMA node sequentially based on the threadid
 * 
 * @param[in] threadid
 * @return int the generated random number 
 */
int
get_cpuid_bynumaid(int threadid){
    if(!inited){
        get_numa_info();
        inited = 1;
    }
    int numa_id  = threadid%socket_num;
    int result = numa[numa_id][numa_count[numa_id]];
    numa_count[numa_id]++;
    if(numa_count[numa_id]==numa_num[numa_id])numa_count[numa_id]=0;
    return result;
}
/**
 * @brief Obtain the NUMA node number to which mytid belongs
 * 
 * @param[in] mytid
 * @return int the generated random number 
 */
int
get_numa_id(int mytid)
{

    if(!inited){
        get_numa_info();
        inited = 1;
    }
    int ret = 0;

    for(int i = 0; i < socket_num; i++)
        for(int j = 0; j < numa_num[i]; j++)
            if(numa[i][j] == mytid){
                ret = i;
                return ret;
            }
    
    

    return 0;

}
/**
 * @brief Obtain the NUMA node number to which ptr belongs
 * 
 * @param[in] ptr
 * @return int the generated random number 
 */
int 
get_numa_node_of_address(void * ptr)
{
    int numa_node = -1;
    get_mempolicy(&numa_node, NULL, 0, ptr, MPOL_F_NODE | MPOL_F_ADDR);
    return numa_node;
}
/**
 * @Randomly assign values to seed and seedValue
 * 
 * @return void
 */
void 
seed_generator(unsigned int seed) 
{
    srand(seed);
    seedValue = seed;
    seeded = 1;
}
/**
 * @Check wheter seeded, if not seed the generator with current time
 * 
 * @return void
 */
static void
check_seed()
{
    if(!seeded) {
        seedValue = time(NULL);
        srand(seedValue);
        seeded = 1;
    }
}
/**
 * @brief generate random number in range [1, range]
 * 
 * @param[in] table
 * @param[in] SF
 * @return int the generated random number 
 */
inline int size_of_table(const TABLE_NAME& table, const double& SF) {
    switch(table) {
        case customer :
            return CUSTOMER_BASE * SF;
        case supplier :
            return SUPPLIER_BASE * SF;
        case part :
            return (int)(PART_BASE * (double)(1 + log2(SF)));
        case date : 
            return DATE_BASE;
        case lineorder : 
            return LINEORDER_BASE * SF;
    }
    return 0;
}
/**
 * @brief generate random number in range [1, range]
 * 
 * @param[in] range 
 * @return int the generated random number 
 */

inline int rand_x(int range) {
    return ((rand() % (range)) + 1);
}
/**
 * @brief generate random number in range [range_min, range_max]
 * 
 * @param[in] range_min 
 * @param[in] range_max
 * @return int the generated random number 
 */
inline int rand_x(int range_min, int range_max)
{
  return (rand()%(range_max-range_min + 1) + range_min);
}
/**
 * @brief generate random number in range [min, max], for double
 * 
 * @param max 
 * @param min 
 * @return double the generated random number 
 */
inline double rand_x(double min, double max ) {
    return min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (max - min)));
}

/**
 * @brief generate test data
 * 
 * @param[out] Ra column a range: 1-100 
 * @param[out] Rb column b range: 1-100 
 * @param[out] Rc column c range: 1-100 
 * @param[out] Rd column d range: 1-100 
 * @return int the number of lines generated
 */
idx gen_data(const idx& size_R, T* Ra, T* Rb, T* Rc, T* Rd, bool is_lsr) {
    size_t i;
    if(is_lsr)
    {
      for(i = 0; i != size_R; ++i){  
        Ra[i] = (rand_x(DATA_MAX));
        Rb[i] = (rand_x(DATA_MAX));
        Rc[i] = (rand_x(DATA_MAX));
        Rd[i] = 1;
      }
      
    }
    else
    {
      for(i = 0; i != size_R; ++i){  
        Ra[i] = (rand_x(DATA_MAX));
        Rb[i] = (rand_x(DATA_MAX));
        Rc[i] = (rand_x(DATA_MAX));
        Rd[i] = 1;
      }
    }
    return i;
}
/**
 * @brief generate test data for project test
 * @param[in]  size_R size of the data set (rows)
 * @param[out] column Ra range: 1-100 
 * @param[out] column Rc range: 1-100 
 * @return int the number of lines generated
 */
int gen_data(const idx& size_R, T* Ra, T* Rc, row_store_min *row_min,  row_store_max *row_max) {

    idx i;
    for(i = 0; i != size_R; ++i){
        Ra[i] = (rand_x(DATA_MAX));
        row_min[i].Ra = Ra[i];
        row_max[i].Ra = Ra[i];
        Rc[i] = (rand_x(DATA_MAX));
        row_min[i].Rc = Rc[i];
        row_max[i].Rc = Rc[i];
    }
    std::cout << ">>> Generated data " << i <<  " lines." << std::endl;
    return i;
}
/**
 * @brief randomly shuffle elements
 * @param[in]  state A parameter used to determine a random number
 * @param[out] relation Data structure for storing table data
 * @return void
 */
void 
knuth_shuffle48(relation_t * relation, unsigned short * state)
{
    int i;
    for (i = relation->num_tuples - 1; i > 0; i--) {
        int64_t  j              = RAND_RANGE48(i, state);
        int tmp            = relation->key[i];
        relation->key[i] = relation->key[j];
        relation->key[j] = tmp;
    }
}
/**
 * @brief Create random unique keys starting from firstkey
 * @param[in]  args Data structure for storing parameter data
 * @return void*
 */
void *
random_unique_gen_thread(void * args) 
{
    create_arg_t * arg      = (create_arg_t *) args;
    relation_t *   rel      = & arg->rel;
    int64_t        firstkey = arg->firstkey;
    int64_t        maxid    = arg->maxid;
    uint64_t i;

    /* for randomly seeding nrand48() */
    unsigned short state[3] = {0, 0, 0};
    unsigned int seed       = time(NULL) + * (unsigned int *) pthread_self();
    memcpy(state, &seed, sizeof(seed));
    for (i = 0; i < rel->num_tuples; i++) {
        if(firstkey %(maxid+1)==0)firstkey = 1;
        firstkey = firstkey%(maxid+1);
        rel->key[i]     = firstkey;
        rel->payload[i] = 1;

      

        firstkey ++;
    }

    /* randomly shuffle elements */
    knuth_shuffle48(rel, state); 
    return 0;
}
/**
 * @brief generate test data for join test
 * @param[in]  size size of the table (rows)
 * @param[in] maxid The maximum random value 
 * @param[in] nthreads The concurrent execution granularity
 * @param[out] relation The data structure for storing table data
 * @return int the number of lines generated
 */
int gen_data(const idx& size, const idx& maxid,
              relation_t * relation, const idx& nthreads) {
    int rv;
    uint32_t i;
    uint64_t offset = 0;
    create_arg_t args[nthreads];
    pthread_t tid[nthreads];
    cpu_set_t set;
    pthread_attr_t attr;
    pthread_barrier_t barrier;
    uint64_t ntuples_perthr;
    uint64_t ntuples_lastthr;
    ntuples_perthr = size / nthreads;
    ntuples_lastthr = size - ntuples_perthr * (nthreads-1);
    pthread_attr_init(&attr);
    rv = pthread_barrier_init(&barrier, NULL, nthreads);
    if(rv != 0){
        printf("[ERROR] Couldn't create the barrier\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < nthreads; i++ )
    {
      int cpu_idx = i;
      CPU_ZERO(&set);
      CPU_SET(cpu_idx, &set);
      pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &set);
      args[i].firstkey = offset + 1;
      args[i].maxid = maxid;
      args[i].ridstart = offset;
      args[i].rel.key = relation->key + offset;
      args[i].rel.payload = relation->payload + offset;
      args[i].rel.num_tuples = (i == nthreads-1) ? ntuples_lastthr 
                                 : ntuples_perthr;
      args[i].barrier = &barrier;
      offset += ntuples_perthr;
      rv = pthread_create(&tid[i], &attr, random_unique_gen_thread, 
                            (void*)&args[i]);
      if (rv){
            fprintf(stderr, "[ERROR] pthread_create() return code is %d\n", rv);
            exit(-1);
      }
    }
    for(i = 0; i < nthreads; i++){
        pthread_join(tid[i], NULL);
    }
    return 0;

}
/**
 * @brief generate test data for star join test
 * 
 * @param rate selection rate
 * @param SF   
 * @param dimvec_c 
 * @param dimvec_s 
 * @param dimvec_p 
 * @param dimvec_d 
 * @param fk_c 
 * @param fk_s 
 * @param fk_p 
 * @param fk_d 
 * @return int 
 */
int gen_data(const double& rate, const double& SF, int8_t* &dimvec_c, int8_t* &dimvec_s, int8_t* &dimvec_p, int8_t* &dimvec_d, 
             int32_t* &fk_c, int32_t* &fk_s, int32_t* &fk_p, int32_t* &fk_d) {
    int size_customer = size_of_table(TABLE_NAME::customer, SF);
    int size_supplier = size_of_table(TABLE_NAME::supplier, SF);
    int size_part = size_of_table(TABLE_NAME::part, SF);
    int size_date = size_of_table(TABLE_NAME::date, SF);
    dimvec_c = new int8_t[size_customer];
    dimvec_s = new int8_t[size_supplier];
    dimvec_p = new int8_t[size_part];
    dimvec_d = new int8_t[size_date];
    int size_lineorder = size_of_table(TABLE_NAME::lineorder, SF);
    fk_c = new int32_t[size_lineorder];
    fk_s = new int32_t[size_lineorder];
    fk_p = new int32_t[size_lineorder];
    fk_d = new int32_t[size_lineorder];


    // generate data for customer table
    for(size_t i = 0; i != size_customer; ++i){
        dimvec_c[i] = rand_x(0.0, 1.0) <= rate ? (int8_t)rand_x(0, 1 << GROUP_BITS_TABLE - 1) : DIM_NULL;
    }

    // generate data for supplier table
    for(size_t i = 0; i != size_supplier; ++i){
        dimvec_s[i] = rand_x(0.0, 1.0) <= rate ? (int8_t)rand_x(0, 1 << GROUP_BITS_TABLE - 1) : DIM_NULL;
    }

    // generate data for part table
    for(size_t i = 0; i != size_part; ++i){
        dimvec_p[i] = rand_x(0.0, 1.0) <= rate ? (int8_t)rand_x(0, 1 << GROUP_BITS_TABLE - 1) : DIM_NULL;
    }

    // generate data for date table
    for(size_t i = 0; i != size_date; ++i){
        dimvec_d[i] = rand_x(0.0, 1.0) <= rate ? (int8_t)rand_x(0, 1 << GROUP_BITS_TABLE - 1) : DIM_NULL;
    }

    // generate data for lineorder table
    for(size_t i = 0; i != size_lineorder; ++i){
        fk_c[i] = rand_x(0, size_customer);
        fk_s[i] = rand_x(0, size_supplier);
        fk_p[i] = rand_x(0, size_part);
        fk_d[i] = rand_x(0, size_date);
    }

    std::cout << ">>> Generated data " << size_lineorder <<  " lines in lineorder table." << std::endl;
    return 0;
}
/**
 * @brief generate test data for OLAPcore test
 * 
 * @param c_sele selection rate
 * @param s_sele   
 * @param p_sele 
 * @param d_sele 
 * @param SF 
 * @param c_bitmap 
 * @param s_bitmap 
 * @param p_bitmap 
 * @param d_bitmap 
 * @param c_groups 
 * @param s_groups 
 * @param p_groups 
 * @param d_groups
 * @param dimvec_c
 * @param dimvec_s
 * @param dimvec_p
 * @param dimvec_d
 * @param fk_c
 * @param fk_s
 * @param fk_p
 * @param fk_d
 * @param M1
 * @param M2
 * @return int 
 */
int gen_data(const double& c_sele, const double& s_sele, const double& p_sele, const double& d_sele,
             const double& SF, const int& c_bitmap, const int &s_bitmap, const int &p_bitmap, const int &d_bitmap,
             const int &c_groups, const int &s_groups, const int &p_groups, const int &d_groups, 
             int8_t* &dimvec_c, int8_t* &dimvec_s, int8_t* &dimvec_p, int8_t* &dimvec_d, 
             int32_t* &fk_c, int32_t* &fk_s, int32_t* &fk_p, int32_t* &fk_d,
             int32_t * &M1, int32_t * &M2) {

    int size_customer = size_of_table(TABLE_NAME::customer, SF);
    int size_supplier = size_of_table(TABLE_NAME::supplier, SF);
    int size_part = size_of_table(TABLE_NAME::part, SF);
    int size_date = size_of_table(TABLE_NAME::date, SF);
    dimvec_c = new int8_t[size_customer];
    dimvec_s = new int8_t[size_supplier];
    dimvec_p = new int8_t[size_part];
    dimvec_d = new int8_t[size_date];
    int size_lineorder = size_of_table(TABLE_NAME::lineorder, SF);
    fk_c = new int32_t[size_lineorder];
    fk_s = new int32_t[size_lineorder];
    fk_p = new int32_t[size_lineorder];
    fk_d = new int32_t[size_lineorder];
    M1 = new int32_t[size_lineorder];
    M2 = new int32_t[size_lineorder];

    // generate data for customer table
    if (c_bitmap)
    {
        for(size_t i = 0; i != size_customer; ++i){
          dimvec_c[i] = rand_x(0.0, 1.0) <= c_sele ? (int8_t)rand_x(0, c_groups - 1) : DIM_NULL;
      }
    }
    else
    {
      for(size_t i = 0; i != size_customer; ++i){
          dimvec_c[i] = rand_x(0.0, 1.0) <= c_sele ? 0 : DIM_NULL;
      }
    }
    

    // generate data for supplier table
    if (s_bitmap)
    {
      for(size_t i = 0; i != size_supplier; ++i){
        dimvec_s[i] = rand_x(0.0, 1.0) <= s_sele ? (int8_t)rand_x(0, s_groups - 1) : DIM_NULL;
      }
    }
    else
    {
      for(size_t i = 0; i != size_supplier; ++i){
        dimvec_s[i] = rand_x(0.0, 1.0) <= s_sele ? 0 : DIM_NULL;
      }
    }
    

    // generate data for part table
    if (p_bitmap)
    {
      for(size_t i = 0; i != size_part; ++i){
        dimvec_p[i] = rand_x(0.0, 1.0) <= p_sele ? (int8_t)rand_x(0, p_groups - 1) : DIM_NULL;
      }
    }
    else
    {
      for(size_t i = 0; i != size_part; ++i){
        dimvec_p[i] = rand_x(0.0, 1.0) <= p_sele ? 0 : DIM_NULL;
      }
    }
    // generate data for date table
    if (d_bitmap)
    {
      for(size_t i = 0; i != size_date; ++i){
        dimvec_d[i] = rand_x(0.0, 1.0) <= d_sele ? (int8_t)rand_x(0, d_groups - 1) : DIM_NULL;
      }
    }
    else
    {
      for(size_t i = 0; i != size_date; ++i){
        dimvec_d[i] = rand_x(0.0, 1.0) <= d_sele ? 0 : DIM_NULL;
      }
    }

    // generate data for lineorder table
    for(size_t i = 0; i != size_lineorder; ++i){
        fk_c[i] = rand_x(0, size_customer);
        fk_s[i] = rand_x(0, size_supplier);
        fk_p[i] = rand_x(0, size_part);
        fk_d[i] = rand_x(0, size_date);
        M1[i] = 5;
        M2[i] = 5;

    }

    std::cout << ">>> Generated data " << size_lineorder <<  " lines in lineorder table."<< std::endl;
    return 0;
}

/**
 * @brief generate test data for OLAPcore numa test
 * 
 * @param c_sele selection rate
 * @param s_sele   
 * @param p_sele 
 * @param d_sele 
 * @param SF 
 * @param c_bitmap 
 * @param s_bitmap 
 * @param p_bitmap 
 * @param d_bitmap 
 * @param c_groups 
 * @param s_groups 
 * @param p_groups 
 * @param d_groups
 * @param dimvec_c
 * @param dimvec_s
 * @param dimvec_p
 * @param dimvec_d
 * @param fk_c
 * @param fk_s
 * @param fk_p
 * @param fk_d
 * @param M1
 * @param M2
 * @return int 
 */
int gen_data(const double& c_sele, const double& s_sele, const double& p_sele, const double& d_sele,
             const double& SF, const int& c_bitmap, const int &s_bitmap, const int &p_bitmap, const int &d_bitmap,
             const int &c_groups, const int &s_groups, const int &p_groups, const int &d_groups, 
             int8_t** dimvec_c_p, int8_t** dimvec_s_p, int8_t** dimvec_p_p, int8_t** dimvec_d_p, 
             int32_t** fk_c_p, int32_t** fk_s_p, int32_t** fk_p_p, int32_t** fk_d_p,
             int32_t ** M1_p, int32_t ** M2_p) {
    int numa_num = numa_max_node() + 1;
    int *num_lineorder = new int[numa_num];
    
    int size_customer = size_of_table(TABLE_NAME::customer, SF);
    int size_supplier = size_of_table(TABLE_NAME::supplier, SF);
    int size_part = size_of_table(TABLE_NAME::part, SF);
    int size_date = size_of_table(TABLE_NAME::date, SF);
    int size_lineorder = size_of_table(TABLE_NAME::lineorder, SF);
    get_numa_info();
    for (int i = 0; i < numa_num; i++)
    {
      if (i == numa_num - 1) num_lineorder[i] = size_lineorder - (numa_num - 1) * size_lineorder/numa_num;
      else num_lineorder[i] = size_lineorder/numa_num;
    }
    for (int i = 0; i < numa_num; i++)
    {
      bind_numa(i);
      dimvec_c_p[i] = (int8_t *)numa_alloc(sizeof(int8_t) * size_customer);
      dimvec_s_p[i] = (int8_t *)numa_alloc(sizeof(int8_t) * size_supplier);
      dimvec_p_p[i] = (int8_t *)numa_alloc(sizeof(int8_t) * size_part);
      dimvec_d_p[i] = (int8_t *)numa_alloc(sizeof(int8_t) * size_date);
      fk_c_p[i] = (int32_t *)numa_alloc(sizeof(int32_t) * num_lineorder[i]);
      fk_s_p[i] = (int32_t *)numa_alloc(sizeof(int32_t) * num_lineorder[i]);
      fk_p_p[i] = (int32_t *)numa_alloc(sizeof(int32_t) * num_lineorder[i]);
      fk_d_p[i] = (int32_t *)numa_alloc(sizeof(int32_t) * num_lineorder[i]);
      M1_p[i] = (int32_t *)numa_alloc(sizeof(int32_t) * num_lineorder[i]);
      M2_p[i] = (int32_t *)numa_alloc(sizeof(int32_t) * num_lineorder[i]);
    }


    // generate data for customer table
    if (c_bitmap)
    {
        for(size_t i = 0; i != size_customer; ++i){
          dimvec_c_p[0][i] = rand_x(0.0, 1.0) <= c_sele ? (int8_t)rand_x(0, c_groups - 1) : DIM_NULL;
          for (int j = 1; j < numa_num; j++) dimvec_c_p[j][i] = dimvec_c_p[0][i];
      }
    }
    else
    {
      for(size_t i = 0; i != size_customer; ++i){
          dimvec_c_p[0][i] = rand_x(0.0, 1.0) <= c_sele ? 0 : DIM_NULL;
          for (int j = 1; j < numa_num; j++) dimvec_c_p[j][i] = dimvec_c_p[0][i];
      }
    }
    

    // generate data for supplier table
    if (s_bitmap)
    {
      for(size_t i = 0; i != size_supplier; ++i){
        dimvec_s_p[0][i] = rand_x(0.0, 1.0) <= s_sele ? (int8_t)rand_x(0, s_groups - 1) : DIM_NULL;
        for (int j = 1; j < numa_num; j++) dimvec_s_p[j][i] = dimvec_s_p[0][i];
      }
    }
    else
    {
      for(size_t i = 0; i != size_supplier; ++i){
        dimvec_s_p[0][i] = rand_x(0.0, 1.0) <= s_sele ? 0 : DIM_NULL;
        for (int j = 1; j < numa_num; j++) dimvec_s_p[j][i] = dimvec_s_p[0][i];
      }
    }
    

    // generate data for part table
    if (p_bitmap)
    {
      for(size_t i = 0; i != size_part; ++i){
        dimvec_p_p[0][i] = rand_x(0.0, 1.0) <= p_sele ? (int8_t)rand_x(0, p_groups - 1) : DIM_NULL;
        for (int j = 1; j < numa_num; j++) dimvec_p_p[j][i] = dimvec_p_p[0][i];
      }
    }
    else
    {
      for(size_t i = 0; i != size_part; ++i){
        dimvec_p_p[0][i] = rand_x(0.0, 1.0) <= p_sele ? 0 : DIM_NULL;
        for (int j = 1; j < numa_num; j++) dimvec_p_p[j][i] = dimvec_p_p[0][i];
      }
    }
    // generate data for date table
    if (d_bitmap)
    {
      for(size_t i = 0; i != size_date; ++i){
        dimvec_d_p[0][i] = rand_x(0.0, 1.0) <= d_sele ? (int8_t)rand_x(0, d_groups - 1) : DIM_NULL;
        for (int j = 1; j < numa_num; j++) dimvec_d_p[j][i] = dimvec_d_p[0][i];
      }
    }
    else
    {
      for(size_t i = 0; i != size_date; ++i){
        dimvec_d_p[0][i] = rand_x(0.0, 1.0) <= d_sele ? 0 : DIM_NULL;
        for (int j = 1; j < numa_num; j++) dimvec_d_p[j][i] = dimvec_d_p[0][i];
      }
    }
    

    // generate data for lineorder table
    for (int i = 0; i < numa_num; i++)
    {
      for (int j = 0; j < num_lineorder[i]; j++)
      {
        fk_c_p[i][j] = rand_x(0, size_customer);
        fk_s_p[i][j] = rand_x(0, size_supplier);
        fk_p_p[i][j] = rand_x(0, size_part);
        fk_d_p[i][j] = rand_x(0, size_date);
        M1_p[i][j] = 5;
        M2_p[i][j] = 5;
      }
    }
    std::cout << ">>> Generated data " << size_lineorder <<  " lines in lineorder table."<< std::endl;
    return 0;
}
/**
 * @brief generate conditions according to the selection rate of each test
 * 
 * @param conditions 
 * @return int number of conditions
 */
idx gen_conditions(std::vector<idx>& conditions, bool is_lsr) {
    int i;

    if(is_lsr)
    {
      for(i = 0; i != CONST_TEST_NUM_LSR; ++i) {
        conditions.emplace_back(DATA_MAX * pow(LSR_BASE * pow(LSR_STRIDE, i), 1.0/3));
        }
    
      std::cout << ">>> Generated conditions ";
      for(size_t j = 0; j != conditions.size(); ++j) {
        std::cout << (double)conditions[j]/10 << "%\t";
      }
    }
    else
    {
      for(i = 0; i != CONST_TEST_NUM; ++i) {
        conditions.emplace_back(DATA_MAX * pow((CONST_BASE + (CONST_STRIDE * i)), 1.0/3));
      }
    
      std::cout << ">>> Generated conditions ";
      for(size_t j = 0; j != conditions.size(); ++j) {
      std::cout << conditions[j] / 10 << "%\t";
      }
      
    }

    std::cout << std::endl;
    return i;
}
/**
 * @brief generate test data
 * 
 * @param size_R 
 * @param group_num 
 * @param vecInx 
 * @param m1 
 * @param m2 
 * @return int number of records generated
 */
int gen_data(const idx& size_R, const int& group_num, int* vecInx, int* m1, int* m2)
{
  idx i;
  for (i = 0; i != size_R; ++i)
  {
    vecInx[i] = (rand_x(group_num));
    m1[i] = M_VALUE;
    m2[i] = M_VALUE;
  }
  return i;
}
/**
 * @brief generate test data
 * 
 * @param size_R 
 * @param group_num 
 * @param vecInx 
 * @param m1 
 * @param m2 
 * @return int number of records generated
 */
int gen_data(const idx& size_R, double *l_tax, double *l_quantity, double *l_extendedprice)
{
  idx i;
  for (i = 0; i != size_R; ++i)
  {
    l_tax[i] = ((double)(rand_x(L_TAX_MIN, L_TAX_MAX)))/100;
    l_quantity[i] = (double)rand_x(L_QUANTITY_MIN, L_QUANTITY_MAX);
    l_extendedprice[i] = (double)rand_x(L_EXPTENDEDPRICE_MIN, L_EXPTENDEDPRICE_MAX);
  }
  std::cout << ">>> Generated data " << i <<  " lines."<< std::endl;
  return i;
}
/**
 * @brief generate test data
 * 
 * @param size_R 
 * @param group_num 
 * @param vecInx 
 * @param m1 
 * @param m2 
 * @return int number of records generated
 */
int gen_data(const idx& size_R, double *l_tax, int64_t *l_quantity, int64_t *l_extendedprice)
{
  idx i;
  for (i = 0; i != size_R; ++i)
  {
    l_tax[i] = ((double)(rand_x(L_TAX_MIN, L_TAX_MAX)))/100;
    l_quantity[i] = rand_x(L_QUANTITY_MIN, L_QUANTITY_MAX);
    l_extendedprice[i] = rand_x(L_EXPTENDEDPRICE_MIN, L_EXPTENDEDPRICE_MAX);
  }
  std::cout << ">>> Generated data " << i <<  " lines."<< std::endl;
  return i;
}
/**
 * @brief generate conditions according to the selection rate of each test
 * 
 * @param conditions 
 * @return int number of conditions
 */
idx gen_conditions(std::vector<int>& conditions, std::vector<int>& conditions_lsr) {
    int i;
    for(i = 0; i != CONST_TEST_NUM; ++i) {
      conditions.emplace_back(DATA_MAX * pow((CONST_BASE + (CONST_STRIDE * i)), 1.0/2));
    }
    
    std::cout << ">>> Generated constant stride test conditions ";
    for(size_t j = 0; j != conditions.size(); ++j) {
      std::cout << (double)conditions[j]/10 << "%\t";
    }
    std::cout << std::endl;
    for(i = 0; i != CONST_TEST_NUM_LSR; ++i) {
      conditions_lsr.emplace_back(DATA_MAX * pow(LSR_BASE * pow(LSR_STRIDE, i), 1.0/2));
    }
    std::cout << ">>> Generated low selection rate test conditions ";
    for(size_t j = 0; j != conditions_lsr.size(); ++j) {
      std::cout << (double)conditions_lsr[j]/10 << "%\t";
    }

    std::cout << std::endl;
    return i;
}
int get_cachesize(const std::string &filePath)
{
  std::ifstream cachefile(filePath, std::ios::in);
  std::string temp;
  if (!cachefile.is_open())
  {
    std::cout << "file open failed!" << std::endl;
    return 0;
  }
  getline(cachefile, temp);
  int position = temp.find("K");
  if (position)
  {
    temp = temp.substr(0, position);
    int cache_size = stoi(temp) * 1024 / 4 ;
    return cache_size;
  }
  position = temp.find("M");
  if (position)
  {
    temp = temp.substr(0, position);
    int cache_size = stoi(temp) * 1024 * 1024 / 4 ;
    return cache_size;
  }
  position = temp.find("G");
  if (position)
  {
    temp = temp.substr(0, position);
    int cache_size = stoi(temp) * 1024 * 1024 * 1024 / 4 ;
    return cache_size;
  }
  return 0;
}
#endif