#include "p3180113-p3180054-p3180243-pizza.h"


// Implemantation of exe func 
void* exefunc(void* coid){
    int rc;
    int* oid = (int*)coid;
    unsigned int alt_seed = seed + *oid;
    struct timespec customerAppearanceTime; // when the customer appears
    struct timespec employeeCallTime; // when the customer start talking with employee
    struct timespec startTime; // when the order is given
    struct timespec preparationTime; // when the order starts being prepared
    struct timespec bakeTime; // when the order is inserted in the oven
    struct timespec bakedTime; // when the order is out of the oven
    struct timespec pakedTime; // when the order is packed by employee
    struct timespec deliveredTime; // when the order is delivered to the customer

    // get the time that cusotmer appears
    clock_gettime(CLOCK_REALTIME, &customerAppearanceTime);
    
    rc = pthread_mutex_lock(&mutex_no_tel);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&rc);
    }
    while(noTel<=0){
        rc = pthread_cond_wait(&cond_no_tel, &mutex_no_tel);
        if (rc != 0) {
            printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
            pthread_exit(&rc);
        }
    }
    printf("Βρέθηκε τηλεφωνητής για την εξυπηρέτηση του πελάτη.\n");

    // get the time that customer start talking with employee
    clock_gettime(CLOCK_REALTIME, &employeeCallTime);

    --noTel;
    rc = pthread_mutex_unlock(&mutex_no_tel);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit(&rc);
    }

    // generated random number of pizzas
    int numberOfPizzas = (rand_r(&alt_seed) % (N_order_high - N_order_low + 1)) + N_order_low;
    
    // generated random number of the payment time the tel needs to charge the customer
    int paymentTime = (rand_r(&alt_seed) % (T_payment_high - T_payment_low + 1)) + T_payment_low;
    sleep(paymentTime); // Wait until the payments is done

    // Payment procedure
    bool fail_payment = rand_r(&alt_seed) % 100 / 100.0f <= pCardFail; // the estimated values of fail_payment is true or false
    
    if (!fail_payment){
        // Successful payment
        rc = pthread_mutex_lock(&mutex_income);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        income += numberOfPizzas * costOfPizza; // to printaroume sthn main
        rc = pthread_mutex_unlock(&mutex_income);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
        printf("\nΗ παραγγελία με αριθμό %d καταχωρήθηκε.\n",*oid);

        rc = pthread_mutex_lock(&mutex_count_of_successful_orders);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        ++countOfSuccessfulOrders;
        rc = pthread_mutex_unlock(&mutex_count_of_successful_orders);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        //Release tel
        rc = pthread_mutex_lock(&mutex_no_tel);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        ++noTel;
        pthread_cond_signal(&cond_no_tel);
        rc = pthread_mutex_unlock(&mutex_no_tel);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
    
        // Start the procedure with cooks
        clock_gettime(CLOCK_REALTIME, &startTime);
        printf("Αρχίζει η διαδικασία για την παραγγελία με αριθμό %d .\n", *oid);
        rc = pthread_mutex_lock(&mutex_no_cook);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        while(noOneCook<=0){
            rc = pthread_cond_wait(&cond_no_cook, &mutex_no_cook);
            if (rc != 0) {
                printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
                pthread_exit(&rc);
            }
        }
        printf("Βρέθηκε ψήστης για την παραγγελία με αριθμό %d.\n", *oid);
        --noOneCook;
        rc = pthread_mutex_unlock(&mutex_no_cook);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Start the procedure for preparation
        clock_gettime(CLOCK_REALTIME, &preparationTime);

        sleep(numberOfPizzas*T_prep); // prepare each pizza for T_prep time

        // Start procedure for bake pizza
        rc = pthread_mutex_lock(&mutex_no_oven);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        while(countAvailableOven<=numberOfPizzas){
            rc = pthread_cond_wait(&cond_no_oven, &mutex_no_oven);
            if (rc != 0) {
                printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
                pthread_exit(&rc);
            }
        }
        printf("Βρέθηκαν φούρνοι για την παραγγελία με αριθμό %d.\n", *oid);
        countAvailableOven -= numberOfPizzas;
        rc = pthread_mutex_unlock(&mutex_no_oven);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        //Release cook
        rc = pthread_mutex_lock(&mutex_no_cook);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        ++noOneCook;
        pthread_cond_signal(&cond_no_cook);
        rc = pthread_mutex_unlock(&mutex_no_cook);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        clock_gettime(CLOCK_REALTIME, &bakeTime); // get the time that the order is inserted in the oven
        sleep(T_bake); // Pizza staying in the oven until pack employee come and get it

        // Start the procedure with pack employee
        rc = pthread_mutex_lock(&mutex_no_pack_employee);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        if(packEmployee != 1){
            rc = pthread_cond_wait(&cond_no_pack_employee, &mutex_no_pack_employee);
            if (rc != 0) {
                printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
                pthread_exit(&rc);
            }
        }
        printf("Ο υπάλληλος πακεταρίσματος αναλαμβάνει την παραγγελία με αριθμό %d.\n", *oid);
        packEmployee = 0;
        rc = pthread_mutex_unlock(&mutex_no_pack_employee);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        clock_gettime(CLOCK_REALTIME, &bakedTime);
        sleep(T_pack); // until the pack employee pack all pizzas in 2 minutes
        
        clock_gettime(CLOCK_REALTIME, &pakedTime); // get the time that pizzas are packed
        
        // Print the time that pizza need to be packed
        rc = pthread_mutex_lock(&mutex_order_packed_time);
        if(rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        order_packed_time = pakedTime.tv_sec - customerAppearanceTime.tv_sec;
        
        rc = pthread_mutex_lock(&mutex_lock_screen);
        if(rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        printf("\nΗ παραγγελία με αριθμό %d ετοιμάστηκε σε %f λεπτά.\n\n", *oid, order_packed_time);

        rc = pthread_mutex_unlock(&mutex_lock_screen);
        if(rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
        
        rc = pthread_mutex_unlock(&mutex_order_packed_time);
        if(rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        //Release oven
        rc = pthread_mutex_lock(&mutex_no_oven);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        countAvailableOven += numberOfPizzas;
        pthread_cond_signal(&cond_no_oven);
        rc = pthread_mutex_unlock(&mutex_no_oven);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Release pack employee
        rc = pthread_mutex_lock(&mutex_no_pack_employee);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        packEmployee = 1;
        pthread_cond_signal(&cond_no_pack_employee);
        rc = pthread_mutex_unlock(&mutex_no_pack_employee);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Searching for available deliverers
        rc = pthread_mutex_lock(&mutex_no_deliverer);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        while(numberOfDeliverers<=0){
            rc = pthread_cond_wait(&cond_no_deliverer, &mutex_no_deliverer);
            if (rc != 0) {
                printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
                pthread_exit(&rc);
            }
        }
        printf("Βρέθηκε διανομέας για την παραγγελία με αριθμό %d.\n", *oid);
        --numberOfDeliverers;
        rc = pthread_mutex_unlock(&mutex_no_deliverer);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Start the procedure of delivery
        // generated random number for the delivery time
        int deliveryTime = (rand_r(&alt_seed) % (T_del_high - T_del_low + 1)) + T_del_low;
        sleep(deliveryTime); // Wait until deliverer goes to customer

        clock_gettime(CLOCK_REALTIME, &deliveredTime);

        // Print the time that pizzas need to be deliverd
        rc = pthread_mutex_lock(&mutex_order_delivered_time);
        if(rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        order_delivered_time = deliveredTime.tv_sec - customerAppearanceTime.tv_sec;
        
        rc = pthread_mutex_lock(&mutex_lock_screen);
        if(rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        printf("\nΗ παραγγελία με αριθμό %d παραδόθηκε σε %f λεπτά.\n\n", *oid, order_delivered_time);

        rc = pthread_mutex_unlock(&mutex_lock_screen);
        if(rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
        
        rc = pthread_mutex_unlock(&mutex_order_delivered_time);
        if(rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        sleep(deliveryTime); // Wait until deliverer come back

        //Release deliverer
        rc = pthread_mutex_lock(&mutex_no_deliverer);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        ++numberOfDeliverers;
        pthread_cond_signal(&cond_no_deliverer);
        rc = pthread_mutex_unlock(&mutex_no_deliverer);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        } 

        // Calculate total waiting time of the customer
        rc = pthread_mutex_lock(&mutex_total_waiting_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        rc = pthread_mutex_lock(&mutex_waiting_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        waiting_time = employeeCallTime.tv_sec - customerAppearanceTime.tv_sec;
        total_waiting_time += waiting_time;
        rc = pthread_mutex_unlock(&mutex_total_waiting_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Update max waiting time
        rc = pthread_mutex_lock(&mutex_max_waiting_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        if(waiting_time > max_waiting_time){
            max_waiting_time = waiting_time;
        }

        rc = pthread_mutex_unlock(&mutex_max_waiting_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
        rc = pthread_mutex_unlock(&mutex_waiting_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Calculate total service time of the customer
        rc = pthread_mutex_lock(&mutex_total_service_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        rc = pthread_mutex_lock(&mutex_service_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        service_time = deliveredTime.tv_sec - customerAppearanceTime.tv_sec;
        total_service_time += service_time;
        rc = pthread_mutex_unlock(&mutex_total_service_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Update max service time
        rc = pthread_mutex_lock(&mutex_max_service_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        if(service_time > max_service_time){
            max_service_time = service_time;
        }

        rc = pthread_mutex_unlock(&mutex_max_service_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
        rc = pthread_mutex_unlock(&mutex_service_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Calculate total cold time of the orders
        rc = pthread_mutex_lock(&mutex_total_cold_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        rc = pthread_mutex_lock(&mutex_cold_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        cold_time = deliveredTime.tv_sec - bakedTime.tv_sec;
        total_cold_time += cold_time;
        rc = pthread_mutex_unlock(&mutex_total_cold_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
        rc = pthread_mutex_unlock(&mutex_cold_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        // Update max cold time
        rc = pthread_mutex_lock(&mutex_max_cold_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }

        if(cold_time > max_cold_time){
            max_cold_time = cold_time;
        }

        rc = pthread_mutex_unlock(&mutex_max_cold_time);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
        
        pthread_exit(NULL); 

    } else { // Payment failed and order is canceled
        printf("\nΗ παραγγελία με αριθμό %d απέτυχε.\n",*oid);

        rc = pthread_mutex_lock(&mutex_count_of_fail_orders);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        ++countOfFailOrders;
        rc = pthread_mutex_unlock(&mutex_count_of_fail_orders);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }
        
        //Release tel
        rc = pthread_mutex_lock(&mutex_no_tel);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&rc);
        }
        ++noTel;
        pthread_cond_signal(&cond_no_tel);
        rc = pthread_mutex_unlock(&mutex_no_tel);
        if(rc != 0){
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&rc);
        }

        pthread_exit(NULL); 
    }
}

int main(int argc, char* argv[]){
    // Variables
    noOneCook = N_cook;
    packEmployee = 1;
    countAvailableOven = N_oven;
    costOfPizza = C_pizza;
    noTel = N_tel;
    numberOfDeliverers = N_deliverer;
    countOfSuccessfulOrders = 0;
    countOfFailOrders = 0;
    pCardFail = P_fail;
    int rc;
    order_packed_time = 0;
    order_delivered_time = 0;
    max_cold_time = 0;
    max_waiting_time = 0;
    max_service_time = 0;
    total_waiting_time = 0;
    total_service_time = 0;
    total_cold_time = 0;
    cold_time = 0;
    waiting_time = 0;
    service_time = 0;

    // Check if arguments are passed properly
    if (argc!=3){
        printf("\nThere is a problem with the number of arguments you passed..\n");
        exit(-1);
    }

    int total_customers = atoi(argv[1]);
    if(total_customers <= 0) {
        printf("The number of customers must be a positive number..\n");
        exit(-1);
    }

    seed = atoi(argv[2]);

    printf("\nΚαλώς ορίσατε στην πιτσαρία μας.\n");
    printf("Πρόκειται να εξυπηρετήσουμε %d πελάτες.\n", total_customers);
    printf("Ο σπόρος είναι %d.\n\n", seed);

    
    // initialize threads
    pthread_t *threads = (pthread_t*)malloc(total_customers*sizeof(pthread_t));
    if(threads==NULL){
        printf("Allocating merrory problem of threads.\n");
        exit(-1);
    }

    // initialize order id's
    int oid[total_customers];
    for (int i=0;i<total_customers;++i){
        oid[i] = i + 1;
    }

    for(int i=0; i<total_customers; ++i)
    {
        rc = pthread_create(&threads[i], NULL, &exefunc, (void *)&oid[i]);
        // every next customer call after a random time
        int rand_time = (rand_r(&seed) % (T_order_high - T_order_low + 1)) + T_order_low;
        sleep(rand_time);
        if (rc != 0) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Waiting for threads to be finished
    for(int i=0; i<total_customers; ++i)
    {
        rc = pthread_join(threads[i],NULL);
        if (rc != 0) {
            printf("ERROR: return code from pthread_join() is %d on thread %d\n", rc, oid[i]);
            exit(-1);
        }
    }

    // Print the total income and the count of successful and failed payments
    printf("\nΤα συνολικά έσοδα από τις πωλήσεις είναι: %d €\n", income);
    printf("Το πλήθος επιτυχημένων παραγγελιών είναι: %d\n", countOfSuccessfulOrders);
    printf("Το πλήθος αποτυχημένων παραγγελιών είναι: %d\n", countOfFailOrders);
    //Print total and max waiting time of the customers
    printf("\nΟ μέσος χρόνος αναμονής των πελατών είναι: %f λεπτά\n", total_waiting_time/total_customers);
    printf("Ο μέγιστος χρόνος αναμονής των πελατών είναι: %f λεπτά\n", max_waiting_time);
    //Print total and max service time of the customers
    printf("\nΟ μέσος χρόνος εξυπηρέτησης των πελατών είναι: %f λεπτά\n", total_service_time/total_customers);
    printf("Ο μέγιστος χρόνος εξυπηρέτησης των πελατών είναι: %f λεπτά\n", max_service_time);
    //Print total and max cold time of the orders
    printf("\nΟ μέσος χρόνος κρυώματος των παραγγελιών είναι: %f λεπτά\n", total_cold_time/total_customers);
    printf("Ο μέγιστος χρόνος κρυώματος των παραγγελιών είναι: %f λεπτά\n\n", max_cold_time);


    // Destroy mutexes and conditions
    rc = pthread_mutex_destroy(&mutex_no_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_no_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_no_deliverer);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_no_tel);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_no_pack_employee);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_cond_destroy(&cond_no_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_cond_destroy(&cond_no_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_cond_destroy(&cond_no_deliverer);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_cond_destroy(&cond_no_pack_employee);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_cond_destroy(&cond_no_tel);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_lock_screen);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_income);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_order_packed_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_order_delivered_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_max_waiting_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_waiting_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_total_waiting_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_max_service_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_service_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_total_service_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_max_cold_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_cold_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_total_cold_time);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_count_of_successful_orders);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_mutex_destroy(&mutex_count_of_fail_orders);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }

    free(threads); // release memory of variable threads
    return 0;
}


