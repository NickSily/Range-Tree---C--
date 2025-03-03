#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>
#include <set>
#include <algorithm>
#include "../src/RangeTree.h"

// Simple test framework
#define TEST(name) void name()
#define ASSERT_TRUE(condition)                                                                                                 \
    if (!(condition))                                                                                                          \
    {                                                                                                                          \
        test_file << "TEST FAILED: " << __func__ << " Line " << __LINE__ << ": " << #condition << " is not true" << std::endl; \
        failed_tests++;                                                                                                        \
        return;                                                                                                                \
    }                                                                                                                          \
    else                                                                                                                       \
    {                                                                                                                          \
        passed_assertions++;                                                                                                   \
    }

#define ASSERT_FALSE(condition)                                                                                                 \
    if ((condition))                                                                                                            \
    {                                                                                                                           \
        test_file << "TEST FAILED: " << __func__ << " Line " << __LINE__ << ": " << #condition << " is not false" << std::endl; \
        failed_tests++;                                                                                                         \
        return;                                                                                                                 \
    }                                                                                                                           \
    else                                                                                                                        \
    {                                                                                                                           \
        passed_assertions++;                                                                                                    \
    }

#define ASSERT_EQUAL(a, b)                                                                                                                                  \
    if ((a) != (b))                                                                                                                                         \
    {                                                                                                                                                       \
        test_file << "TEST FAILED: " << __func__ << " Line " << __LINE__ << ": " << #a << " != " << #b << " (" << (a) << " != " << (b) << ")" << std::endl; \
        failed_tests++;                                                                                                                                     \
        return;                                                                                                                                             \
    }                                                                                                                                                       \
    else                                                                                                                                                    \
    {                                                                                                                                                       \
        passed_assertions++;                                                                                                                                \
    }

#define RUN_TEST(name)                                          \
    {                                                           \
        test_file << "Running " << #name << "..." << std::endl; \
        name();                                                 \
        if (failed_tests == old_failed_tests)                   \
        {                                                       \
            test_file << "PASSED" << std::endl                  \
                      << std::endl;                             \
            passed_tests++;                                     \
        }                                                       \
        else                                                    \
        {                                                       \
            old_failed_tests = failed_tests;                    \
        }                                                       \
        total_tests++;                                          \
    }

std::ofstream test_file;
int failed_tests = 0;
int old_failed_tests = 0;
int passed_tests = 0;
int total_tests = 0;
int passed_assertions = 0;

