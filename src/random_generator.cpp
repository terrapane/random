/*
 *  random_generator.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      Implementation file for the RandomGenerator object.
 *
 *  Portability Issues:
 *      None.
 */

#if defined(__unix__) || defined(__APPLE__)
#include <fcntl.h>
#include <unistd.h>
#endif
#ifdef _WIN32
// The following define is required as ntstatus.h would have already defined
// various status definitions and, without it, there are redefinition errors
#define WIN32_NO_STATUS
#include <Windows.h>
#include <bcrypt.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#endif
#include <chrono>
#include <cstdlib>
#include <terra/random/random_generator.h>

namespace Terra::Random
{

/*
 *  RandomGenerator::RandomGenerator()
 *
 *  Description:
 *      Constructor for the RandomGenerator.
 *
 *  Parameters:
 *      pseudo_random_only [in]
 *          If true, use the C++ PRNG only.  This is faster, but less ideal than
 *          using OS-provided random sources in addition to the C++ PRNG.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
RandomGenerator::RandomGenerator(bool pseudo_random_only) :
    pseudo_random_only(pseudo_random_only),
    distribution(0, 255),
    random_engine{static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count())}
{
#if defined(__unix__) || defined(__APPLE__)
    if (pseudo_random_only)
    {
        random_fd = pseudo_random_fd = -1;
    }
    else
    {
        // Open /dev/random, which will yield the best source of random values
        random_fd = open("/dev/random", O_NONBLOCK | O_RDONLY |  O_CLOEXEC);

        // Open /dev/urandom, which will yield pseudo-random values
        pseudo_random_fd =
            open("/dev/urandom", O_NONBLOCK | O_RDONLY | O_CLOEXEC);
    }
#endif

    try
    {
        // Define the random device
        std::random_device rd;

        // Re-seed with random device (may throw an exception in rare cases);
        // note that the engine has already been seeded with the current
        // time, so no need to re-seed if there there is no entropy
        if (rd.entropy() > 0)
        {
            random_engine.seed(static_cast<std::mt19937::result_type>(rd()));
        }
    }
    catch (...)
    {
        // This code is very unlikely to ever be called since it's very
        // unusual for std::random_device() to fail, but reseed if it happens
        random_engine.seed(static_cast<std::mt19937::result_type>(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    }
}

/*
 *  RandomGenerator::~RandomGenerator()
 *
 *  Description:
 *      Destructor for the RandomGenerator object.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
RandomGenerator::~RandomGenerator()
{
#if defined(__unix__) || defined(__APPLE__)
    // Close the random file sources if they are open
    if (random_fd >= 0) close(random_fd);
    if (pseudo_random_fd >= 0) close(pseudo_random_fd);
#endif
}

/*
 *  RandomGenerator::GetRandomOctet
 *
 *  Description:
 *      Get a single random octet.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
std::uint8_t RandomGenerator::GetRandomOctet() noexcept
{
    std::uint8_t octet = 0;

    // Source one random octet from the operating system
    SourceRandomOctets({&octet, 1});

    // XOR that value with the C++ pseudo-random number generator
    octet ^= GetPseudoRandomOctet();

    return octet;
}

/*
 *  RandomGenerator::GetRandomOctets
 *
 *  Description:
 *      Get multiple random octets.
 *
 *  Parameters:
 *      count [in]
 *          Number of random octets to return.
 *
 *  Returns:
 *      A vector of random octets of the requested count.
 *
 *  Comments:
 *      None.
 */
std::vector<std::uint8_t> RandomGenerator::GetRandomOctets(std::size_t count)
{
    std::vector<std::uint8_t> octets(count);

    GetRandomOctets(octets);

    return octets;
}

/*
 *  RandomGenerator::GetRandomOctets
 *
 *  Description:
 *      Get multiple random octets.
 *
 *  Parameters:
 *      octets [out]
 *          A span into which random octets will be written.
 *
 *  Returns:
 *      Nothing, though the requested number of random octets will be placed
 *      in the span of "octets".
 *
 *  Comments:
 *      None.
 */
void RandomGenerator::GetRandomOctets(std::span<std::uint8_t> octets) noexcept
{
    // If requesting no values, return early
    if (octets.empty()) return;

    // Source some random values from the operating system
    SourceRandomOctets(octets);

    // XOR each of the random octets with octets from the C++ pseudo-random
    // number generator
    for (auto &octet : octets) octet ^= GetPseudoRandomOctet();
}

/*
 *  RandomGenerator::GetPseudoRandomOctet
 *
 *  Description:
 *      Get a single random octet.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
std::uint8_t RandomGenerator::GetPseudoRandomOctet() noexcept
{
    return static_cast<std::uint8_t>(distribution(random_engine));
}

/*
 *  RandomGenerator::SourceRandomOctets
 *
 *  Description:
 *      Source the specified number of octets from the random and/or
 *      pseudo-random operating system sources.
 *
 *  Parameters:
 *      buffer [in]
 *          A span of octets into which random numbers will be placed.
 *
 *  Returns:
 *      A count of the number of octets placed into the buffer.  This count may
 *      be smaller than the size of the span.
 *
 *  Comments:
 *      None.
 */
std::size_t RandomGenerator::SourceRandomOctets(
                                std::span<std::uint8_t> buffer) const noexcept
{
    std::size_t octets_sourced = 0;

    // If the count is zero or using the C++ PRNG, just return
    if (buffer.empty() || pseudo_random_only) return 0;

#if defined(__unix__) || defined(__APPLE__)
    // Attempt to read random values
    if (random_fd >= 0)
    {
        auto result = read(random_fd, buffer.data(), buffer.size());
        if (result > 0) octets_sourced += result;
    }

    // Attempt to read pseudo-random values if more octets are needed
    if ((octets_sourced < buffer.size()) && (pseudo_random_fd >= 0))
    {
        auto result = read(pseudo_random_fd,
                           buffer.data() + octets_sourced,
                           buffer.size() - octets_sourced);
        if (result > 0) octets_sourced += result;
    }
#elif defined(_WIN32)
    // Source random octets from Windows' NIST SP800-90 compliant RNG, or
    // prior to Vista SP1, a FIPS 186-2 compliant RNG
    if (BCryptGenRandom(NULL,
                        reinterpret_cast<PUCHAR>(buffer.data()),
                        static_cast<ULONG>(buffer.size()),
                        BCRYPT_USE_SYSTEM_PREFERRED_RNG) == STATUS_SUCCESS)
    {
        octets_sourced = buffer.size();
    }
#endif

    return octets_sourced;
}

} // namespace Terra::Random
