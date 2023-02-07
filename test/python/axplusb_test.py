import duckdb

def test_axplusb():
    conn = duckdb.connect('');
    conn.execute('SELECT axplusb() as value;');
    res = conn.fetchall()
    assert(res[0][0] == "I'm a axplusb!");