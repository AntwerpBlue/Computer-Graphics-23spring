#include<random>
#include<chrono>
#include"matrix.h"
#include"Eigen/Dense"
using Eigen::MatrixXd;

int main() {
    Matrix<double> A(3,5), B(3,5);

    // todo 6: fill A anb B with random numbers
    //for(int i=0; i<A.nrow(); i++)
    //    for(int j=0; j<A.ncol(); j++)
    //        A(i, j) = rand();


    // https://blog.csdn.net/qq_45797026/article/details/108536921
    default_random_engine e;
    uniform_real_distribution<double> u(-10,10);
    for(int i=1;i<=A.nrow();i++){
        for(int j=1;j<=A.ncol();j++){
            A(i,j)=u(e);
        }
    }
    for(int i=1;i<=B.nrow();i++){
        for(int j=1;j<=B.ncol();j++){
            B(i,j)=u(e);
        }
    }
    
    // todo 7: benchmark with runtime, using std::chrono
    // https://en.cppreference.com/w/cpp/chrono
    // https://blog.csdn.net/qq_36336522/article/details/79410813

    auto start_time=chrono::system_clock::now();
    Matrix<double> C = A*B;
    auto end_time=chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    double time_in_milliseconds = static_cast<double>(duration.count());

    cout<<'\n'<<"runtime: "<<time_in_milliseconds<<" ms"<<endl;
    cout<<"\n"<<endl;

    A.print();
    B.print();
    C.print();

    // todo 8: use Eigen and compare

    MatrixXd MtA(2,2),MtB(2,2);
    MtA<<1,2,3,4;
    MtB<<1,0,-1,1;

    Matrix<double> selfA(2,2),selfB(2,2);
    selfA(1,1)=1,selfA(1,2)=2,selfA(2,1)=3,selfA(2,2)=4;
    selfB(1,1)=1,selfB(1,2)=0,selfB(2,1)=-1,selfB(2,2)=1;

    cout<<"Using Eigen:"<<endl;
    cout<<"the value of A:\n"<<MtA<<endl;
    cout<<"the value of B:\n"<<MtB<<endl;
    cout<<'\n'<<endl;
    cout<<"Using Self-defined Matrix:"<<endl;
    cout<<"the value of A:"<<endl;
    selfA.print();
    cout<<"the value of B:"<<endl;
    selfB.print();


    //test of SparseMatrix
    
    SparseMatrix<double> M(3,3);
    M.add_element(1,2,2); M.add_element(2,3,3);
    M.add_element(3,1,1);
    SparseMatrix<double> N(3,3);
    N.add_element(1,1,2); N.add_element(1,2,1.5);
    N.add_element(2,3,-3); N.add_element(3,1,-1);

    SparseMatrix<double> P=M+N;
    cout<<"M:"<<endl; M.print();
    cout<<"N:"<<endl; N.print();
    cout<<"P=M+N:"<<endl; P.print();

    system("pause");
    return 0;
}