// Helper functions
template <typename T>
bool vectorsEqual(const std::vector<T> &a, const std::vector<T> &b)
{
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

template <typename T>
bool vectorContainsPoint(const std::vector<std::vector<T>> &vectors, const std::vector<T> &point)
{
    for (const auto &vec : vectors)
    {
        if (vectorsEqual(vec, point))
            return true;
    }
    return false;
}

// Count distinct points in a result set (handling potential duplicates)
template <typename T>
size_t countDistinctPoints(const std::vector<std::vector<T>> &points)
{
    std::set<std::vector<T>> distinct_points;
    for (const auto &point : points)
    {
        distinct_points.insert(point);
    }
    return distinct_points.size();
}

// Manually check if a point is in a range
template <typename T>
bool isPointInRange(const std::vector<T> &point, const std::vector<T> &low, const std::vector<T> &high)
{
    if (point.size() != low.size() || point.size() != high.size())
        return false;

    for (size_t i = 0; i < point.size(); ++i)
    {
        if (point[i] < low[i] || point[i] > high[i])
            return false;
    }
    return true;
}

// Tests for an empty Range Tree
TEST(test_empty_tree)
{
    std::vector<std::vector<int>> empty_points;
    RangeTree<int, 2> tree(empty_points);

    // Test range search on empty tree
    auto results = tree.rangeSearch({0, 0}, {10, 10});
    ASSERT_EQUAL(results.size(), 0);

    // Test point search on empty tree
    ASSERT_FALSE(tree.search({3, 6}));
}

// Tests for 1D specialization of Range Tree
TEST(test_1d_range_tree)
{
    std::vector<std::vector<int>> points_1d = {
        {3}, {7}, {1}, {9}, {5}, {2}, {8}, {4}, {6}};

    RangeTree<int, 1> tree(points_1d);

    // Test range queries
    auto results = tree.rangeSearch({3}, {7});
    ASSERT_EQUAL(results.size(), 5); // Should find points 3,4,5,6,7

    results = tree.rangeSearch({1}, {1});
    ASSERT_EQUAL(results.size(), 1); // Should find point 1

    results = tree.rangeSearch({0}, {0});
    ASSERT_EQUAL(results.size(), 0); // Should find no points

    // Test point search
    ASSERT_TRUE(tree.search({5}));
    ASSERT_FALSE(tree.search({10}));
}

// Tests for building and basic functions of 2D Range Tree
TEST(test_2d_range_tree_basic)
{
    std::vector<std::vector<int>> points_2d = {
        {3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}, {2, 7}, {10, 19}};

    RangeTree<int, 2> tree(points_2d);

    // Test point search
    ASSERT_TRUE(tree.search({3, 6}));
    ASSERT_TRUE(tree.search({17, 15}));
    ASSERT_TRUE(tree.search({9, 1}));

    ASSERT_FALSE(tree.search({4, 6}));
    ASSERT_FALSE(tree.search({17, 16}));
    ASSERT_FALSE(tree.search({20, 20}));

    // Test exact point range search
    auto results = tree.rangeSearch({3, 6}, {3, 6});
    ASSERT_EQUAL(results.size(), 1);
    ASSERT_TRUE(vectorContainsPoint(results, {3, 6}));

    // Test non-existent point range search
    results = tree.rangeSearch({4, 6}, {4, 6});
    ASSERT_EQUAL(results.size(), 0);
}

// Test for range queries in 2D that should return multiple points
TEST(test_2d_range_queries)
{
    std::vector<std::vector<int>> points_2d = {
        {3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}, {2, 7}, {10, 19}, {14, 11}, {8, 7}, {15, 5}, {4, 8}, {11, 9}, {16, 3}, {5, 14}};

    RangeTree<int, 2> tree(points_2d);

    // Test mid-range query
    auto results = tree.rangeSearch({5, 5}, {15, 15});

    // Manual verification of which points should be in this range
    std::vector<std::vector<int>> expected_points;
    for (const auto &point : points_2d)
    {
        if (isPointInRange(point, {5, 5}, {15, 15}))
        {
            expected_points.push_back(point);
        }
    }

    ASSERT_EQUAL(countDistinctPoints(results), countDistinctPoints(expected_points));

    // Verify each expected point is in the results
    for (const auto &point : expected_points)
    {
        ASSERT_TRUE(vectorContainsPoint(results, point));
    }

    // Test full range query
    results = tree.rangeSearch({0, 0}, {20, 20});
    ASSERT_EQUAL(results.size(), points_2d.size());

    // Test empty range query
    results = tree.rangeSearch({100, 100}, {200, 200});
    ASSERT_EQUAL(results.size(), 0);
}

// Tests for boundary conditions in range queries
TEST(test_range_boundary_conditions)
{
    std::vector<std::vector<int>> points_2d = {
        {5, 5}, {10, 10}, {15, 15}};

    RangeTree<int, 2> tree(points_2d);

    // Test inclusive boundaries
    auto results = tree.rangeSearch({5, 5}, {15, 15});
    ASSERT_EQUAL(results.size(), 3);

    // Test exclusive lower boundary
    results = tree.rangeSearch({6, 5}, {15, 15});
    ASSERT_EQUAL(results.size(), 2);
    ASSERT_FALSE(vectorContainsPoint(results, {5, 5}));

    // Test exclusive upper boundary
    results = tree.rangeSearch({5, 5}, {14, 15});
    ASSERT_EQUAL(results.size(), 2);
    ASSERT_FALSE(vectorContainsPoint(results, {15, 15}));

    // Test boundary that's between points
    results = tree.rangeSearch({6, 6}, {14, 14});
    ASSERT_EQUAL(results.size(), 1);
    ASSERT_TRUE(vectorContainsPoint(results, {10, 10}));
}

// Tests for 3D Range Tree
TEST(test_3d_range_tree)
{
    std::vector<std::vector<int>> points_3d = {
        {3, 6, 2}, {17, 15, 9}, {13, 15, 5}, {6, 12, 1}, {9, 1, 7}, {2, 7, 3}, {10, 19, 8}, {14, 11, 4}, {8, 7, 6}, {15, 5, 10}};

    RangeTree<int, 3> tree(points_3d);

    // Test point search
    ASSERT_TRUE(tree.search({3, 6, 2}));
    ASSERT_TRUE(tree.search({15, 5, 10}));
    ASSERT_FALSE(tree.search({3, 6, 3}));

    // Test range query
    auto results = tree.rangeSearch({5, 5, 3}, {15, 15, 8});

    // Manual verification
    std::vector<std::vector<int>> expected_points;
    for (const auto &point : points_3d)
    {
        if (isPointInRange(point, {5, 5, 3}, {15, 15, 8}))
        {
            expected_points.push_back(point);
        }
    }

    ASSERT_EQUAL(countDistinctPoints(results), countDistinctPoints(expected_points));

    // Verify each expected point is in the results
    for (const auto &point : expected_points)
    {
        ASSERT_TRUE(vectorContainsPoint(results, point));
    }
}

// Tests for dimension validation
TEST(test_dimension_validation)
{
    std::vector<std::vector<int>> points_2d = {
        {3, 6}, {17, 15}, {13, 15}};

    RangeTree<int, 2> tree(points_2d);

    // Test insufficient dimensions in search (should throw)
    try
    {
        tree.search({3});
        test_file << "TEST FAILED: " << __func__ << " Line " << __LINE__ << ": Expected exception for insufficient dimensions" << std::endl;
        failed_tests++;
        return;
    }
    catch (const std::invalid_argument &)
    {
        passed_assertions++;
    }

    // Test extra dimensions in search (should NOT throw with our new validation)
    try
    {
        tree.search({3, 6, 2});
        passed_assertions++; // Should pass with our relaxed validation
    }
    catch (const std::invalid_argument &)
    {
        test_file << "TEST FAILED: " << __func__ << " Line " << __LINE__ << ": Unexpected exception for extra dimensions" << std::endl;
        failed_tests++;
        return;
    }

    // Test insufficient dimensions in range search (should throw)
    try
    {
        tree.rangeSearch({3}, {6});
        test_file << "TEST FAILED: " << __func__ << " Line " << __LINE__ << ": Expected exception for insufficient dimensions" << std::endl;
        failed_tests++;
        return;
    }
    catch (const std::invalid_argument &)
    {
        passed_assertions++;
    }

    // Test extra dimensions in range search (should NOT throw with our new validation)
    try
    {
        tree.rangeSearch({3, 6, 2}, {6, 9, 5});
        passed_assertions++; // Should pass with our relaxed validation
    }
    catch (const std::invalid_argument &)
    {
        test_file << "TEST FAILED: " << __func__ << " Line " << __LINE__ << ": Unexpected exception for extra dimensions" << std::endl;
        failed_tests++;
        return;
    }
}

// Tests for different data types
TEST(test_different_data_types)
{
    std::vector<std::vector<float>> points_float = {
        {3.5f, 6.7f}, {17.2f, 15.3f}, {13.8f, 15.1f}, {6.4f, 12.9f}};

    RangeTree<float, 2> tree(points_float);

    // Test point search
    ASSERT_TRUE(tree.search({3.5f, 6.7f}));
    ASSERT_TRUE(tree.search({17.2f, 15.3f}));
    ASSERT_FALSE(tree.search({3.51f, 6.7f}));

    // Test range search
    auto results = tree.rangeSearch({3.0f, 6.0f}, {14.0f, 16.0f});
    ASSERT_EQUAL(results.size(), 3);
    ASSERT_TRUE(vectorContainsPoint(results, {3.5f, 6.7f}));
    ASSERT_TRUE(vectorContainsPoint(results, {13.8f, 15.1f}));
    ASSERT_TRUE(vectorContainsPoint(results, {6.4f, 12.9f}));
}

// Tests for large dataset
TEST(test_large_dataset)
{
    std::vector<std::vector<int>> points_large;

    // Create 100 points in a grid pattern
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            points_large.push_back({i, j});
        }
    }

    RangeTree<int, 2> tree(points_large);

    // Test point search for all points
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            ASSERT_TRUE(tree.search({i, j}));
        }
    }

    // Test range query for a subset of points
    auto results = tree.rangeSearch({3, 3}, {6, 6});
    ASSERT_EQUAL(results.size(), 16); // 4x4 grid from (3,3) to (6,6)

    // Test range query for all points
    results = tree.rangeSearch({0, 0}, {9, 9});
    ASSERT_EQUAL(results.size(), 100); // 10x10 grid
}

