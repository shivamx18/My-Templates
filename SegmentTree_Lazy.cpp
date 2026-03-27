ll n;
vl v;

struct segTree
{
  vl seg, lazy;

  segTree()
  {
    seg.assign(4*n, 0);
    lazy.assign(4*n, 0); // 👉 identity for ADD = 0
    build(0, 0, n - 1);
  }

  inline ll left(ll i) { return 2*i + 1; }
  inline ll right(ll i) { return 2*i + 2; }

  // 👉 CHANGE THIS for different queries (sum/min/max/gcd)
  inline ll merge(ll a, ll b)
  {
    return a + b; // change acc to the problem
  }

  // 👉 BUILD: depends on initial array
  void build(ll idx, ll low, ll high)
  {
    if(low == high)
    {
      seg[idx] = v[low];
      return;
    }

    ll mid = (low + high) / 2;
    build(left(idx), low, mid);
    build(right(idx), mid + 1, high);

    seg[idx] = merge(seg[left(idx)], seg[right(idx)]);
  }

  // 👉 APPLY lazy to current node
  void apply(ll idx, ll low, ll high, ll val)
  {
    seg[idx] += (high - low + 1) * val; // Change acc to the problem
  }

  // 👉 COMBINE lazy values
  void combineLazy(ll &old, ll val)
  {
    old += val; // change accordingly
  }

  // 👉 PUSH lazy down
  void push(ll idx, ll low, ll high)
  {
    if(lazy[idx] == 0) return; // 👉 identity check (ADD)

    apply(idx, low, high, lazy[idx]);

    if(low != high)
    {
      combineLazy(lazy[left(idx)], lazy[idx]);
      combineLazy(lazy[right(idx)], lazy[idx]);
    }

    lazy[idx] = 0; // reset
  }

  // 👉 RANGE UPDATE
  void update(ll idx, ll low, ll high, ll ql, ll qr, ll val)
  {
    push(idx, low, high);

    if(qr < low || high < ql) return;

    if(ql <= low && high <= qr)
    {
      combineLazy(lazy[idx], val);
      push(idx, low, high);
      return;
    }

    ll mid = (low + high) / 2;

    update(left(idx), low, mid, ql, qr, val);
    update(right(idx), mid+1, high, ql, qr, val);

    seg[idx] = merge(seg[left(idx)], seg[right(idx)]);
  }

  // 👉 RANGE QUERY
  ll query(ll idx, ll low, ll high, ll ql, ll qr)
  {
    push(idx, low, high);

    if(qr < low || high < ql) return 0; // 👉 change if NOT sum

    if(ql <= low && high <= qr) return seg[idx];

    ll mid = (low + high) / 2;

    return merge(
      query(left(idx), low, mid, ql, qr),
      query(right(idx), mid+1, high, ql, qr)
    );
  }
};
