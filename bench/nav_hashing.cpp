#include <benchmark/benchmark.h>
#include <nav/nav_hashmap.hpp>
#include <utility>

using benchmark::State;

template <class Fn>
void hash_string(State& state, Fn func, std::string_view sv) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(sv);
        auto result = func(sv);
        benchmark::DoNotOptimize(result);
    }
}

using nav::detail::hash_sv;
constexpr auto std_hash_sv = std::hash<std::string_view> {};

constexpr std::string_view source = "0123456789abcdef0123456789abcdef";

BENCHMARK_CAPTURE(hash_string, (hash, 0), hash_sv, source.substr(0, 0));
BENCHMARK_CAPTURE(hash_string, (hash, 1), hash_sv, source.substr(0, 1));
BENCHMARK_CAPTURE(hash_string, (hash, 2), hash_sv, source.substr(0, 2));
BENCHMARK_CAPTURE(hash_string, (hash, 3), hash_sv, source.substr(0, 3));
BENCHMARK_CAPTURE(hash_string, (hash, 4), hash_sv, source.substr(0, 4));
BENCHMARK_CAPTURE(hash_string, (hash, 5), hash_sv, source.substr(0, 5));
BENCHMARK_CAPTURE(hash_string, (hash, 6), hash_sv, source.substr(0, 6));
BENCHMARK_CAPTURE(hash_string, (hash, 7), hash_sv, source.substr(0, 7));
BENCHMARK_CAPTURE(hash_string, (hash, 8), hash_sv, source.substr(0, 8));
BENCHMARK_CAPTURE(hash_string, (hash, 9), hash_sv, source.substr(0, 9));
BENCHMARK_CAPTURE(hash_string, (hash, 10), hash_sv, source.substr(0, 10));
BENCHMARK_CAPTURE(hash_string, (hash, 11), hash_sv, source.substr(0, 11));
BENCHMARK_CAPTURE(hash_string, (hash, 12), hash_sv, source.substr(0, 12));
BENCHMARK_CAPTURE(hash_string, (hash, 13), hash_sv, source.substr(0, 13));
BENCHMARK_CAPTURE(hash_string, (hash, 14), hash_sv, source.substr(0, 14));
BENCHMARK_CAPTURE(hash_string, (hash, 15), hash_sv, source.substr(0, 15));
BENCHMARK_CAPTURE(hash_string, (hash, 16), hash_sv, source.substr(0, 16));


BENCHMARK_CAPTURE(hash_string, (std_hash, 0), std_hash_sv, source.substr(0, 0));
BENCHMARK_CAPTURE(hash_string, (std_hash, 1), std_hash_sv, source.substr(0, 1));
BENCHMARK_CAPTURE(hash_string, (std_hash, 2), std_hash_sv, source.substr(0, 2));
BENCHMARK_CAPTURE(hash_string, (std_hash, 3), std_hash_sv, source.substr(0, 3));
BENCHMARK_CAPTURE(hash_string, (std_hash, 4), std_hash_sv, source.substr(0, 4));
BENCHMARK_CAPTURE(hash_string, (std_hash, 5), std_hash_sv, source.substr(0, 5));
BENCHMARK_CAPTURE(hash_string, (std_hash, 6), std_hash_sv, source.substr(0, 6));
BENCHMARK_CAPTURE(hash_string, (std_hash, 7), std_hash_sv, source.substr(0, 7));
BENCHMARK_CAPTURE(hash_string, (std_hash, 8), std_hash_sv, source.substr(0, 8));
BENCHMARK_CAPTURE(hash_string, (std_hash, 9), std_hash_sv, source.substr(0, 9));
BENCHMARK_CAPTURE(hash_string, (std_hash, 10), std_hash_sv, source.substr(0, 10));
BENCHMARK_CAPTURE(hash_string, (std_hash, 11), std_hash_sv, source.substr(0, 11));
BENCHMARK_CAPTURE(hash_string, (std_hash, 12), std_hash_sv, source.substr(0, 12));
BENCHMARK_CAPTURE(hash_string, (std_hash, 13), std_hash_sv, source.substr(0, 13));
BENCHMARK_CAPTURE(hash_string, (std_hash, 14), std_hash_sv, source.substr(0, 14));
BENCHMARK_CAPTURE(hash_string, (std_hash, 15), std_hash_sv, source.substr(0, 15));
BENCHMARK_CAPTURE(hash_string, (std_hash, 16), std_hash_sv, source.substr(0, 16));
