#ifndef MATRIX_H_
#define MATRIX_H_
#include<cstdio>
#include<iostream>
#include<cstdlib>
#include<math.h>
#include<vector>
using namespace std;


// todo 5: change the class in to a template class
template <class Tp>
class Matrix {
public:
    // default constructor
    // https://en.cppreference.com/w/cpp/language/constructor
    Matrix() {
        rows=0;cols=0;
        data=nullptr;
    }

    // constructor with initilizer list
    Matrix(int row, int col) {/*.todo 1..*/
        rows=row; cols=col;
        data=new Tp[rows*cols];
        for(int i=1;i<=rows;i++){
            for(int j=1;j<=cols;j++){
                data[Map(i,j)]=0;
            }
        }
    }

    // desctructor
    // https://en.cppreference.com/w/cpp/language/destructor
    ~Matrix() {/*.todo 2..*/
        delete data;
    }

    int nrow() const {return rows;}
    int ncol() const {return cols;}

    // operator overloding
    Tp& operator()(int r, int c) {/* todo 3: particular entry of the matrix*/
        Tp& num=data[Map(r,c)];
        return num;
    }
    Tp& operator[](int num) {/* todo 3: particular entry of the matrix*/
        if(num>rows*cols-1){throw out_of_range("Out of Range!");  }
        Tp& ref=data[num];
        return ref;
    }

    Matrix col(int c) {/* todo 4: particular column of the matrix*/
        if(c>cols){ throw out_of_range("Out of Range!"); }
        Matrix Vec(rows,1);
        for(int i=0;i<rows;i++){
            Vec.data[i]=data[Map(i+1,c)];
        }
        return Vec;
    }
    Matrix row(int r) {/* todo 4: particular row of the matrix*/
        if(r>rows){ throw out_of_range("Out of Range!");  }
        Matrix Vec(1,cols);
        for(int i=0;i<cols;i++){
            Vec.data[i]=data[Map(r,i+1)];
        }
        return Vec;
    }
    Matrix submat(int r1, int c1, int r2, int c2) const {/* todo 4: return a sub-matrix specified by the input parameters*/
        if(r1>rows||r1<=0||c1<=0||c1>cols||r2>rows||r2<=0||c2<=0||c2>cols){ cout<<"Out Of Range!"<<endl; abort(); }
        if(r1>r2||c1>c2){throw out_of_range("Out of Range!"); }
        Matrix SubM(r2-r1+1,c2-c1+1);
        for(int r=r1;r<=r2;r++){
            for(int c=c1;c<=c2;c++){
                SubM.data[Map(r-r1+1,c-c1+1)]=data[Map(r,c)];
            }
        }
        return SubM;
    }

    // constant alias
    Matrix& operator= (const Matrix& rhs) {/*.todo 3..*/
        if(data)delete data;
        rows=rhs.rows();
        cols=rhs.cols();
        data=new Tp[rows*cols];  
        for(int i=0;i<rows*cols-1;i++){
            data[i]=rhs.data[i];
        }
        return *this;
    }

