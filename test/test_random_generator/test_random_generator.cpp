/*
 *  test_random_generator.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module contains tests for random generator class.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstddef>
#include <utility>
#include <terra/random/random_generator.h>
#include <terra/stf/stf.h>

using namespace Terra::Random;

// Chi-squared test to verify the histograms in the following tests
// prove the distribution appears to be uniform
std::pair<bool, bool> PerformChiSquaredTest(
                                    const std::vector<std::size_t> &histogram)
{
    const double Threshold = 293.25;            // alpha = 0.05
    const double Critical = 364.04;             // alpha = 0.00001
    const double Expected = 100.0;              // Buckets have ~100 in each
    double chi_squared = 0.0;

    // Ensure the histogram has the right size
    if (histogram.size() != 256) return {true, true};

    for (std::size_t count : histogram)
    {
        chi_squared += (count - Expected) * (count - Expected) / Expected;
    }

    return {chi_squared >= Threshold, chi_squared >= Critical};
}

// Check that there are not many buckets outside the expected range(this
// has been replaced with the Chi-squared test)
bool PerformRangeTest(const std::vector<std::size_t> &histogram)
{
    const std::size_t lower_bound = 65;
    const std::size_t upper_bound = 135;
    const std::size_t threshold = 2;
    std::size_t count = 0;

    for (auto value : histogram)
    {
        // Though rare, a bucket might have fewer elements -- retry
        if (value < lower_bound) count++;
        if (value > upper_bound) count++;
        if (count > threshold) return false;
    }

    return true;
}

// Test to ensure that the the PRNG routine(s) are seeding distinctly
STF_TEST(RandomGenerator, DistinctSeeding)
{
    bool seeding_unique = false;

    // Seeds should alway be distinct, but perform more than one test
    for (unsigned trials = 0; trials < 100; trials++)
    {
        RandomGenerator generator1;
        RandomGenerator generator2;

        // Generate two random values
        auto value1 = generator1.GetRandomOctet();
        auto value2 = generator2.GetRandomOctet();

        // These two values should be different
        if (value1 != value2)
        {
            seeding_unique = true;
            break;
        }
    }

    // Verify that the seeding appears to be distinct
    STF_ASSERT_TRUE(seeding_unique);
}

// Test to ensure that the the PRNG routine(s) are seeding distinctly
// using the C++ PRNG only
STF_TEST(RandomGenerator, DistinctSeedingPseudoRandom)
{
    bool seeding_unique = false;

    // Seeds should alway be distinct, but perform more than one test
    for (unsigned trials = 0; trials < 100; trials++)
    {
        RandomGenerator generator1(true);
        RandomGenerator generator2(true);

        // Generate two random values
        auto value1 = generator1.GetRandomOctet();
        auto value2 = generator2.GetRandomOctet();

        // These two values should be different
        if (value1 != value2)
        {
            seeding_unique = true;
            break;
        }
    }

    // Verify that the seeding appears to be distinct
    STF_ASSERT_TRUE(seeding_unique);
}

// Verify the the PRNG produces a uniform distribution
STF_TEST(RandomGenerator, UniformDistribution)
{
    constexpr std::size_t Trials = 5;
    constexpr std::size_t Max_Failures = 4;
    constexpr std::size_t Iterations = 25'600;
    std::vector<std::size_t> histogram(256);
    std::size_t failures = 0;

    // Test will be tried Trials times
    for (std::size_t trials = 0; trials < Trials; trials++)
    {
        RandomGenerator generator;

        // Initialize the histogram
        std::fill(histogram.begin(), histogram.end(), 0);

        // Generate 25'600 random octets
        for (std::size_t i = 0; i < Iterations; i++)
        {
            std::uint8_t value = generator.GetRandomOctet();
            histogram[value]++;
        }

        // The histogram should have about 100 in each bucket; the following
        // test will confirm that there is a uniform distribution
        auto [failure, critical] = PerformChiSquaredTest(histogram);
        if (failure) failures++;
        STF_ASSERT_FALSE(critical);
    }

    // Ensure the number of failures is not exceeded allowable
    STF_ASSERT_LE(failures, Max_Failures);
}

// Verify the the PRNG produces a uniform distribution (C++ PRNG only)
STF_TEST(RandomGenerator, UniformDistributionPseudoRandom)
{
    constexpr std::size_t Trials = 5;
    constexpr std::size_t Max_Failures = 4;
    constexpr std::size_t Iterations = 25'600;
    RandomGenerator generator(true);
    std::vector<std::size_t> histogram(256);
    std::size_t failures = 0;

    // Test will be tried Trials times
    for (std::size_t trials = 0; trials < Trials; trials++)
    {
        RandomGenerator generator;

        // Initialize the histogram
        std::fill(histogram.begin(), histogram.end(), 0);

        // Generate 25'600 random octets
        for (std::size_t i = 0; i < Iterations; i++)
        {
            std::uint8_t value = generator.GetRandomOctet();
            histogram[value]++;
        }

        // The histogram should have about 100 in each bucket; the following
        // test will confirm that there is a uniform distribution
        auto [failure, critical] = PerformChiSquaredTest(histogram);
        if (failure) failures++;
        STF_ASSERT_FALSE(critical);
    }

    // Ensure the number of failures is not exceeded allowable
    STF_ASSERT_LE(failures, Max_Failures);
}

// Verify the ability to retrieve a vector of random values in bulk
// and that those appear to have a uniform distribution (first of two
// overloaded functions named GetRandomOctets())
STF_TEST(RandomGenerator, GetVectorOfRandomOctets1)
{
    constexpr std::size_t Trials = 5;
    constexpr std::size_t Max_Failures = 4;
    constexpr std::size_t Count = 25'600;
    std::vector<std::size_t> histogram(256);
    std::size_t failures = 0;

    // Test will be tried Trials times
    for (std::size_t trials = 0; trials < Trials; trials++)
    {
        RandomGenerator generator(true);

        // Initialize the histogram
        std::fill(histogram.begin(), histogram.end(), 0);

        // Generate "Count" random octets
        std::vector<std::uint8_t> values = generator.GetRandomOctets(Count);
        STF_ASSERT_EQ(Count, values.size());
        for (auto value : values) histogram[value]++;

        // The histogram should have about 100 in each bucket; the following
        // test will confirm that there is a uniform distribution
        auto [failure, critical] = PerformChiSquaredTest(histogram);
        if (failure) failures++;
        STF_ASSERT_FALSE(critical);
    }

    // Ensure the number of failures is not exceeded allowable
    STF_ASSERT_LE(failures, Max_Failures);
}
