#pragma once
#include "db/sqlite3/sqlite3.h"
#include "core/sqlite3.h"

namespace ema::db {
	class sqlite3cpp::impl {
public:
		impl() = default;
		~impl() { close(); }
		sqlite3_result open(const string& db, const sqlite3_open_flags flags);
		void close();
		string error();
		sqlite3_result begin();
		sqlite3_result commit();
		sqlite3_result rollback();
		sqlite3_result exec(const string& sql, sqlite3_rows& rows);
		sqlite3_result exec(const string& sql);
		sqlite3_result prepare(const string& sql, sqlite3_context& ctx);
		sqlite3_result bind_value(sqlite3_context& ctx, sqlite3_value&& value);
		sqlite3_result bind_values(sqlite3_context& ctx, std::vector<sqlite3_value>&& values);
		sqlite3_result step(sqlite3_context& ctx);
		sqlite3_result step(sqlite3_context& ctx, sqlite3_columns& cols);
		sqlite3_result finalize(sqlite3_context& ctx);

	   private:
		sqlite3* db_{nullptr};
	};
}
