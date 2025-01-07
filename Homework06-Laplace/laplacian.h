#pragma once
#include <vector>

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <cmath>

template<class MatF, class MatI> 
auto Laplacian(const MatF &X, const MatI &T, int mode)//X--vertices; T--Topology
{
    auto Cot = [X](int i, int j,int k) {
        Eigen::Vector3f ki=X.row(i)-X.row(k);
        Eigen::Vector3f kj=X.row(j)-X.row(k);
        float cosVal = ki.dot(kj) / (ki.norm() * kj.norm());
        float angle = acos(cosVal);
        return 1/tanf(angle);
    };

    if (mode == 1) {
        std::vector<Eigen::Triplet<float>> ijv_A, ijv_id;
        int n_pt = X.rows();
        // TODO 1: compute ijv triplet for the sparse Laplacian
        for (int i = 0; i < T.rows(); i++) {
            auto temp = T.block(i, 0, 1, 3);
            for (int j = 0; j < 3; j++) {
                ijv_A.push_back(Eigen::Triplet<float>(temp(0, j), temp(0, (j + 1) % 3), 1.0));
                ijv_A.push_back(Eigen::Triplet<float>(temp(0, (j + 1) % 3), temp(0, j), 1.0));//ijv_A stores the adjacent mat 
            }
        }
        Eigen::SparseMatrix<float, Eigen::ColMajor> A(n_pt, n_pt);
        A.setFromTriplets(ijv_A.cbegin(), ijv_A.cend());
        for (int i = 0; i < n_pt; i++) {
            ijv_id.push_back(Eigen::Triplet<float>(i, i, 1.0));
            A.row(i) /= A.row(i).sum();
        }
        Eigen::SparseMatrix<float, Eigen::ColMajor> Id(n_pt, n_pt);
        Id.setFromTriplets(ijv_id.cbegin(), ijv_id.cend());

        Eigen::SparseMatrix<float, Eigen::ColMajor> L = Id - A;

        return L;
    }
    else {
        int n_pt = X.rows();
        std::vector<Eigen::Triplet<float>> ijv_id;
        Eigen::SparseMatrix<float,Eigen::ColMajor> Cot_Mat(n_pt,n_pt);
        for (int i = 0; i < T.rows(); i++) {
            auto temp = T.block(i, 0, 1, 3);
            for (int j = 0; j < 3; j++) {
                Cot_Mat.coeffRef(temp(0, j), temp(0, (j + 1) % 3)) += Cot(temp(0, j), temp(0, (j + 1) % 3), temp(0, (j + 2) % 3)) / 2;
                Cot_Mat.coeffRef(temp(0, (j+2)%3), temp(0, (j+1)%3)) += Cot(temp(0, (j+2)%3), temp(0, (j + 1) % 3), temp(0, j))/2;
            }
        }
        for (int i = 0; i < n_pt; i++) {
            ijv_id.push_back(Eigen::Triplet<float>(i, i, 1.0));
            Cot_Mat.row(i) /= Cot_Mat.row(i).sum();
        }
        Eigen::SparseMatrix<float, Eigen::ColMajor> Id(n_pt, n_pt);
        Id.setFromTriplets(ijv_id.cbegin(), ijv_id.cend());
        Eigen::SparseMatrix<float, Eigen::ColMajor> L(n_pt, n_pt);
        L = Id - Cot_Mat;
        return L;
    }
}
