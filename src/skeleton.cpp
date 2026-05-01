#include "helpers.hpp"

#include "fgbs.hpp"

#include <cmath>
#include <iostream>
#include <vector>

using namespace fgbs;

int main(int argc, char** argv) {
    skeleton::prelude();

    // DEFINE CONSTANTS

    constexpr usize N = 32;
    constexpr usize CUTOFF_ORDER = 4;
    constexpr usize LAST_STRIDE = 1;

    constexpr uint SQUEEZING_SEED = 25565u;
    constexpr f64 SQUEEZING_MIN = 1.0;
    constexpr f64 SQUEEZING_MAX = 1.5;
    constexpr f64 MODE_MIXING_ANGLE = 0.5;
    
    constexpr usize SAMPLE_COUNT = 10000;
    constexpr uint SAMPLING_SEED = 4598u;

    constexpr usize BOOTSTRAP_COUNT = 100;
    constexpr uint BOOTSTRAP_SEED = 16807u;

    // INIT COVARIANCE MATRIX

    auto squeezing_random = UniformRandomReal<f64>(SQUEEZING_SEED, SQUEEZING_MIN, SQUEEZING_MAX);
    
    Matrix<f64> cov_matrix = Matrix<f64>::Zero(2 * N, 2 * N);
    for (usize i = 0; i < N; ++i) {
        const auto r = squeezing_random();
        cov_matrix(i, i) = std::exp(-2.0 * r);
        cov_matrix(i + N, i + N) = std::exp(2.0 * r);
    }

    Matrix<f64> mode_mixing = identity_matrix<f64>(2 * N);
    const auto c = std::cos(MODE_MIXING_ANGLE);
    const auto s = std::sin(MODE_MIXING_ANGLE);
    for (usize i = 0; i + 1 < N; i += 2) {
        const auto j = i + 1;

        mode_mixing(i, i) = c;
        mode_mixing(i, j) = -s;
        mode_mixing(j, i) = s;
        mode_mixing(j, j) = c;

        mode_mixing(i + N, i + N) = c;
        mode_mixing(i + N, j + N) = -s;
        mode_mixing(j + N, i + N) = s;
        mode_mixing(j + N, j + N) = c;
    }
    
    cov_matrix = mode_mixing * cov_matrix * mode_mixing.transpose();

    // INIT LAYOUTS

    const auto bitstring_layout = generate_bitstring_layout<N>(CUTOFF_ORDER).value();

    const auto cumulant_layout = generate_cumulant_layout<N>(CUTOFF_ORDER).value();

    const auto sublist_layout = generate_sublist_layout<N>(bitstring_layout, LAST_STRIDE).value();

    // INIT TARGETS

    const auto sampling_target = generate_sampling_target<N>(bitstring_layout, cumulant_layout, cov_matrix).value();

    const auto correlator_vec = generate_correlator_vec<N>(bitstring_layout, cov_matrix).value();

    const auto moment_cumulant_vec = generate_moment_cumulant_vec<N>(bitstring_layout, cumulant_layout, correlator_vec).value();

    const auto stats_target = generate_stats_target<N>(bitstring_layout, sublist_layout, moment_cumulant_vec).value();

    // GENERATE SAMPLES

    const auto samples = generate_samples<N>(bitstring_layout, sampling_target, SAMPLE_COUNT, SAMPLING_SEED).value();

    // EVALUATE SAMPLES

    const auto click_stats = compute_sample_click_stats<N>(samples);

    const auto cumulant_stats = compute_sample_cumulant_stats<N>(bitstring_layout, cumulant_layout, sublist_layout, stats_target, samples, BOOTSTRAP_COUNT, BOOTSTRAP_SEED).value();

    f64 mean_click_count = 0.0;
    for (usize click_count = 0, click_probs_len = click_stats.total_click_probs.size(); click_count < click_probs_len; ++click_count) {
        mean_click_count += static_cast<f64>(click_count) * click_stats.total_click_probs[click_count];
    }
    std::cout << "Mean number of clicks: " << mean_click_count << std::endl;

    for (usize order = 1; order <= CUTOFF_ORDER; ++order) {
        const auto index = order - 1;
        std::cout << "Order: " << order << " | Pearson: " << cumulant_stats.pearson_info_vec[index].correlation << " | Spearman: " << cumulant_stats.spearman_info_vec[index].correlation << std::endl;
    }

    return 0;
}