    Matrix operator+ (const Matrix& rhs) {/*.todo 3..*/
        if(rhs.cols!=cols||rhs.rows!=rows){
            throw logic_error("Size mismatch");
        }
        Matrix Mt(rows,cols);
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                Mt.data[Map(i+1,j+1)]=rhs.data[Map(i+1,j+1)]+data[Map(i+1,j+1)];
            }
        }
        return Mt;
    }
    Matrix operator- (const Matrix& rhs) {/*.todo 3..*/ 
        if(rhs.cols!=cols||rhs.rows!=rows){
            throw logic_error("Size mismatch");
        }
        Matrix Mt(rows,cols);
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                Mt.data[Map(i+1,j+1)]=data[Map(i+1,j+1)]-rhs.data[Map(i+1,j+1)];
            }
        }
        return Mt;
    }
    Matrix operator* (const Matrix& rhs) {/*.todo 3..*/
        if(rhs.cols!=cols||rhs.rows!=rows){
            throw logic_error("Size mismatch");
        }
        Matrix Mt(rows,cols);
        for(int i=1;i<=rows;i++){
            for(int j=1;j<=cols;j++){
                Mt.data[Map(i,j)]=rhs.data[Map(i,j)]*data[Map(i,j)];
            }
        }
        return Mt;
    }
    Matrix operator/ (const Matrix& rhs) {/*.todo 3..*/ 
        if(rhs.cols!=cols||rhs.rows!=rows){
            throw logic_error("Size mismatch");
        }
        Matrix Mt(rows,cols);
        for(int i=1;i<=rows;i++){
            for(int j=1;j<=cols;j++){
                if(rhs.data[Map(i,j)]==0){
                    throw logic_error("Divide by zero");
                }
                Mt.data[Map(i,j)]=data[Map(i,j)]/rhs.data[Map(i,j)];
            }
        }
        return Mt;
    }

    Matrix& operator+= (const Matrix& rhs) {/*.todo 3..*/
        if(rhs.cols!=cols||rhs.rows!=rows){
            throw logic_error("Size mismatch");
        }
        for(int i=1;i<=rows;i++){
            for(int j=1;j<=cols;j++){
                data[Map(i,j)]+=rhs.data[Map(i,j)];
            }
        }
        return *this;
    }
    Matrix& operator-= (const Matrix& rhs) {/*.todo 3..*/ 
        if(rhs.cols!=cols||rhs.rows!=rows){
            throw logic_error("Size mismatch");
        }
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                data[Map(i+1,j+1)]-=data[Map(i+1,j+1)];
            }
        }
        return *this;
    }
    Matrix& operator*= (const Matrix& rhs) {/*.todo 3..*/ 
        if(rhs.cols!=cols||rhs.rows!=rows){
            cout<<"Size not match!"<<endl;
            abort();
        }
        for(int i=1;i<=rows;i++){
            for(int j=1;j<=cols;j++){
                data[Map(i,j)]*=rhs.data[Map(i,j)];
            }
        }
        return *this;
    }
    Matrix& operator/= (const Matrix& rhs) {/*.todo 3..*/
        if(rhs.cols!=cols||rhs.rows!=rows){
            throw logic_error("Size mismatch");
        }
        for(int i=1;i<=rows;i++){
            for(int j=1;j<=cols;j++){
                if(rhs.data[Map(i,j)]==0){
                    throw logic_error("Divide by zero");
                }
                data[Map(i,j)]/=rhs.data[Map(i,j)];
            }
        }
        return *this;
    }

    Matrix operator+ (double v) {/*.todo 3..*/
        Matrix Mt=*this;
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                Mt.data[Map(i+1,j+1)]+=v;
            }
        }
        return Mt;
    }
    Matrix operator- (double v) {/*.todo 3..*/
        Matrix Mt=*this;
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                Mt.data[Map(i+1,j+1)]-=v;
            }
        }
        return Mt;
    }
    Matrix operator* (double v) {/*.todo 3..*/
        Matrix Mt=*this;
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                Mt.data[Map(i+1,j+1)]*=v;
            }
        }
        return Mt;
    }
    Matrix operator/ (double v) {/*.todo 3..*/
        if(v==0){
            throw logic_error("Divide by zero");
        }
        Matrix Mt=*this;
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                Mt.data[Map(i+1,j+1)]/=v;
            }
        }
        return Mt;
    }

    Matrix& operator+= (double v) {/*.todo 3..*/
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                data[Map(i+1,j+1)]+=v;
            }
        }
        return *this;
    }
    Matrix& operator-= (double v) {/*.todo 3..*/
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                data[Map(i+1,j+1)]-=v;
            }
        }
        return *this;
    }
    Matrix& operator*= (double v) {/*.todo 3..*/
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                data[Map(i+1,j+1)]*=v;
            }
        }
        return *this;
    }
    Matrix& operator/= (double v) {/*.todo 3..*/
        if(v==0){
            throw logic_error("Divide by zero");
        }
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                data[Map(i+1,j+1)]/=v;
            }
        }
        return *this;
    }

    Matrix Id_mat(int order){//create an identity matrix with given order
        Matrix Id(order,order);
        for(int i=0;i<order;i++){
            Id.data[Map(i+1,i+1)]=1;
        }
        return Id;
    }



    void print () const {
        printf("this matrix has size (%d x %d)\n", rows, cols);
        printf("the entries are:\n");
        /* todo 4: print all the entries of the matrix */

        
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                cout.setf(ios::left);
                cout.setf(ios::showpoint);
                cout.precision(5);
                cout.width(15);
                cout<<data[Map(i+1,j+1)];
            }
            cout<<"\n"<<endl;
        }
    }

