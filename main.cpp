#include <iostream>
#include <vector>
#include <array>
#include <stdlib.h>     /* abs */
#include <math.h>       /* log2 */


bool profilerMode = false;

// https://stackoverflow.com/questions/14997165/fastest-way-to-get-a-positive-modulo-in-c-c
unsigned long positiveModulo(long value, unsigned long m) {
    int mod = value % (int) m;
    if (mod < 0) {
        mod += m;
    }
    return mod;
}

// http://cplusplus.com/forum/general/228429/
unsigned long cdn( unsigned long c, unsigned long d, unsigned long n )      // work out c^d mod n
{
    unsigned long value = 1;
    while( d > 0 )
    {
        value *= c;
        value %= n;
        d--;
    }
    return value;
}

bool isPrime(int num) {
    if (num == 1) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

std::vector<int> findPrimes(int start = 1, int count = 10) {
    std::vector<int> primes;
    for (int i = start; primes.size() < count; ++i) {
        if (isPrime(i)) {
            primes.push_back(i);
        }
    }
    return primes;
}

int calcEuler(int p, int q) {
    return (p - 1) * (q - 1);
}

std::array<long, 3> egcd(long a, long b) {
    if (a == 0)
        return {b, 0, 1};
    std::array<long, 3> ret = egcd(b % a, a);

    long x = ret[2] - (b / a) * ret[1];
    long y = ret[1];
    return {ret[0], x, y};
}

// returns: e, d, N
std::array<unsigned long, 3> genRSA(unsigned prime_start = 10, unsigned prime_count = 50) {
    std::vector<int> primes = findPrimes(prime_start, prime_count);
    if (profilerMode) {
        srand(NULL);
    } else {
        srand((unsigned) time(NULL));
    }

    // find p & q
    unsigned int p = primes[rand() % primes.size()];
    unsigned int q;
    while (true) {
        q = primes[rand() % primes.size()];
        if (q >= p) continue;
        // check bitsize
        double log2pq = abs(log2(p) - log2(q));
        if (0.1 < log2pq && log2pq < 30) break;
    }
//    std::cout << p << p; // make sure compiler does not optimize those calculations away
//    p=31237;
//    q=17863;


    unsigned long N = p * q;
    unsigned long phi_N = calcEuler(p, q);


    // find e
    unsigned int e;
    while (true) {
        e = primes[rand() % primes.size()];
        if (egcd(e, phi_N)[0] != 1) continue;
        if (e != 1 && e < phi_N) break; // 1 < e < phi_N
    }
//    std::cout << e; // make sure compiler does not optimize those calculations away
//    e=83701;


    // calculate multiplicative inverse (and k, which isnt really relevant)
    std::array<long, 3> egcd_result = egcd(e, phi_N);
    unsigned long d = positiveModulo(egcd_result[1], phi_N);
    long k = egcd_result[2];

    if (!profilerMode) {
        std::cout << "p: " << p << "\n";
        std::cout << "q: " << q << "\n";
        std::cout << "N: " << N << "\n";
        std::cout << "phi_N: " << phi_N << "\n";
        std::cout << "e: " << e << "\n";
        std::cout << "d: " << d << "\n";
        std::cout << "k: " << k << "\n";
        std::cout << "ggt e,phi_N (has to be 1): " << egcd_result[0] << "\n";
    }

    return {e, d, N};
}

std::vector<unsigned long> encString(std::string m, unsigned long e, unsigned long N) {
    std::vector<char8_t> char_string(m.begin(), m.end());

    std::vector<unsigned long> c;
    for (char8_t character : char_string) {
        c.push_back(cdn(character, e, N));
    }
    return c;
}

std::string decString(std::vector<unsigned long> c, unsigned long d, unsigned long N) {
    std::vector<char8_t> m2;
    for (unsigned long cipher : c) {
        m2.push_back(cdn(cipher, d, N));
    }
    return std::string(m2.begin(), m2.end());
}



int main() {
    std::cout << "https://www.jetbrains.com/help/clion/cpu-profiler.html#Prerequisites";

    std::array<unsigned long, 3> rsa = genRSA(1000, 8000);
    unsigned long e = rsa[0];
    unsigned long d = rsa[1];
    unsigned long N = rsa[2];

    if (!profilerMode) {
        std::cout << "-------------\n";
        std::cout << "public key " << e << "\n";
        std::cout << "private key " << d << "\n";
        std::cout << "N " << N << "\n";
        std::cout << "-------------\n";


//    // sample testing with one char
//    char8_t m = 'a';
//    unsigned long c = cdn(m,rsa[0],rsa[2]);
//    std::cout << "m: " << (char)m << "(" << m << ")\n";
//    std::cout << "c: " << (char)c << "(" << c << ")\n";
//
//    char8_t m2 = cdn(c, rsa[1], rsa[2]);
//    std::cout << "m': " << (char)m2 << "(" << m2 << ")\n";


    // demo with string
        std::string message = "Lorem ipsum dolor sit amet";
        std::cout << "message: " << message << "\n";

        // enc
        std::vector<unsigned long> c_msg = encString(message, e, N);
        std::cout << "encoded message: ";
        for (unsigned long c_element : c_msg) std::cout << c_element << "  ";
        std::cout << "\n";

        //dec
        std::string m_decoded = decString(c_msg, d, N);
        std::cout << "decoded message: " << m_decoded;
    } else {

        // for profiler
        std::string message(5, '*');
        std::vector<unsigned long> c_msg = encString(message, e, N);
        std::string m_decoded = decString(c_msg, d, N);
        std::cout << m_decoded;  // make sure compiler does not optimize something away
    }

    return 0;
}
