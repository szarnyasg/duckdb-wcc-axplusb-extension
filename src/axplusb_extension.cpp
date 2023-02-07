#define DUCKDB_EXTENSION_MAIN

#include "axplusb_extension.hpp"
#include "duckdb.hpp"

#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

namespace duckdb {

// axplusb(a, x, b): calculates a*x+b over GF(2^64).
// Irreducible polynomial: x^64 + x^4 + x^3 + x + 1
int64_t axplusb(int64_t a, int64_t x, int64_t b) {
	int64_t r = 0;
	while (x) {
		if (x & 1) {
			r ^= a;
		}
		x = (x >> 1) & 0x7fffffffffffffff;
		if (a & (1LL << 63)) {
			a = (a << 1) ^ 0x1b;
		} else {
			a <<= 1;
		}
	}
	return r ^ b;
}

inline void AxplusbScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
	D_ASSERT(args.ColumnCount() >= 3);
	errno = 0;
	auto a = FlatVector::GetData<int64_t>(args.data[0])[0];
	auto x = FlatVector::GetData<int64_t>(args.data[1]);
	auto b = FlatVector::GetData<int64_t>(args.data[2])[0];
	auto r = FlatVector::GetData<int64_t>(result);
	for (idx_t i = 0; i < args.size(); i++) {
		r[i] = axplusb(a, x[i], b);
	}
}

static void LoadInternal(DatabaseInstance &instance) {
	Connection con(instance);
    con.BeginTransaction();

    auto &catalog = Catalog::GetSystemCatalog(*con.context);

    CreateScalarFunctionInfo axplusb_fun_info(
            ScalarFunction("axplusb", {LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT}, LogicalType::BIGINT, AxplusbScalarFun)
        );
    axplusb_fun_info.on_conflict = OnCreateConflict::ALTER_ON_CONFLICT;
    catalog.CreateFunction(*con.context, &axplusb_fun_info);
    con.Commit();
}

void AxplusbExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string AxplusbExtension::Name() {
	return "axplusb";
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void axplusb_init(duckdb::DatabaseInstance &db) {
	LoadInternal(db);
}

DUCKDB_EXTENSION_API const char *axplusb_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
