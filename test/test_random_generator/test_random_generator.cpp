/*
 *  test_random_generator.cpp
 *
 *  Copyright (C) 2024
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

#include <terra/random/random_generator.h>
#include <terra/stf/stf.h>

using namespace Terra::Random;

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
    constexpr unsigned Retry_Count = 5;
    RandomGenerator generator;
    std::vector<std::size_t> histogram(256);
    unsigned trials;

    // Test will be tried Retry_Count times
    for (trials = 0; trials < Retry_Count; trials++)
    {
        bool retry = false;

        // Generate 25'600 random octets
        for (auto i = 0; i < 25'600; i++)
        {
            std::uint8_t value = generator.GetRandomOctet();
            histogram[value]++;
        }

        // Given a uniform distribution, each bucket of the histogram should
        // have about 100 elements in it; assume there are at least 70
        for (auto i = 0; i < 256; i++)
        {
            // Though rare, a bucket might have fewer elements. In that case,
            // try it a second time
            if (histogram[i] < 70) retry = true;
            histogram[i] = 0;
        }

        // Stop if another trail is not needed
        if (!retry) break;
    }

    // Ensure that the trail count was not exhausted.
    STF_ASSERT_NE(trials, Retry_Count);
}

// Verify the the PRNG produces a uniform distribution (C++ PRNG only)
STF_TEST(RandomGenerator, UniformDistributionPseudoRandom)
{
    constexpr unsigned Retry_Count = 5;
    RandomGenerator generator(true);
    std::vector<std::size_t> histogram(256);
    unsigned trials;

    // Test will be tried Retry_Count times
    for (trials = 0; trials < Retry_Count; trials++)
    {
        bool retry = false;

        // Generate 25'600 random octets
        for (auto i = 0; i < 25'600; i++)
        {
            std::uint8_t value = generator.GetRandomOctet();
            histogram[value]++;
        }

        // Given a uniform distribution, each bucket of the histogram should
        // have about 100 elements in it; assume there are at least 70
        for (auto i = 0; i < 256; i++)
        {
            // Though rare, a bucket might have fewer elements. In that case,
            // try it a second time
            if (histogram[i] < 70) retry = true;
            histogram[i] = 0;
        }

        // Stop if another trail is not needed
        if (!retry) break;
    }

    // Ensure that the trail count was not exhausted.
    STF_ASSERT_NE(trials, Retry_Count);
}

// Verify the ability to retrieve a vector of random values in bulk
// and that those appear to have a uniform distribution (first of two
// overloaded functions named GetRandomOctets())
STF_TEST(RandomGenerator, GetVectorOfRandomOctets1)
{
    constexpr unsigned Retry_Count = 5;
    RandomGenerator generator(true);
    std::vector<std::size_t> histogram(256);
    unsigned trials;

    // Test will be tried Retry_Count times
    for (trials = 0; trials < Retry_Count; trials++)
    {
        bool retry = false;

        // Generate 25'600 random octets
        std::vector<std::uint8_t> values = generator.GetRandomOctets(25'600);
        STF_ASSERT_EQ(25'600, values.size());
        for (auto value : values) histogram[value]++;

        // Given a uniform distribution, each bucket of the histogram should
        // have about 100 elements in it; assume there are at least 70
        for (auto i = 0; i < 256; i++)
        {
            // Though rare, a bucket might have fewer elements -- retry
            if (histogram[i] < 70) retry = true;
            histogram[i] = 0;
        }

        // Stop if another trail is not needed
        if (!retry) break;
    }

    // Ensure that the trail count was not exhausted.
    STF_ASSERT_NE(trials, Retry_Count);
}

// Verify the ability to retrieve a vector of random values in bulk
// and that those appear to have a uniform distribution (second of two
// overloaded functions named GetRandomOctets())
STF_TEST(RandomGenerator, GetVectorOfRandomOctets2)
{
    constexpr unsigned Retry_Count = 5;
    RandomGenerator generator(true);
    std::vector<std::size_t> histogram(256);
    unsigned trials;

    // Test will be tried Retry_Count times
    for (trials = 0; trials < Retry_Count; trials++)
    {
        bool retry = false;

        // Generate 25'600 random octets
        auto values = generator.GetRandomOctets(25'600);
        for (auto value : values) histogram[value]++;

        // Given a uniform distribution, each bucket of the histogram should
        // have about 100 elements in it; assume there are at least 70
        for (auto i = 0; i < 256; i++)
        {
            // Though rare, a bucket might have fewer elements -- retry
            if (histogram[i] < 70) retry = true;
            histogram[i] = 0;
        }

        // Stop if another trail is not needed
        if (!retry) break;
    }

    // Ensure that the trail count was not exhausted.
    STF_ASSERT_NE(trials, Retry_Count);
}

// Verify we can fill a span of octets with random data
STF_TEST(RandomGenerator, GetVectorOfRandomOctets3)
{
    constexpr unsigned Retry_Count = 5;
    RandomGenerator generator(true);
    std::vector<std::size_t> histogram(256);
    unsigned trials;

    // Test will be tried Retry_Count times
    for (trials = 0; trials < Retry_Count; trials++)
    {
        bool retry = false;

        // Generate 25'600 random octets
        std::vector<std::uint8_t> values(25'600);
        generator.GetRandomOctets(values);
        for (auto value : values) histogram[value]++;

        // Given a uniform distribution, each bucket of the histogram should
        // have about 100 elements in it; assume there are at least 70
        for (auto i = 0; i < 256; i++)
        {
            // Though rare, a bucket might have fewer elements -- retry
            if (histogram[i] < 70) retry = true;
            histogram[i] = 0;
        }

        // Stop if another trail is not needed
        if (!retry) break;
    }

    // Ensure that the trail count was not exhausted.
    STF_ASSERT_NE(trials, Retry_Count);
}
