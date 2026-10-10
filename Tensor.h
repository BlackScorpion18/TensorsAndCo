//
// Created by Guilherme Martins on 24/09/2026.
//

#ifndef TENSORSANDCO_TENSOR_H
#define TENSORSANDCO_TENSOR_H
#include <algorithm>
#include <stdexcept>
#include <vector>

#endif //TENSORSANDCO_TENSOR_H

class Tensor {
    protected:
        std::vector<int> dimensions;
        std::vector<float> data;

        /**
         * returns a "pure" index - that is, an index that actually points to data's location in the vector -
         * based on a vector of indices. the tensor is ZERO-INDEXED.
         * @param indices the vector of indices.
         * @return the "pure" index.
         */
        [[nodiscard]] int indexVectorToPure(const std::vector<int>& indices) const {
            if (indices.size() != this->dimensions.size()) {
                throw std::invalid_argument(
                    "Indices dimension count (" + std::to_string(indices.size()) +
                    ") does not match tensor rank (" + std::to_string(this->dimensions.size()) + ")."
                );
            }

            for (int i = 0; i < indices.size(); i++) {
                if (indices.at(i) >= this->dimensions.at(i)) throw std::invalid_argument(
                    "Index in the " + std::to_string(i) + "th dimension is exceeds the tensor's " +
                    "length in that dimension: index is " + std::to_string(indices.at(i)) + ", max index is "
                    + std::to_string(this->dimensions.at(i))
                    );

                if (indices.at(i) < 0) throw std::invalid_argument(
                    "Index specified for the " + std::to_string(i) + "th dimension is less than 0"
                    );
            }

            // getting the actual "pure" index for the data vector.
            int pureDataIndex = 0;
            int currentMultiplier = 1;
            // this is set to 1 initially to calculate the first "index" offset.
            int lastTensorDimensionLength = 1;
            /*
            initially i thought about iterating through the indices in reverse, because of how i had first thought
            this out with pen and paper. i then realized that i can actually iterate normally after all: i just have
            to keep consistency whenever i do this.
            */
            for (int i = 0; i < indices.size(); i++) {
                // we first multiply the multiplier by the previous dimension's length
                currentMultiplier *= lastTensorDimensionLength;
                // then, add the offset based on the specified index
                pureDataIndex += indices.at(i) * currentMultiplier;
                // finally, we update the last dimension's length to be used in the next iteration of the loop
                lastTensorDimensionLength = this->dimensions.at(i);
            }

            return pureDataIndex;
        }

        /**
         * returns a vector of indices based on a "pure" index
         * - that is, an index that actually points to data's location in the vector. the tensor is ZERO-INDEXED.
         * @param pureIndex the pure index.
         * @return the vector of indices.
         */
        [[nodiscard]] std::vector<int> pureIndexToVector(int pureIndex) const {
            if (pureIndex < 0) throw std::invalid_argument("Pure index cannot be negative");
            if (pureIndex >= this->getSize()) throw std::invalid_argument(
                "Pure index cannot be larger than the tensor size"
                );

            std::vector<int> indices;
            indices.reserve(this->dimensions.size());

            int currentStride = 1;
            for (int dim : this->dimensions) {
                indices.push_back((pureIndex / currentStride) % dim);
                currentStride *= dim;
            }

            return indices;
        }

    public:
        ~Tensor() = default;

        explicit Tensor(const std::vector<int>& dimensions) {

            if (std::ranges::any_of(dimensions.begin(), dimensions.end(),
            [](int i) {return i <= 0}
            )) throw std::invalid_argument("Tensor dimensions must not be null nor negative");

            this->dimensions = dimensions;
            this->data.resize(this->getSize());
            this->fillZero();
        }

        /**
        * @return the number of data entries in the tensor
        */
        [[nodiscard]] int getSize() const {
            int size = 1;
            for (int dimension : this->dimensions) {
                size *= dimension;
            }
            return size;
        }

        /**
         * @return the order of the tensor (how many dimensions it has)
         */
        [[nodiscard]] int getOrder() const {
            return static_cast<int>(this->dimensions.size());
        }

        /**
         * sets all data values of the tensor to a certain value.
         */
        void fillValue(const float value) {
            std::fill(this->data.begin(), this->data.end(), value);
        }

        /**
         * sets all data values of the tensor to 0.
         */
        void fillZero() {
            fillValue(0);
        }

        /**
         * adds two tensors together, with the result being added to this tensor.
         * tensors must be of the same order and dimensions
         * @param b the tensor to be added to this tensor.
         */
        void tensorAdd(Tensor b) {

            for (int i = 0; i < this->getOrder(); i++) {
                if (this->dimensions.at(i) != b.dimensions.at(i)) throw std::invalid_argument(
                    "Tensors being added must have the same dimensions"
                    );
            }

            for (int i = 0; i < this->getSize(); i++) {
                this->data.at(i) += b.data.at(i);
            }  if (this->getOrder() != b.getOrder()) throw std::invalid_argument(
                "Tensors being added must be of equal order (first tensor's order is "
                + std::to_string(this->getOrder()) + ", second tensor's order is " + std::to_string(b.getOrder())
                );
        }

        /**
         * subtracts the input tensor from this tensor, with this tensor storing the result.
         * tensors must be of the same order and dimensions
         * @param b the tensor to be subtracted from this tensor.
         */
        void tensorSubtract(const Tensor& b) {

            for (int i = 0; i < this->getOrder(); i++) {
                if (this->dimensions.at(i) != b.dimensions.at(i)) throw std::invalid_argument(
                    "Tensors being added must have the same dimensions"
                    );
            }

            for (int i = 0; i < this->getSize(); i++) {
                this->data.at(i) -= b.data.at(i);
            }  if (this->getOrder() != b.getOrder()) throw std::invalid_argument(
                "Tensors being added must be of equal order (first tensor's order is "
                + std::to_string(this->getOrder()) + ", second tensor's order is " + std::to_string(b.getOrder())
                );
        }

        /**
         * gets a value at the index of the tensor. the tensor is ZERO-INDEXED.
         * @param indices a vector of the indices of the tensor to get the value from
         * @return the value at the specified indices
         */
         [[nodiscard]] float getVal(const std::vector<int>& indices) const {
            if (this->getOrder() != indices.size()) throw std::invalid_argument(
                "indices passed in for lookup must match the tensor's order (tensor's order: "
                + std::to_string(this->getOrder()) + ", number of indices: " + std::to_string(indices.size())
                );

            return this->data.at(this->indexVectorToPure(indices));
        }

        /**
         * sets a value at the index of the tensor. the tensor is ZERO-INDEXED.
         * @param value the value to be set at the index
         * @param indices a vector of the indices of the tensor whose value is to be set
         */
        void setVal(float value, const std::vector<int>& indices) {
            if (this->getOrder() != indices.size()) throw std::invalid_argument(
            "indices passed in for lookup must match the tensor's order (tensor's order: "
            + std::to_string(this->getOrder()) + ", number of indices: " + std::to_string(indices.size())
            );

            this->data.at(this->indexVectorToPure(indices)) = value;
        }

        /**
         * scales the tensor by a certain number.
         * @param scalar the scalar to multiply the tensor's data by.
         */
        void multByScalar(float scalar) {
            for (int i = 0; i < this->getSize(); i++) {
                this->data.at(i) *= scalar;
            }
        }

};