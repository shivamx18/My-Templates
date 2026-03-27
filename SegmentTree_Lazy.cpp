ll n;
vl v;

struct segTree
{
  vl seg;
  vl lazy;
  segTree()
  {
    seg.assign(4*n, 0);
    lazy.assign(4*n, 0);
    build(0, 0, n - 1);
  }

  inline ll left(ll i) {return 2*i + 1;}
  inline ll right(ll i) {return 2*i + 2;}
  inline ll merge(ll a, ll b) {return a + b;} // Change Accordingly

  void build(ll idx, ll low, ll high)
  {
    if(low == high)
    {
      seg[idx] = v[low];
      return;
    }
    
    ll mid = low + (high - low) / 2;
    build(left(idx), low, mid);
    build(right(idx), mid + 1, high);

    seg[idx] = merge(seg[left(idx)], seg[right(idx)]);
  }

  void push(ll idx, ll low, ll high)
  {
    if(lazy[idx] == 1) return;
    seg[idx] = (lazy[idx] * (high - low + 1));

    if(low != high)
    {
      lazy[left(idx)]  += lazy[idx];
      lazy[right(idx)] += lazy[idx];
    }
    
    lazy[idx] = 0;
  }

  void update(ll idx, ll low, ll high, ll ql, ll qr, ll val)
  {
    push(idx, low, high);
    if(low > qr || high < ql) return;
    if(low >= ql && high <= qr) 
    {
      lazy[idx] = val;
      push(idx, low, high);
      return;
    }
    
    ll mid = low + (high - low) / 2;
    update(left(idx), low, mid, ql, qr, val);
    update(right(idx), mid + 1, high, ql, qr, val);

    seg[idx] = merge(seg[left(idx)], seg[right(idx)]);
  }
  
  ll query(ll idx, ll low, ll high, ll ql, ll qr)
  {
    push(idx, low, high);

    if(low > qr || high < ql) return 0;
    if(low >= ql && high <= qr) return seg[idx];
    
    ll mid = low + (high - low) / 2;
    return merge(query(left(idx), low, mid, ql, qr),
      query(right(idx), mid + 1, high, ql, qr)); 
  }
};
