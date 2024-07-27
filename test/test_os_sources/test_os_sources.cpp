/*
 *  test_os_sources.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      Test random generation from OS sources.
 *
 *  Portability Issues:
 *      None.
 */

#include <array>
#include <terra/random/random_generator.h>
#include <terra/stf/stf.h>

using namespace Terra::Random;

class RandomGenerator_ : public RandomGenerator
{
    public:
        using RandomGenerator::RandomGenerator;
        using RandomGenerator::SourceRandomOctets;
};

// Verify the OS RNG produces output
STF_TEST(RandomOSSources, VerifyContent)
{
    std::array<std::uint8_t, 10> octets;
    RandomGenerator_ rng;

    // Ensure we retrieve the expected number of octets
    STF_ASSERT_EQ(octets.size(), rng.SourceRandomOctets(octets));
}
