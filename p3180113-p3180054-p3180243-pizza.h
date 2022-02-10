#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

// Initialization of variables
const int N_tel = 3;
const int N_cook = 2;
const int N_oven= 10;
const int N_deliverer= 7;
const int T_order_low = 1;
const int T_order_high = 5;
const int N_order_low = 1;
const int N_order_high = 5;
const int T_payment_low = 1;
const int T_payment_high = 2;
const int C_pizza = 10;
const double P_fail = 0.05;
const int T_prep = 1;
const int T_bake = 10;
const int T_pack = 2;
const int T_del_low = 5;
const int T_del_high = 15;
double order_packed_time;
double order_delivered_time;
double max_cold_time;
double max_waiting_time;
double max_service_time;
double total_waiting_time;
double total_service_time;
double total_cold_time;
double cold_time;
double waiting_time;
double service_time;
int noOneCook;
int packEmployee;
int countAvailableOven;
int costOfPizza;
int noTel;
int numberOfDeliverers;
int income;
int countOfSuccessfulOrders;
int countOfFailOrders;
double pCardFail;
unsigned int seed;

// Declaration of mutexes
pthread_mutex_t mutex_no_cook = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_no_oven = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_no_pack_employee = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_no_deliverer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_no_tel = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_income = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_order_packed_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_order_delivered_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_waiting_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_waiting_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_waiting_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_service_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_service_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_service_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_count_of_successful_orders = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_count_of_fail_orders = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_screen = PTHREAD_MUTEX_INITIALIZER;

// Declaration of conditions
pthread_cond_t cond_no_cook = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_no_oven = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_no_pack_employee = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_no_deliverer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_no_tel = PTHREAD_COND_INITIALIZER;