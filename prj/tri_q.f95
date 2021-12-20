      MODULE CUSTOMER
      IMPLICIT NONE

!     declaring enter time, service time, exit time,
!     for a customer in each server
      DOUBLE PRECISION en1 , ser1 , ex1 , &
      en2 , ser2 , ex2 , en3 , ser3 , ex3

      END MODULE CUSTOMER

      PROGRAM TRI_QUEUE
      USE CUSTOMER
      IMPLICIT NONE
      INTEGER,PARAMETER :: seed = 86456 , total_customer = 5000000
      DOUBLE PRECISION :: clk = 0. , total_wait_time(3) , &
      total_service_time(3) , total_idle_time(3) , &
      total_spent_time = 0.
      DOUBLE PRECISION :: n_waited(3) , total_server_customer(3)
      INTEGER i
      DOUBLE PRECISION idle1 , idle2, idle3, p_server

      CALL SRAND(seed)

!     init metrics with zero
      DO i=1,3
         total_wait_time(i) = 0.
         total_service_time(i) = 0.
         total_idle_time(i) = 0.
         n_waited(i) = 0
         total_server_customer = 0
      END DO

      CALL RUN_SIMULATION()

      CONTAINS

!     for generating a exponential random number
      DOUBLE PRECISION FUNCTION GENERATE_RANDOM_NUMBER(l)
      DOUBLE PRECISION :: random , l

      random = rand()

      DO WHILE (random == 0)
         random = rand()
      END DO

      GENERATE_RANDOM_NUMBER = -1 * l * LOG(random)
      End FUNCTION GENERATE_RANDOM_NUMBER

      SUBROUTINE RUN_SIMULATION()
      INTEGER :: cus

!     reaching stable state
      DO cus=1,100000
         CALL STEP_SIMULATE(.FALSE.)
      END DO

!     run until reaching total_customer
      DO cus=1,total_customer
         CALL STEP_SIMULATE(.TRUE.)
         !CALL PRINT_CUSTOMER()
      End Do

      CALL PRINT_METRICS()

      END SUBROUTINE RUN_SIMULATION

      SUBROUTINE PRINT_CUSTOMER()

      SELECT CASE (p_server <= 0.4)
      CASE (.TRUE.)
         WRITE (*,800) en1 , ser1 , ex1 , &
         en2 , ser2 , ex2 , '-' , '-' , '-' , &
         p_server , idle1 , idle2 , idle3
      CASE (.FALSE.)
         WRITE (*,801) en1 , ser1 , ex1 , &
         '-' , '-' , '-' , en3 , ser3 , ex3 , &
         p_server , idle1 , idle2 , idle3
      END SELECT

 800  FORMAT (6(F5.2,X),3(A4,X,X),4(F5.2,X))
 801  FORMAT (3(F5.2,X),3(A4,X,X),7(F5.2,X))
      END SUBROUTINE PRINT_CUSTOMER

      SUBROUTINE PRINT_METRICS()
      DOUBLE PRECISION l , es , lq , w , wq , p , R , N

      DO i=1,3
         WRITE (*,901) "server" , i , "metrics"
         WRITE (*,902) "total idle time:" , total_idle_time(i)
         WRITE (*,902) "total service time:" , total_service_time(i)
         WRITE (*,902) "total wait time:" , total_wait_time(i)
         WRITE (*,903) "total waited customers:" , n_waited(i)
         WRITE (*,903) "total server customers:" , total_server_customer(i)

         wq = total_wait_time(i) / total_server_customer(i)
         lq = n_waited(i) / total_server_customer(i)
         es = total_service_time(i) / total_server_customer(i)
         p = 1 - total_idle_time(i) / clk
         l = lq + p
         w = wq + es

         WRITE (*,902) "average customer in server (L):" , &
         l
         WRITE (*,902) "average customer in queue (LQ):" , &
         lq
         WRITE (*,902) "average time spent in server (W):" , &
         w
         WRITE (*,902) "average waiting time in queue (WQ):" , &
         wq
         WRITE (*,902) "average service time (E[S]):" , &
         es
         WRITE (*,902) "productivity (P):" , &
         p
         WRITE (*,*) ""
      END DO

      R = total_spent_time / total_customer
      N = total_customer / clk

      WRITE (*,*) "system metrics"

      WRITE (*,902) "average service time of system (R):" , &
      R
      WRITE (*,902) "average customers in system (N):" , &
      N
      WRITE (*,902) "clock:" , &
      clk

 901  FORMAT (A,X,I1,X,A)
 902  FORMAT (3(X),A,T40,F30.2)
 903  FORMAT (3(X),A,T40,F30.0)
      END SUBROUTINE PRINT_METRICS

      SUBROUTINE STEP_SIMULATE(is_sim)
