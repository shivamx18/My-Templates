/**************************** STRING HASHING ******************************/

/*
 * Polynomial String Hashing (with Double Hashing for collision resistance)
 *
 *  - Precomputes powers of a base (31) modulo large primes.
 *  - Supports O(1) substring hash queries using prefix hashes.
 *  - Uses modular inverses so substring hashes are position-independent.
 *
 *  Key idea: 
 *    hash(s[l..r]) = (prefix[r] - prefix[l-1]) * invBase^l
 *
 *  Example usage:
 *    Hashing hs("abacaba");
 *    auto h1 = hs.substringHashPair(0, 2); // hash of "aba"
 *    auto h2 = hs.substringHashPair(4, 6); // hash of "aba"
 *    if(h1 == h2) -> substrings are equal
 *
 *  Supports:
 *    - substringHashVec(l, r): returns hash as vector<ll> (for N primes)
 *    - substringHashPair(l, r): optimized pair<ll,ll> (for 2 primes)
 */

struct Hashing
{
  string str;   // the input string
  ll n;         // length of string
  const ll base = 31; // base for polynomial hashing (common choice: 31 or 131)
  
  // Two large primes for double hashing (reduces probability of collisions)
  vl hashPrimes = {1000000009, 100000007};
  
  // Precomputed values:
  vvl hashValues;         // prefix hashes for each prime
  vvl powersOfBase;       // base^i % prime
  vvl inversePowersOfBase;// modular inverses of base^i % prime

  // Constructor: preprocess everything in O(n * primes)
  Hashing(string a)
  {
    str = a;
    n = str.length();
    ll primes = (ll)hashPrimes.size();

    // allocate memory
    hashValues.resize(primes);
    powersOfBase.resize(primes);
    inversePowersOfBase.resize(primes);

    // Precompute base powers and inverse base powers
    rep(i, 0, primes)
    {
      powersOfBase[i].resize(n + 1);
      inversePowersOfBase[i].resize(n + 1);
      powersOfBase[i][0] = 1;

      // base^j % prime
      rep(j, 1, n + 1)
        powersOfBase[i][j] = (base * powersOfBase[i][j - 1]) % hashPrimes[i];

      // inverse of base^j % prime (using Fermat's Little Theorem)
      inversePowersOfBase[i][n] = mminvprime(powersOfBase[i][n], hashPrimes[i]);
      rrep(j, 0, n - 1)
        inversePowersOfBase[i][j] = mulMod(inversePowersOfBase[i][j + 1], base, hashPrimes[i]);
    }

    // Precompute prefix hashes
    rep(i, 0, primes)
    {
      hashValues[i].resize(n);
      rep(j, 0, n)
      {
        // Map character to number: 'a' -> 1, 'b' -> 2, ...
        // You can change this mapping for wider char sets
        hashValues[i][j] = ((str[j] - 'a' + 1LL) * powersOfBase[i][j]) % hashPrimes[i];

        // build prefix sum
        hashValues[i][j] = (hashValues[i][j] + (j > 0 ? hashValues[i][j - 1] : 0LL)) % hashPrimes[i];
      }
    }
  }

  // Get substring hash [l, r] as vector (supports arbitrary number of primes)
  vl substringHashVec(ll l, ll r)
  {
    vl hash(hashPrimes.size());
    rep(i, 0, hashPrimes.size())
    {
      ll val1 = hashValues[i][r];
      ll val2 = (l > 0 ? hashValues[i][l - 1] : 0LL);

      // Normalize with inverse power so that hash is position-independent
      hash[i] = mulMod(subMod(val1, val2, hashPrimes[i]), inversePowersOfBase[i][l], hashPrimes[i]);
    }

    return hash;
  }

  // Optimized: return substring hash [l, r] as pair (works if primes >= 2)
  pair<ll,ll> substringHashPair(ll l, ll r)
  {
    ll h1, h2;

    { // prime 1
      ll val1 = hashValues[0][r];
      ll val2 = (l > 0 ? hashValues[0][l - 1] : 0LL);
      h1 = mulMod(subMod(val1, val2, hashPrimes[0]), inversePowersOfBase[0][l], hashPrimes[0]);
    }
    { // prime 2
      ll val1 = hashValues[1][r];
      ll val2 = (l > 0 ? hashValues[1][l - 1] : 0LL);
      h2 = mulMod(subMod(val1, val2, hashPrimes[1]), inversePowersOfBase[1][l], hashPrimes[1]);
    }

    return {h1, h2};
  }
};

