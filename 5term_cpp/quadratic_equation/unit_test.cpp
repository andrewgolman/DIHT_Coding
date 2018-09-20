#include "solve_equation.hpp"
#include "../test_framework.hpp"
#include <iostream>
#include <vector>

TEST(constant_no_roots) {
    assert(solve_quadratic_equation(0., 0., 1.) == std::vector<double>());
}

TEST(inf_roots) {
    assert(solve_quadratic_equation(0., 0., 0.) == SE_INF_ROOTS<double>);
}

TEST(linear_1) {
    assert(solve_quadratic_equation(0, 1, 0) == std::vector<int>({0}));
}

TEST(linear_2) {
    assert(solve_quadratic_equation(0., -1., 2.) == std::vector<double>({2.}));
}

TEST(quad_no_roots) {
    assert(solve_quadratic_equation(1., 2., 3.) == std::vector<double>({}));
}

TEST(quad_one_root) {
    assert(solve_quadratic_equation(-1., -2., -1.) == std::vector<double>({-1}));
}

TEST(quad_two_roots) {
    assert(solve_quadratic_equation(1., -7., 12.) == std::vector<double>({3, 4}));
}

TEST(quad_bigger) {
    assert(solve_quadratic_equation(10000., -20000., 10000.) == std::vector<double>({1}));
}

RUN_ALL_TESTS()
