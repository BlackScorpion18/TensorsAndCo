//
// Created by Guilherme Martins on 08/10/2026.
//

#ifndef TENSORSANDCO_MATRIX_H
#define TENSORSANDCO_MATRIX_H
#include <bits/error_constants.h>

#include "Tensor.h"


class Matrix : public Tensor {
    private:


    public:
        ~Matrix() = default;

        explicit Matrix(int rows, int cols) : Tensor({rows, cols}) {

        }

        /**
         * @return true if the matrix is square (rows == cols), false otherwise
         */
        [[nodiscard]] bool isSquare() const {
            return this->dimensions.at(0) == this->dimensions.at(1);
        }

        /**
         * @return the number of rows of the matrix
         */
        [[nodiscard]] int getRows() const {
            return this->dimensions.at(0);
        }

        /**
         * @return the number of cols of the matrix
         */
        [[nodiscard]] int getCols() const {
            return this->dimensions.at(1);
        }

        /**
         * turns the matrix into an identity matrix.
         * the matrix MUST be square.
         */
        void initIdentity() {
            if (!this->isSquare()) throw std::invalid_argument(
                "Matrix must be square in order to be made into an identity matrix"
                );

            this->fillZero();
            for (int i = 0; i < this->dimensions.at(0); i++) {
                this->setVal(1, {i, i});
            }
        }

        [[nodiscard]] Matrix* multiply(const Matrix& b) const {
            auto* result = new Matrix(this->getRows(), b.getCols());

            for (int i = 0; i < this->getRows(); i++) {
                for (int j = 0; j < b.getCols(); j++) {
                    float sum = 0;
                    for (int k = 0; k < this->getCols(); k++) {
                        sum += this->getVal({i, k}) * b.getVal({k ,j})
                    }
                    result->setVal(sum, {i, j})
                }
            }

            return result;
        }

        /**
         * returns the determinant of the matrix. the matrix must be square.
         * @return the determinant of the matrix
         */
        [[nodiscard]] float getDeterminant() const {
            if (!this->isSquare()) throw std::invalid_argument(
                "Matrix must be square in order to compute the determinant"
                );

            if (this->getRows() == 1) return this->getVal({0, 0})

            if (this->getRows() == 2) {
                return (this->getVal({0, 0}) * this->getVal({1, 1}))
                    - (this->getVal({0, 1}) * this->getVal({1, 0}));
            }

            if (this->getRows() == 3) {
                return (this->getVal({0, 0}) * this->getVal({1, 1}) * this->getVal({2, 2}))
                + (this->getVal({0, 1}) * this->getVal({1, 2}) * this->getVal({2, 0}))
                + (this->getVal({0, 2}) * this->getVal({1, 0}) * this->getVal({2, 1}))
                - (this->getVal({0, 2}) * this->getVal({1, 1}) * this->getVal({2, 0}))
                - (this->getVal({0, 1}) * this->getVal({1, 0}) * this->getVal({2, 2}))
                - (this->getVal({0, 0}) * this->getVal({1, 2}) * this->getVal({2, 1}));
            }

            // using the Bareiss algorithm
            if (this->getRows() >= 4) {
                int previousPivot = 1;
                int sign = 1
                const int n = this->getRows()
                Matrix mat = *this;

                for (int k = 0; k < n - 2; k++) {
                    int pivotRow = k;
                    while (pivotRow < n && this->getVal({pivotRow, k})) {
                        pivotRow++;
                    }

                    if pivotRow == n return 0;

                    if (pivotRow != k) {
                        // row swapping
                        for (int currCol = 0; currCol < n; currCol++) {
                            const float temp = mat.getVal({k, currCol});
                            mat.setVal(mat.getVal(pivotRow, currCol), {k, currCol});
                            mat.setVal(temp, {pivotRow, currCol});
                        }
                    }
                }
            }

        }
};


#endif //TENSORSANDCO_MATRIX_H
