#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <math.h>

class Matrix{
    int row, col;
    double **matrix;
public:
    Matrix(int row, int col){
        this->row = row;
        this->col = col;
        matrix = new double*[row];
        for(int i = 0; i < row; i++){
            matrix[i] = new double[col];
            for(int j = 0; j < col; j++){
                matrix[i][j] = 0;
            }
        }
    }

    Matrix(const Matrix &m){
        row = m.row;
        col = m.col;
        matrix = new double*[row];
        for(int i = 0; i < row; i++){
            matrix[i] = new double[col];
            for(int j = 0; j < col; j++){
                matrix[i][j] = m.matrix[i][j];
            }
        }
    }

    ~Matrix(){
        for(int i = 0; i < row; i++){
            delete[] matrix[i];
        }
        delete[] matrix;
    }

    Matrix IMatrix(int row, int col){
        Matrix result(row, col);
        for(int i = 0; i < row; i++){
            result.matrix[i][i] = 1;
        }
        return result;
    }

    void setVal(int row, int col, double val){
        matrix[row][col] = val;
    }

    double getVal(int row, int col){
        return matrix[row][col];
    }

    Matrix operator*(const Matrix &m){
        Matrix result(row, m.col);
        for(int i = 0; i < row; i++){
            for(int j = 0; j < m.col; j++){
                for(int k = 0; k < col; k++){
                    result.matrix[i][j] += matrix[i][k] * m.matrix[k][j];
                }
            }
        }
        return result;
    }

    Matrix operator=(const Matrix &m){
        for(int i = 0; i < row; i++){
            delete[] matrix[i];
        }
        delete[] matrix;
        row = m.row;
        col = m.col;
        matrix = new double*[row];
        for(int i = 0; i < row; i++){
            matrix[i] = new double[col];
            for(int j = 0; j < col; j++){
                matrix[i][j] = m.matrix[i][j];
            }
        }
        return *this;
    }

};
#endif // MATRIX_HPP