// Test for invalid input data
TEST(test_invalid_input)
{
    // Test with inconsistent dimensions
    std::vector<std::vector<int>> inconsistent_points = {
        {3, 6}, {17, 15}, {13} // Last point has wrong dimension
    };

    try
    {
        RangeTree<int, 2> tree(inconsistent_points);
        test_file << "TEST FAILED: " << __func__ << " Line " << __LINE__ << ": Expected exception for inconsistent dimensions" << std::endl;
        failed_tests++;
        return;
    }
    catch (const std::invalid_argument &)
    {
        passed_assertions++;
    }
}

// Test for inverted range boundaries
TEST(test_inverted_ranges)
{
    std::vector<std::vector<int>> points_2d = {
        {3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}};

    RangeTree<int, 2> tree(points_2d);

    // Test range query with inverted x bounds
    auto results = tree.rangeSearch({15, 5}, {5, 15});

    // This should return no results since low > high in x dimension
    ASSERT_EQUAL(results.size(), 0);
}

int main()
{
    test_file.open("range_tree_test_results.txt");

    if (!test_file.is_open())
    {
        std::cerr << "Error opening range_tree_test_results.txt!" << std::endl;
        return 1;
    }

    test_file << "Range Tree Unit Tests" << std::endl;
    test_file << "=====================" << std::endl
              << std::endl;

    // Run all tests
    RUN_TEST(test_empty_tree);
    RUN_TEST(test_1d_range_tree);
    RUN_TEST(test_2d_range_tree_basic);
    RUN_TEST(test_2d_range_queries);
    RUN_TEST(test_range_boundary_conditions);
    RUN_TEST(test_3d_range_tree);
    RUN_TEST(test_dimension_validation);
    RUN_TEST(test_different_data_types);
    RUN_TEST(test_large_dataset);
    RUN_TEST(test_invalid_input);
    RUN_TEST(test_inverted_ranges);

    // Output test summary
    test_file << std::endl;
    test_file << "Test Summary" << std::endl;
    test_file << "============" << std::endl;
    test_file << "Total Tests: " << total_tests << std::endl;
    test_file << "Passed Tests: " << passed_tests << std::endl;
    test_file << "Failed Tests: " << failed_tests << std::endl;
    test_file << "Passed Assertions: " << passed_assertions << std::endl;

    test_file.close();

    std::cout << "Tests completed. Results saved to range_tree_test_results.txt" << std::endl;

    return failed_tests;
}