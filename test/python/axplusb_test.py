import duckdb

def test_axplusb():
    conn = duckdb.connect('');
    conn.execute('SELECT axplusb(1, 2, 3) as value;');
    res = conn.fetchall()
    assert(res[0][0] == 1);