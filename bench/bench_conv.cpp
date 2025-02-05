// MIT License
// Copyright (c) 2022 - 傅莘莘
// Source URL: https://github.com/zjhellofss/KuiperInfer
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Created by fushenshen on 2023/3/15.

#include <benchmark/benchmark.h>
#include "../source/layer/details/convolution.hpp"
#include "../source/layer/details/winograd.hpp"
#include "runtime/runtime_ir.hpp"

static void BM_Convolutionk3x3s1x1(benchmark::State& state) {
  using namespace kuiper_infer;

  uint32_t kernel_count = state.range(0);
  uint32_t channels = state.range(1);
  uint32_t rows = state.range(2);
  uint32_t cols = state.range(3);

  sftensor input = std::make_shared<ftensor>(channels, rows, cols);
  input->Fill(1.f);

  std::vector<sftensor> weights(kernel_count);
  for (uint32_t k = 0; k < kernel_count; ++k) {
    sftensor weight = std::make_shared<ftensor>(channels, 3, 3);
    weight->Rand();
    weights.at(k) = weight;
  }

  std::vector<sftensor> outputs(1);
  std::vector<sftensor> inputs;
  inputs.push_back(input);
  ConvolutionLayer conv_layer(ConvType::OpConv, kernel_count, channels, 3, 3, 0,
                              0, 1, 1, 1, false);
  conv_layer.set_weights(weights);
  for (auto _ : state) {
    conv_layer.Forward(inputs, outputs);
  }
}

BENCHMARK(BM_Convolutionk3x3s1x1)
    ->Args({32, 3, 320, 320})
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Convolutionk3x3s1x1)
    ->Args({64, 32, 160, 160})
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Convolutionk3x3s1x1)
    ->Args({128, 64, 80, 80})
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Convolutionk3x3s1x1)
    ->Args({256, 128, 40, 40})
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Convolutionk3x3s1x1)
    ->Args({512, 256, 20, 20})
    ->Unit(benchmark::kMillisecond);

static void BM_DeConvolutionk2x2s2x2(benchmark::State& state) {
  using namespace kuiper_infer;

  uint32_t kernel_count = state.range(0);
  uint32_t channels = state.range(1);
  uint32_t rows = state.range(2);
  uint32_t cols = state.range(3);
  uint32_t batch = state.range(4);

  sftensor input = std::make_shared<ftensor>(channels, rows, cols);
  input->Fill(1.f);

  std::vector<float> weight_values(kernel_count * channels * 2 * 2);
  for (uint32_t k = 0; k < kernel_count * channels * 2 * 2; ++k) {
    weight_values.push_back(float(k % 31));
  }

  std::vector<sftensor> outputs(batch);
  std::vector<sftensor> inputs;
  for (uint32_t i = 0; i < batch; ++i) {
    inputs.push_back(input);
  }
  ConvolutionLayer conv_layer(ConvType::OpDeconv, kernel_count, channels, 3, 3,
                              0, 0, 2, 2, 1, false);
  conv_layer.set_weights(weight_values);
  for (auto _ : state) {
    conv_layer.Forward(inputs, outputs);
  }
}

BENCHMARK(BM_DeConvolutionk2x2s2x2)
    ->Args({64, 128, 192, 192, 1})
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_DeConvolutionk2x2s2x2)
    ->Args({128, 256, 96, 96, 1})
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_DeConvolutionk2x2s2x2)
    ->Args({256, 512, 48, 48, 1})
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_DeConvolutionk2x2s2x2)
    ->Args({512, 1024, 24, 24, 1})
    ->Unit(benchmark::kMillisecond);