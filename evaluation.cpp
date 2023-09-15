#include <array>
#include <cstdint>
#include <bit>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>

#include "types.h"
#include "transpositiontable.h"
#include "drawboard.h"

#include "json.hpp"
using json = nlohmann::json;


bool is_won (position pos)
{
    uint64_t x = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            x |= pos.cross_count[i][j][4];
    return std::popcount(x) > 0;
}


std::array<std::array<float, 552>, 64> linear_m1 = {};
std::array<float, 64> bias_m1 = {};
std::array<std::array<float,  64>, 16> linear_m2 = {};
std::array<float, 16> bias_m2 = {};
std::array<std::array<float,  16>, 16> linear_m3 = {};
std::array<float, 16> bias_m3 = {};
std::array<std::array<float,  16>,  3> linear_m4 = {};
std::array<float,  3> bias_m4 = {};


template<int size_in, int size_out>
void json_to_2d_array(std::array<std::array<float, size_in>, size_out>& linear_array, json linear_json)
{
    if (linear_json.size() != size_out)
        throw std::invalid_argument("NN has wrong size");
    for (int i = 0; i < size_out; i++) {
        if (linear_json[i].size() != size_in)
            throw std::invalid_argument("NN has wrong size");
        for (int j = 0; j < size_in; j++)
             linear_array[i][j] = linear_json[i][j];
    }
}


template<int size>
void json_to_array(std::array<float, size>& bias_array, json bias_json)
{
    if (bias_json.size() != size)
        throw std::invalid_argument("NN has wrong size");
    for (int i = 0; i < size; i++)
        bias_array[i] = bias_json[i];
}


void load_NN_value()
{
    std::ifstream NN_file("NeuralNetwork.json");
    json NN_json = json::parse(NN_file);

    json_to_2d_array<552, 64>(linear_m1, NN_json["linear_relu_stack.0.weight"]);
    json_to_array<64>(bias_m1, NN_json["linear_relu_stack.0.bias"]);
    json_to_2d_array< 64, 16>(linear_m2, NN_json["linear_relu_stack.2.weight"]);
    json_to_array<16>(bias_m2, NN_json["linear_relu_stack.2.bias"]);
    json_to_2d_array< 16, 16>(linear_m3, NN_json["linear_relu_stack.4.weight"]);
    json_to_array<16>(bias_m3, NN_json["linear_relu_stack.4.bias"]);
    json_to_2d_array< 16,  3>(linear_m4, NN_json["linear_relu_stack.6.weight"]);
    json_to_array< 3>(bias_m4, NN_json["linear_relu_stack.6.bias"]);
}


std::array<float, 552> pos_to_NN_input(position pos)
{
    std::array<float, 552> input = {};
    position cf_pos = {};

    int index = 0;
    for (int i = 0; i < 4; i++)
        for (int color = 0; color < 2; color++)
            for (int pieces = 0; pieces < 4; pieces++)
                for (int bit = 0; bit < 42; bit++)
                    if ((cf_pos.cross_count[i][color][0] & (1LL << bit)) != 0) {
                        // TODO: Correct the Neural Network
                        input[index] = float((pos.cross_count[i][color][pieces] >> bit) & 1LL);
                        index++;
                    }
    return input;
}


template<int size_in, int size_out>
std::array<float, size_out> linear(std::array<float, size_in> input, 
        std::array<std::array<float, size_in>, size_out> linear_m, std::array<float, size_out> bias_m)
{
    std::array<float, size_out> output = {};
    for (int i = 0; i < size_out; i++) {
        for (int j = 0; j < size_in; j++) {
            output[i] += input[j] * linear_m[i][j];
        }
        output[i] += bias_m[i];
    }
    return output;
}


template<int size>
std::array<float, size> relu(std::array<float, size> input)
{
    for (int i = 0; i < size; i++)
        if (input[i] < 0)
            input[i] = 0;
    return input;
}


std::array<float, 3> softmax(std::array<float, 3> input)
{
    float denominator = std::exp(input[0]) + std::exp(input[1]) + std::exp(input[2]);

    for (int i = 0; i < 3; i++)
        input[i] = std::exp(input[i]) / denominator * 100;

    return input;
}


wdlm_struct evaluation (position pos)
{
/*    float eval = 0;
    // Here will be the evaluation eventually the net

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 1; k < 5; k++) {
                eval += (std::popcount(pos.cross_count[i][  j][k] & 
                                       pos.cross_count[i][1-j][0]) << (k*2)) * (j*2-1);
            }

    return wdlm_struct{50 + 49 * erff(eval / 100), 0, 50 - 49 * erff(eval / 100), 0};
*/
    // NN
    std::array<float, 552>  input = pos_to_NN_input(pos);
    std::array<float,  64> layer1 = relu<64>(linear<552, 64>( input, linear_m1, bias_m1));
    std::array<float,  16> layer2 = relu<16>(linear< 64, 16>(layer1, linear_m2, bias_m2));
    std::array<float,  16> layer3 = relu<16>(linear< 16, 16>(layer2, linear_m3, bias_m3));
    std::array<float,   3> output =  softmax(linear< 16,  3>(layer3, linear_m4, bias_m4));

    return wdlm_struct{output[0], output[1], output[2], 0};
}