private:
    int rows, cols;
    Tp *data;
    inline int Map(int r,int c) const {
        if(r>rows||r<=0||c<=0||c>cols) { 
            throw out_of_range("Out of Range!");
        }
        return (r-1)*cols+c-1;
    }
};

// BONUS: write a sparse matrix class
class Node{
public:
    int Row,Col;
    double Data;
    Node* next;
    Node(int row,int col,double data){
        Row=row;Col=col;Data=data;
        next=nullptr;
    }
};
template<typename R>
class SparseMatrix {
private:
    int rows,cols;
    Node** matrix;

public:
    SparseMatrix(int row,int col):rows(row),cols(col){
        matrix=new Node*[row+1];
        for(int i=1;i<=rows;i++){
            matrix[i]=new Node(-1,-1,0);
        }
    }

    ~SparseMatrix(){
        delete matrix;
    }

    void add_element(int r,int c,R v){
        if(v==0)return;

        Node* p=matrix[r];
        while(p->next&&p->next->Col<c){
            p=p->next;
        }
        if(p->next&&p->next->Col==c){
            p->next->Data=v;
        }
        else {
            auto new_node= new Node(r,c,v);
            new_node->next=p->next;
            p->next=new_node;
        }
    }
    
    R operator()(int r,int c) const {//only for visit the node, to change the value of node, use add_element()
        Node* p=matrix[r]->next;
        while(p&&p->Col<c){
            p=p->next;
        }
        if(p&&p->Col==c){
            return p->Data;
        }
        else {
            return 0;
        }
    }

    SparseMatrix operator+(const SparseMatrix& rhs){//the reload of other operators are similar
        if(rows!=rhs.rows||cols!=rhs.cols){
            throw logic_error("Size mismatch");
        }
        SparseMatrix result(rows,cols);
        for(int r=1;r<=rows;r++){
            Node* p1=matrix[r]->next;
            Node* p2=rhs.matrix[r]->next;

            while(p1&&p2){//merge the element at the same location
                if(p1->Col < p2->Col){
                    result.add_element(p1->Row,p1->Col,p1->Data);
                    p1=p1->next;
                }
                else if(p1->Col > p2->Col){
                    result.add_element(p2->Row,p2->Col,p2->Data);
                    p2=p2->next;
                }
                else {
                    result.add_element(p1->Row,p1->Col,p1->Data+p2->Data);
                    p1=p1->next;
                    p2=p2->next;
                }
            }
            //add the rest of the elements
            while(p1){
                result.add_element(p1->Row,p1->Col,p1->Data);
                p1=p1->next;
            }
            while(p2){
                result.add_element(p2->Row,p2->Col,p2->Data);
                p2=p2->next;
            }

        }
        return result;
    }

    void print()const {
        printf("this matrix has size (%d x %d)\n", rows, cols);
        printf("the entries are:\n");
        for(int r=1;r<=rows;r++){
            for(int c=1;c<=cols;c++){
                cout.setf(ios::left);
                cout.setf(ios::showpoint);
                cout.precision(5);
                cout.width(15);
                cout<<operator()(r,c);
            }
            cout<<"\n"<<endl;
        }
    }
};


#endif