/********************* HASH HELPERS FOR UNORDERED_* *********************/

/*
 * Default C++ unordered_map/set are vulnerable to "hash hacks"
 * (worst-case O(n^2) if someone forces collisions).
 *
 * These helpers provide safe hashing for custom types:
 *   - custom_hash   : integers (64-bit safe)
 *   - pair_hash     : pair<T1,T2>
 *   - vector_hash   : vector<ll>
 *
 * Use them with unordered_map/unordered_set to avoid collisions.
 */

// Universal integer hash (safe against collisions)
struct custom_hash
{
  static uint64_t splitmix64(uint64_t x)
  {
    // A strong mixing function
    x += 0x9e3779b97f4a7c15;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
  }
  size_t operator()(uint64_t x) const
  {
    static const uint64_t FIXED_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
    return splitmix64(x + FIXED_RANDOM);
  }
};

// Hash for pair<T,U>
struct pair_hash
{
  template<class T1, class T2>
  size_t operator()(const pair<T1,T2>& p) const
  {
    auto h1 = custom_hash{}((uint64_t)p.first);
    auto h2 = custom_hash{}((uint64_t)p.second);
    return h1 ^ (h2 << 1LL);
  }
};

// Hash for vector<ll>
struct vector_hash
{
  size_t operator()(const vl &v) const
  {
    size_t h = 0;
    for(auto &it : v)
    {
      // combine hashes like boost::hash_combine
      h ^= custom_hash{}((uint64_t)it) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    return h;
  }
};

/******** Aliases for convenience ********/
template<class T>
using uset = unordered_set<T, custom_hash>;                // unordered_set<int>
template<class K, class V>
using umap = unordered_map<K, V, custom_hash>;             // unordered_map<int,int>

using uset_pairll = unordered_set<pair<ll,ll>, pair_hash>; // unordered_set<pair<ll,ll>>
using umap_pairll = unordered_map<pair<ll,ll>, ll, pair_hash>;

using uset_vl = unordered_set<vl, vector_hash>;            // unordered_set<vector<ll>>
using umap_vl_ll = unordered_map<vl, ll, vector_hash>;

/****************************** DEMO FUNCTION ******************************/

void letsGo()
{
  string str = "abacaba";
  Hashing hs(str);

  /*
  // === Example 1: Get hash of substrings ===
  auto h1 = hs.substringHashPair(0, 2); // "aba" from [0..2]
  auto h2 = hs.substringHashPair(4, 6); // "aba" from [4..6]
  if(h1 == h2) cout << "Substrings are equal\n";
  else cout << "Different substrings\n";

  // === Example 2: Using vector-hash version (supports any # of primes) ===
  vl hv1 = hs.substringHashVec(0, 3); // hash of "abac"
  vl hv2 = hs.substringHashVec(3, 7); // hash of "caba"
  if(hv1 == hv2) cout << "Equal (unlikely)\n";

  // === Example 3: Store substring hashes in unordered_set<pair<ll,ll>> ===
  uset_pairll seen;
  for(int i = 0; i + 2 < str.size(); i++)
  {
    seen.insert(hs.substringHashPair(i, i+2)); // all substrings of length 3
  }
  cout << "Unique substrings of length 3: " << seen.size() << nl;

  // === Example 4: Store substring hashes in unordered_set<vl> ===
  uset_vl seenVec;
  for(int i = 0; i + 2 < str.size(); i++)
  {
    seenVec.insert(hs.substringHashVec(i, i+2));
  }
  cout << "Unique substrings of length 3 (vec): " << seenVec.size() << nl;

  // === Example 5: unordered_map with pair<ll,ll> key ===
  umap_pairll freq;
  for(int i = 0; i + 2 < str.size(); i++)
  {
    freq[hs.substringHashPair(i, i+2)]++;
  }
  for(auto &it : freq)
  {
    cout << "hash: (" << it.first.first << "," << it.first.second 
         << ") -> freq: " << it.second << nl;
  }

  // === Example 6: unordered_map with vector<ll> key ===
  umap_vl_ll freqVec;
  for(int i = 0; i + 2 < str.size(); i++)
  {
    freqVec[hs.substringHashVec(i, i+2)]++;
  }
  cout << "Stored " << freqVec.size() << " unique vector-hashes\n";
  */
}