!     declaring lambda for events
      DOUBLE PRECISION enl1 , serl1, serl2, serl3 , &
      enter_time
      LOGICAL is_sim

      enl1 = 1.
      serl1 = 2.
      serl2 = 4.
      serl3 = 3.

!     server 1
!     enter time
      en1 = GENERATE_RANDOM_NUMBER(enl1) + en1
!     service begin time
      ser1 = MAX(en1, ex1)
!     idle time
      idle1 = ser1 - ex1
!     exit time
      ex1 = ser1 + GENERATE_RANDOM_NUMBER(serl1)


!     generate random number for selecting second hop
      p_server = rand()
      SELECT CASE (p_server <= 0.4)
!     server 2
      CASE (.TRUE.)
         en2 = ex1
         ser2 = MAX(en2, ex2)
         idle2 = ser2 - ex2
         ex2 = ser2 + GENERATE_RANDOM_NUMBER(serl2)

!     server 3
      CASE (.FALSE.)
         en3 = ex1
         ser3 = MAX(en3, ex3)
         idle3 = ser3 - ex3
         ex3 = ser3 + GENERATE_RANDOM_NUMBER(serl3)

      END SELECT

      IF (is_sim) THEN
         CALL UPDATE_METRICS()
      END IF

      END SUBROUTINE STEP_SIMULATE

      SUBROUTINE UPDATE_METRICS()
!     update clk, total_wait_time(3), total_service_time(3),
!     total_idle_time(3), total_spent_time, n_waited(3)

!     server 1
!     update wait if waited
      total_wait_time(1) = total_wait_time(1) + ser1 - en1
!     update service time
      total_service_time(1) = total_service_time(1) + ex1 - ser1
!     update idle time
      total_idle_time(1) = total_idle_time(1) + idle1
!     update served customers
      total_server_customer(1) = total_server_customer(1) + 1

!     update waited customers number
      IF (ser1 > en1) THEN
         n_waited(1) = n_waited(1) + 1
      END IF

      SELECT CASE (p_server <= 0.4)
!     server 2
      CASE (.TRUE.)
         total_wait_time(2) = total_wait_time(2) + ser2 - en2
         total_service_time(2) = total_service_time(2) + ex2 - ser2
         total_idle_time(2) = total_idle_time(2) + idle2
         total_server_customer(2) = total_server_customer(2) + 1

         IF (ser2 > en2) THEN
            n_waited(2) = n_waited(2) + 1
         END IF

         total_spent_time = total_spent_time + ex2 - en1

!     server 3
      CASE (.FALSE.)
         total_wait_time(3) = total_wait_time(3) + ser3 - en3
         total_service_time(3) = total_service_time(3) + ex3 - ser3
         total_idle_time(3) = total_idle_time(3) + idle3
         total_server_customer(3) = total_server_customer(3) + 1

         IF (ser3 > en3) THEN
            n_waited(3) = n_waited(3) + 1
         END IF

         total_spent_time = total_spent_time + ex3 - en1
      END SELECT

!     setting clock 
      clk = MAX(ex2, ex3)

      END SUBROUTINE UPDATE_METRICS

      END PROGRAM TRI_QUEUE
