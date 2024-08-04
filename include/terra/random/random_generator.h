/*
 *  random_generator.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      Header file that defines the RandomGenerator object.  This object will
 *      generate random numbers from one or two entropy sources.
 *
 *      If the constructor's pseudo_random_only argument is true, this object
 *      will generate random octets using C++'s std::mt19937 PRNG.
 *
 *      If the constructor's pseudo_random_only argument is false (default),
 *      the object will first read random octets from the operating system's
 *      random sources (and pseudo-random sources as a fallback, if available).
 *      In addition, it will then XOR those operating-system provided random
 *      octets with octets from C++'s std::mt19937 PRNG.  This will provide
 *      a greater degree of randomness in case one of the two sources has
 *      low entropy.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <random>
#include <cstdint>
#include <vector>
#include <cstddef>
#include <span>

namespace Terra::Random
{

class RandomGenerator
{
    public:
        RandomGenerator(bool pseudo_random_only = false);
        ~RandomGenerator();
        std::uint8_t GetRandomOctet() noexcept;
        std::vector<std::uint8_t> GetRandomOctets(std::size_t count);
        void GetRandomOctets(std::span<std::uint8_t> octets) noexcept;

    protected:
        std::uint8_t GetPseudoRandomOctet();
        std::size_t SourceRandomOctets(
                                std::span<std::uint8_t> buffer) const noexcept;

        bool pseudo_random_only;
        std::uniform_int_distribution<std::mt19937::result_type> distribution;
        std::mt19937 random_engine;

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        int random_fd;
        int pseudo_random_fd;
#endif
};

} // namespace Terra::Random
